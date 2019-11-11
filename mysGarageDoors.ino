/*
 * MysGarageDoors
 * Copyright (C) 2017-2019 Pierre Wieser <pwieser@trychlos.org>
 *
 * Description:
 * Manages in one MySensors-compatible board the two garage doors.
 *
 * Radio module:
 * Is implemented with a NRF24L01+ radio module
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * NOTE: this sketch has been written for MySensor v 2.1 library.
 * 
 * For each garage door:
 * - we have a command to open/close the door
 * - we are able to detect:
 *   > the opening/closing moves (when the motor is active)
 *   > when the door is closed (via a fixed contact).
 * 
 * From the motor detection, and by measuring the move duration, we are able to
 * roughly estimate the position of the door.
 * 
 * We cannot know if the door is fully opened without instrumenting the motor box.
 * But instrumenting the door itself via a dry contact let us know is the door is
 * fully closed.
 * 
 * Commands are accepted: see README.
 *   
 * A learning program is defined.
 * It is expected to be run once, when the board is put into service:
 * - the two doors are expected to be fully closed;
 * - for each of the two doors, the learning program will fully open and then fully
 *   close the door.
 * This sequence is used to estimate the duration of a full move (opening/closing).
 * The four measures are recorded in the EEPROM as four unsigned longs.
 * The average is used to estimate the position of the door.
 * 
 * pwi 2017- 4-26 creation
 */

// uncomment for debugging this sketch
#define SKETCH_DEBUG

static char const sketchName[] PROGMEM    = "mysGarageDoors";
static char const sketchVersion[] PROGMEM = "2.0-2019";

#define MY_REPEATER_FEATURE
#define MY_RADIO_NRF24
#define MY_RF24_PA_LEVEL RF24_PA_HIGH
//#include <pwi_myhmac.h>
#include <pwi_myrf24.h>
#include <MySensors.h>

MyMessage msg;

enum {
    CHILD_MAIN = 1,
    CHILD_ID_DOOR1 = 20,
    CHILD_ID_DOOR2 = 40,
};

/* Include our own classes
 */
#include <pwiCommon.h>
#include "Eeprom.h"
static Eeprom eeprom;

#include <pwiTimer.h>
static pwiTimer st_main_timer;
static pwiTimer st_config_timer;
static pwiTimer st_learning_oc_timer;
static pwiTimer st_learning_bd_timer;

/* **************************************************************************************
 * Door management
 * - up to two doors
 */
#include "Door.h"
#include "learning.h"
#include "hub.h"

// core hub
static hub_t hub;

// Doors objects
Door door1( CHILD_ID_DOOR1,  5,  2,  3,  4,  6 );
Door door2( CHILD_ID_DOOR2, A2, A5, A4, A3, A1 );

// Learning management
// Learning program may be run from a received command
//  or by briefly pulsing the LEARNING_PIN to LOW
//
static learning_t st_learning;
#define LEARNING_PIN A0

void doorGetElapsed( uint8_t id, unsigned long *open, unsigned long *close )
{
    if( id == door1.getId()){
       *open = eeprom.d.door1_opening_ms;
       *close = eeprom.d.door1_closing_ms;
    } else if( id == door2.getId()){
       *open = eeprom.d.door2_opening_ms;
       *close = eeprom.d.door2_closing_ms;
    } else {
        *open = 0;
        *close = 0;
    }
}

/* **************************************************************************************
 *  CHILD_MAIN Sensor
 */

void mainPresentation()
{
#ifdef SKETCH_DEBUG
    Serial.println( F( "mainPresentation()" ));
#endif
    //                                1234567890123456789012345
    present( CHILD_MAIN,   S_CUSTOM, "Actions" );
    present( CHILD_MAIN+1, S_CUSTOM, "UnchangedTimeout" );
    present( CHILD_MAIN+2, S_CUSTOM, "SensorsReactTime" );
    present( CHILD_MAIN+3, S_CUSTOM, "OpenCloseLearnDelay" );
    present( CHILD_MAIN+4, S_CUSTOM, "InterDoorsLearnDelay" );
    present( CHILD_MAIN+5, S_CUSTOM, "DebounceDelay" );
    present( CHILD_MAIN+6, S_CUSTOM, "ButtonPushDelay" );
    present( CHILD_MAIN+7, S_CUSTOM, "MinPublishedChange" );
}

void mainSetup()
{
#ifdef SKETCH_DEBUG
    Serial.println( F( "mainSetup()" ));
#endif
    pinMode( LEARNING_PIN, INPUT_PULLUP );
}

/* **************************************************************************************
 *  MAIN CODE
 */
void presentation()
{
#ifdef SKETCH_DEBUG
    Serial.println( F( "presentation()" ));
#endif
    mainPresentation();
    door1.present();
    door2.present();
}

void setup()  
{
#ifdef SKETCH_DEBUG
    Serial.begin( 115200 );
    Serial.println( F( "setup()" ));
#endif
    sendSketchInfo( PGMSTR( sketchName ), PGMSTR( sketchVersion ));

    // library version
    msg.clear();
    mSetCommand( msg, C_INTERNAL );
    sendAsIs( msg.setSensor( 255 ).setType( I_VERSION ).set( MYSENSORS_LIBRARY_VERSION ));

    memset( &hub, '\0', sizeof( hub_t ));
    hub.average_ms = 0;
    hub.door1 = &door1;
    hub.door2 = &door2;
    hub.eeprom = &eeprom;
    hub.oc_timer = &st_learning_oc_timer;
    hub.bd_timer = &st_learning_bd_timer;
    hub.learning = &st_learning;

    eeprom.read();
    hub.average_ms = eeprom.computeAverage();

    mainSetup();
    door1.setup( &hub );
    door2.setup( &hub );
    
    st_main_timer.setup( "MainTimer", eeprom.d.react_time_ms, false, main_loop );
    st_main_timer.start();
    st_config_timer.setup( "ConfigTimer", eeprom.d.unchanged_timeout_ms, false, onUnchangedConfigCb );
    st_config_timer.start();

    memset( &st_learning, '\0', sizeof( learning_t ));
    
    st_learning_oc_timer.setup( "LearningOpenCloseTimer", eeprom.d.learning_oc_wait_ms, true, learning_onOpenCloseCb, &hub );
    st_learning_bd_timer.setup( "LearningBetweenDoorsTimer", eeprom.d.learning_bd_wait_ms, true, learning_onBetweenDoorsCb, &hub );

    pwiTimer::Dump();
    dumpConfiguration();
}

void loop()
{
    pwiTimer::Loop();
}

/* The st_main_timer callback
 *  It is defined as recurrent, on react_time_ms period.
 */
void main_loop( void *empty )
{
    uint8_t learning_phase = eeprom.d.learning_phase;

#ifdef SKETCH_DEBUG
    if( learning_phase != LEARNING_NONE ){
        learning_dump( &hub );
    }
#endif
    door1.runLoop( &hub );
    door2.runLoop( &hub );

    learning_runProgram( &hub, learning_phase );

    if( digitalRead( LEARNING_PIN ) == LOW ){
        learning_startProgram( &hub );
    }
}

/*
 * Handle C_SET/C_REQ requests:
 * see README.
 */
void receive(const MyMessage &message)
{
    char payload[2*MAX_PAYLOAD+1];
    uint8_t cmd = message.getCommand();
    uint8_t req, uint;
    unsigned long ulong;
    memset( payload, '\0', sizeof( payload ));
    message.getString( payload );
#ifdef SKETCH_DEBUG
    Serial.print( F( "[receive] sensor=" )); Serial.print( message.sensor );
    Serial.print( F( ", cmd=" )); Serial.print( cmd );
    Serial.print( F( ", message=" )); Serial.print( message.type );
    Serial.print( F( ", payload='" )); Serial.print( payload ); Serial.println( "'" ); 
#endif

    switch( message.sensor ){
        case CHILD_MAIN:
            switch( cmd ){
                case C_REQ:
                    switch( message.type ){
                        case V_CUSTOM:
                            req = strlen( payload ) > 0 ? atoi( payload ) : 0;
                            switch( req ){
                                case 1:
                                    eeprom.reset();
                                    globalSendStatus();
                                    break;
                                case 2:
                                    globalSendStatus();
                                    break;
                                case 3:
                                    learning_runProgram( &hub, eeprom.d.learning_phase );
                                    break;
                            }
                    }
            }
            break;

        case CHILD_MAIN+1:
            switch( cmd ){
                case C_SET:
                    switch( message.type ){
                        case V_CUSTOM:
                            ulong = strlen( payload ) ? atol( payload ) : 0;
                            if( ulong > 0 ){
                                eeprom.d.unchanged_timeout_ms = ulong;
                                eeprom.write();
                                st_config_timer.setDelay( ulong );
                                door1.setUnchangedDelay( ulong );
                                door2.setUnchangedDelay( ulong );
                                globalSendStatus();
                            }
                    }
            }
            break;

        case CHILD_MAIN+2:
            switch( cmd ){
                case C_SET:
                    switch( message.type ){
                        case V_CUSTOM:
                            ulong = strlen( payload ) ? atol( payload ) : 0;
                            if( ulong > 0 ){
                                eeprom.d.react_time_ms = ulong;
                                eeprom.write();
                                st_main_timer.setDelay( ulong );
                                st_main_timer.restart();
                                globalSendStatus();
                            }
                    }
            }
            break;

        case CHILD_MAIN+3:
            switch( cmd ){
                case C_SET:
                    switch( message.type ){
                        case V_CUSTOM:
                            ulong = strlen( payload ) ? atol( payload ) : 0;
                            if( ulong > 0 ){
                                eeprom.d.learning_oc_wait_ms = ulong;
                                eeprom.write();
                                st_learning_oc_timer.setDelay( ulong );
                                globalSendStatus();
                            }
                    }
            }
            break;

        case CHILD_MAIN+4:
            switch( cmd ){
                case C_SET:
                    switch( message.type ){
                        case V_CUSTOM:
                            ulong = strlen( payload ) ? atol( payload ) : 0;
                            if( ulong > 0 ){
                                eeprom.d.learning_bd_wait_ms = ulong;
                                eeprom.write();
                                st_learning_bd_timer.setDelay( ulong );
                                globalSendStatus();
                            }
                    }
            }
            break;

        case CHILD_MAIN+5:
            switch( cmd ){
                case C_SET:
                    switch( message.type ){
                        case V_CUSTOM:
                            ulong = strlen( payload ) ? atol( payload ) : 0;
                            if( ulong > 0 ){
                                eeprom.d.move_debounce_ms = ulong;
                                eeprom.write();
                                door1.setMoveDebounceDelay( ulong );
                                door2.setMoveDebounceDelay( ulong );
                                globalSendStatus();
                            }
                    }
            }
            break;

        case CHILD_MAIN+6:
            switch( cmd ){
                case C_SET:
                    switch( message.type ){
                        case V_CUSTOM:
                            ulong = strlen( payload ) > 2 ? atol( payload+2 ) : 0;
                            if( ulong > 0 ){
                                eeprom.d.btn_push_ms = ulong;
                                eeprom.write();
                                door1.setBtnPushDelay( ulong );
                                door2.setBtnPushDelay( ulong );
                                globalSendStatus();
                            }
                    }
            }
            break;

        case CHILD_MAIN+7:
            switch( cmd ){
                case C_SET:
                    switch( message.type ){
                        case V_CUSTOM:
                            uint = strlen( payload ) ? atoi( payload ) : 0;
                            if( uint > 0 ){
                                eeprom.d.min_published_pct = uint;
                                eeprom.write();
                                door1.setMinPublished( uint );
                                door2.setMinPublished( uint );
                                globalSendStatus();
                            }
                    }
            }
            break;

        case CHILD_ID_DOOR1+DOOR_MAIN:
            switch( cmd ){
                case C_REQ:
                    switch( message.type ){
                        case V_CUSTOM:
                            req = strlen( payload ) > 0 ? atoi( payload ) : 0;
                            switch( req ){
                                case 1:
                                    door1.pushButton();
                                    break;
                            }
                    }
            }
            break;

        case CHILD_ID_DOOR2+DOOR_MAIN:
            switch( cmd ){
                case C_REQ:
                    switch( message.type ){
                        case V_CUSTOM:
                            req = strlen( payload ) > 0 ? atoi( payload ) : 0;
                            switch( req ){
                                case 1:
                                    door2.pushButton();
                                    break;
                            }
                    }
            }
            break;
    }
}

/*
 * A callback to periodically re-send the global configuration
 * As the same globalSendStatus() function may be triggered by a received command,
 *  we restart here the config timer.
 */
void onUnchangedConfigCb( void *empty )
{
    globalSendStatus();
}

void globalSendStatus( void )
{
    msg.clear();
    send( msg.setSensor( CHILD_MAIN ).setType( V_VAR1 ).set( hub.average_ms ));
    msg.clear();
    send( msg.setSensor( CHILD_MAIN+1 ).setType( V_VAR1 ).set( eeprom.d.unchanged_timeout_ms ));
    msg.clear();
    send( msg.setSensor( CHILD_MAIN+2 ).setType( V_VAR1 ).set( eeprom.d.react_time_ms ));
    msg.clear();
    send( msg.setSensor( CHILD_MAIN+3 ).setType( V_VAR1 ).set( eeprom.d.learning_oc_wait_ms ));
    msg.clear();
    send( msg.setSensor( CHILD_MAIN+4 ).setType( V_VAR1 ).set( eeprom.d.learning_bd_wait_ms ));
    msg.clear();
    send( msg.setSensor( CHILD_MAIN+5 ).setType( V_VAR1 ).set( eeprom.d.move_debounce_ms ));
    msg.clear();
    send( msg.setSensor( CHILD_MAIN+6 ).setType( V_VAR1 ).set( eeprom.d.btn_push_ms ));
    msg.clear();
    send( msg.setSensor( CHILD_MAIN+7 ).setType( V_VAR1 ).set( eeprom.d.min_published_pct ));

    st_config_timer.restart();
}

/* Dumping the whole configuration from a received command.
 *  Also sent at startup time before having read any actual status.
 */
void dumpConfiguration( void )
{
    globalSendStatus();
    door1.sendMove();
    door1.sendStatus();
    door2.sendMove();
    door2.sendStatus();
}

