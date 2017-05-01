/*
 * MysGarageDoors
 * Copyright (C) 2017 Pierre Wieser <pwieser@trychlos.org>
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
 *   > the opening/closing moves (when the motors is active)
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
 * - for each of the two doors, the learning program will fully open and the fully
 *   close the door.
 * This sequence is used to estimate the duration of a full move (opening/closing).
 * The four measures are recorded in the EEPROM as four unsigned longs.
 * The average is used to estimate the position of the door.
 * 
 * pwi 2017- 4-26 creation
 */

// uncomment for debugging this sketch
#define DEBUG_ENABLED

// uncomment for debugging the EEPROM code
#define EEPROM_DEBUG

// uncomment for having Door class
#define HAVE_DOOR

// uncomment for having mySensors radio enabled
#define HAVE_NRF24_RADIO

#include "eeprom.h"
#include "pwi_timer.h"

static const char * const thisSketchName    = "mysGarageDoors";
static const char * const thisSketchVersion = "1.0-2017";

#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_RF24_CHANNEL 103
#define MY_SIGNING_SOFT
#define MY_SIGNING_SOFT_RANDOMSEED_PIN 7
#define MY_SOFT_HMAC_KEY 0xe5,0xc5,0x36,0xd8,0x4b,0x45,0x49,0x25,0xaa,0x54,0x3b,0xcc,0xf4,0xcb,0xbb,0xb7,0x77,0xa4,0x80,0xae,0x83,0x75,0x40,0xc2,0xb1,0xcb,0x72,0x50,0xaa,0x8a,0xf1,0x6d
// do not check the uplink if we choose to disable the radio
#ifndef HAVE_NRF24_RADIO
#define MY_TRANSPORT_WAIT_READY_MS 1000
#define MY_TRANSPORT_UPLINK_CHECK_DISABLED
#endif // HAVE_NRF24_RADIO
#include <MySensors.h>

enum {
    CHILD_MAIN = 1,
    CHILD_ID_DOOR1 = 10,
    CHILD_ID_DOOR2 = 20,
};

sEeprom eeprom;
unsigned long average_ms = 0;

/* main loop is 100 ms. */
#define MAIN_LOOP             250
static const char   *st_main_label        = "MainTimer";
pwiTimer main_timer;

/* unchanged default timeout is 1h */
#define UNCHANGED_TIMEOUT 3600000
static const char   *st_unchanged_label   = "UnchangedTimer";
static unsigned long st_unchanged_timeout = UNCHANGED_TIMEOUT;
pwiTimer unchanged_timer;

pwiTimer learning_oc_timer;           /* wait before opening and closing a door */
pwiTimer learning_bd_timer;           /* wait before closing door 1 and opening door 2 */

static const char *st_learning_oc = "LearningOpenCloseTimer";
static const char *st_learning_bd = "LearningBetweenDoorsTimer";

MyMessage msgVar1( 0, V_VAR1 );
MyMessage msgVar2( 0, V_VAR2 );
MyMessage msgVar3( 0, V_VAR3 );

/* **************************************************************************************
 * MySensors gateway
 */
void presentation_my_sensors()  
{
#ifdef DEBUG_ENABLED
    Serial.println( F( "[presentation_my_sensors]" ));
#endif
#ifdef HAVE_NRF24_RADIO
    sendSketchInfo( thisSketchName, thisSketchVersion );
    present( CHILD_MAIN, S_CUSTOM );
#endif // HAVE_NRF24_RADIO
}

/* **************************************************************************************
 * Door management
 * - up to two doors
 */
#ifdef HAVE_DOOR
#include "door.h"
#include "doorglobal.h"
#include "learning.h"
#include "until_now.h"

// Doors objects
Door door1( CHILD_ID_DOOR1,  5,  2,  3,  4,  6 );
Door door2( CHILD_ID_DOOR2, A2, A5, A4, A3, A1 );

// learning management
sLearning slearning;
void learning_set_door2( sLearning *slearn );

MyMessage msgUp( 0, V_UP );
MyMessage msgDown( 0, V_DOWN );

/* defaults to only sent position change greater than the minimal */
#define POSITION_MIN                       10
static uint8_t st_position_min = POSITION_MIN;

/*
 * Presents the doors to the mySensors gateway
 */
void presentation_door( void *pdoor )
{
    Door *door = ( Door * ) pdoor;
    uint8_t child_id = door->getChildId();
#ifdef DEBUG_ENABLED
    Serial.print( F( "[presentation_door] child_id=" )); Serial.println( child_id );
#endif
#ifdef HAVE_NRF24_RADIO
    present( child_id+DOOR_MAIN, S_CUSTOM );
    present( child_id+DOOR_UP, S_CUSTOM );
    present( child_id+DOOR_DOWN, S_CUSTOM );
    present( child_id+DOOR_POSITION, S_CUSTOM );
    present( child_id+DOOR_CLOSED, S_CUSTOM );
#endif
    door->setMinPositionChange( st_position_min );
}

void doorSendStatus( Door *door )
{
    uint8_t child_id = door->getChildId();

    // send the enabled status
    bool is_enabled = door->getEnabled();
    msgVar1.setSensor( child_id+DOOR_MAIN ).set( is_enabled );
    send( msgVar1 );

    if( is_enabled ){
        // send the move status
        uint8_t move_id = door->getMove();
        msgUp.setSensor( child_id+DOOR_UP );
        msgDown.setSensor( child_id+DOOR_DOWN );
        
        switch( move_id ){
            case DOOR_NOMOVE:
                send( msgUp.set( false ));
                send( msgDown.set( false ));
                break;
            case DOOR_MOVEUP:
                send( msgUp.set( true ));
                send( msgDown.set( false ));
                break;
            case DOOR_MOVEDOWN:
                send( msgUp.set( false ));
                send( msgDown.set( true ));
                break;
        }

        // send the position
        msgVar1.setSensor( child_id+DOOR_POSITION ).set( door->getPosition());
        send( msgVar1 );

        // send the closed state
        msgVar1.setSensor( child_id+DOOR_CLOSED ).set( door->getClosed());
        send( msgVar1 );
    }

    unchanged_timer.restart();
}
#endif // HAVE_DOOR

void globalSendStatus( void )
{
    msgVar1.setSensor( CHILD_MAIN ).set( average_ms );
    send( msgVar1 );
    
    msgVar2.setSensor( CHILD_MAIN ).set( st_unchanged_timeout );
    send( msgVar2 );
    
    msgVar3.setSensor( CHILD_MAIN ).set( st_position_min );
    send( msgVar3 );

    unchanged_timer.restart();
}

void onUnchangedCb( void *empty )
{
    globalSendStatus();
#ifdef HAVE_DOOR
    doorSendStatus( &door1 );
    doorSendStatus( &door2 );
#endif
}

/* **************************************************************************************
 *  MAIN CODE
 */
void presentation()
{
#ifdef DEBUG_ENABLED
    Serial.println( F( "[presentation]" ));
#endif
    presentation_my_sensors();
#ifdef HAVE_DOOR
    presentation_door(( void * ) &door1 );
    presentation_door(( void * ) &door2 );
#endif
}

void setup()  
{
#ifdef DEBUG_ENABLED
    Serial.begin( 115200 );
    Serial.println( F( "[setup]" ));
#endif

    eeprom_read( eeprom );
    average_ms = eeprom_compute_average( eeprom );
    main_timer.set( st_main_label, MAIN_LOOP, false, main_loop, NULL );
    main_timer.setDebug( false );
    unchanged_timer.set( st_unchanged_label, st_unchanged_timeout, false, onUnchangedCb, NULL );

#ifdef HAVE_DOOR
    memset( &slearning, '\0', sizeof( sLearning ));
    slearning.phase = LEARNING_NONE;
    learning_oc_timer.set( st_learning_oc, LEARNING_OPENCLOSE_WAIT, true, onOpenCloseCb, &slearning );
    learning_bd_timer.set( st_learning_bd, LEARNING_BETWEENDOORS_WAIT, true, onBetweenDoorsCb, &slearning );
#endif

    pwiTimer::Dump();
    main_timer.start();
    unchanged_timer.start();
}

void loop()
{
    pwiTimer::Loop();
}

void main_loop( void *empty )
{
#ifdef DEBUG_ENABLED
    if( slearning.phase != LEARNING_NONE ){
        Serial.println( F( "[loop]" ));
        learning_dump( slearning );
    }
#endif
#ifdef HAVE_DOOR
    door1.runLoop( average_ms );
    door2.runLoop( average_ms );
    
    switch( slearning.phase ){

        // the door is ready to be opened
        case LEARNING_OPEN:
            slearning.door->pushButton();
            slearning.phase = LEARNING_WAITOPEN;
            slearning.ms = millis();
            break;

        // waiting for the opening move takes place
        case LEARNING_WAITOPEN:
            if( slearning.door->getMove() == DOOR_MOVEUP ){
                slearning.phase = LEARNING_OPENING;
            }
            break;
      
        // the door is opening (the button has been pushed)
        //  waiting for the end of the move
        case LEARNING_OPENING:
            if( slearning.door->getMove() == DOOR_NOMOVE ){
                *( slearning.opening_ms ) = untilNow( millis(), slearning.ms );
                learning_oc_timer.start();
            }
            break;

        // the door is ready to be closed
        case LEARNING_CLOSE:
            slearning.door->pushButton();
            slearning.phase = LEARNING_WAITCLOSE;
            slearning.ms = millis();
            break;

        // waiting for the opening move takes place
        case LEARNING_WAITCLOSE:
            if( slearning.door->getMove() == DOOR_MOVEDOWN ){
                slearning.phase = LEARNING_CLOSING;
            }
            break;

        // the door is closing (the button has been pushed)
        //  waiting for the end of the move
        case LEARNING_CLOSING:
            if( slearning.door->getMove() == DOOR_NOMOVE ){
                *( slearning.closing_ms ) = untilNow( millis(), slearning.ms );
                if( slearning.next ){
                    learning_bd_timer.start();
                } else {
                    memset( &slearning, '\0', sizeof( sLearning ));
                    slearning.phase = LEARNING_NONE;
                    eeprom_write( eeprom );
                    average_ms = eeprom_compute_average( eeprom );
                }
            }
            break;
    }
#endif
}

/*
 * This is the timer callback between opening and closing a door
 */
void onOpenCloseCb( void *user_data )
{
#ifdef HAVE_DOOR
    (( sLearning * ) user_data )->phase = LEARNING_CLOSE;
#endif
}

/*
 * This is the timer callback between closing door 1 and opening door 2
 */
void onBetweenDoorsCb( void *user_data )
{
#ifdef HAVE_DOOR
    learning_set_door2(( sLearning * ) user_data );
#endif
}

#ifdef HAVE_DOOR
void learning_set_door2( sLearning *slearn )
{
    slearn->door = &door2;
    slearn->next = NULL;
    slearn->phase = LEARNING_OPEN;
    slearn->opening_ms = &eeprom.door2_opening_ms;
    slearn->closing_ms = &eeprom.door2_closing_ms;
}
#endif

/*
 * Handle a C_SET/C_REQ request on a CHILD_ID:
 * see README.
 * 
 * C_SET on CHILD_MAIN:
 * - '1': raz the eeprom
 * - '2': run the learning program
 * 
 * C_SET on a CHILD_ACTOR:
 * - '1': open/close the door (push the button)
 * 
 * C_REQ on a CHILD_ACTOR:
 * - '1': send a custom message
 */
void receive(const MyMessage &message)
{
    char payload[2*MAX_PAYLOAD+1];
    uint8_t cmd = message.getCommand();
    uint8_t req;

    memset( payload, '\0', sizeof( payload ));
    message.getString( payload );
#ifdef HAVE_DEBUG
    Serial.print( F( "[receive] payload='" )); Serial.print( payload ); Serial.println( "'" ); 
#endif

    if( message.sensor == CHILD_MAIN ){
        switch( cmd ){
            case C_SET:
                req = strlen( payload ) > 0 ? atoi( payload ) : 0;
                switch( req ){
                    case 1:
                        eeprom_raz();
                        break;
                    case 2:
                        runLearningProgram();
                        break;
                    case 3:
                        st_unchanged_timeout = strlen( payload ) > 2 ? atol( payload+2 ) : 0;
                        unchanged_timer.set( st_unchanged_timeout );
                        break;
                    case 4:
                        st_position_min = strlen( payload ) > 2 ? atoi( payload+2 ) : 0;
#ifdef HAVE_DOOR
                        door1.setMinPositionChange( st_position_min );
                        door2.setMinPositionChange( st_position_min );
#endif
                        break;
                }
                break;
            case C_REQ:
                req = strlen( payload ) > 0 ? atoi( payload ) : 0;
                switch( req ){
                    case 1:
                        onUnchangedCb( NULL );
                        break;
                }
                break;
        }

#ifdef HAVE_DOOR
    } else if( message.sensor == CHILD_ID_DOOR1+DOOR_MAIN || message.sensor == CHILD_ID_DOOR2+DOOR_MAIN ){
        switch( cmd ){
            case C_SET:
                req = strlen( payload ) > 0 ? atoi( payload ) : 0;
                switch( req ){
                    case 1:
                        if( message.sensor == CHILD_ID_DOOR1+DOOR_MAIN ){
                            door1.pushButton();
                        } else {
                            door2.pushButton();
                        }
                        break;
                }
                break;
        }
#endif
    }
}

void runLearningProgram( void )
{
#ifdef HAVE_DOOR
    if( door1.getEnabled()){
        slearning.door = &door1;
        slearning.next = door2.getEnabled() ? &door2 : NULL;
        slearning.phase = LEARNING_OPEN;
        slearning.opening_ms = &eeprom.door1_opening_ms;
        slearning.closing_ms = &eeprom.door1_closing_ms;

    } else if( door2.getEnabled()){
        learning_set_door2( &slearning );
    }
#endif
}

/**
 * eeprom_compute_average:
 * @sdata: the sEeprom data structure to be dumped.
 *
 * Dump the sEeprom struct content.
 */
unsigned long eeprom_compute_average( sEeprom &sdata )
{
    uint8_t count = 0;
    unsigned long average = 0;
    if( sdata.door1_opening_ms > 0 ){
        average += sdata.door1_opening_ms;
        count += 1;
    }
    if( sdata.door1_closing_ms > 0 ){
        average += sdata.door1_closing_ms;
        count += 1;
    }
    if( sdata.door2_opening_ms > 0 ){
        average += sdata.door2_opening_ms;
        count += 1;
    }
    if( sdata.door2_closing_ms > 0 ){
        average += sdata.door2_closing_ms;
        count += 1;
    }
    if( count > 0 ){
        average /= count;
    }
#ifdef EEPROM_DEBUG
    Serial.print( F( "[eeprom_compute_average] average=" )); Serial.println( average );
#endif
    return( average );
}

/**
 * eeprom_dump:
 * @sdata: the sEeprom data structure to be dumped.
 *
 * Dump the sEeprom struct content.
 */
void eeprom_dump( sEeprom &sdata )
{
#ifdef EEPROM_DEBUG
    Serial.print( F( "[eeprom_dump] door n° 1 opening duration=" )); Serial.println( sdata.door1_opening_ms );
    Serial.print( F( "[eeprom_dump] door n° 1 closing duration=" )); Serial.println( sdata.door1_closing_ms );
    Serial.print( F( "[eeprom_dump] door n° 2 opening duration=" )); Serial.println( sdata.door2_opening_ms );
    Serial.print( F( "[eeprom_dump] door n° 2 closing duration=" )); Serial.println( sdata.door2_closing_ms );
#endif
}

/**
 * eeprom_read:
 * @sdata: the sEeprom data structure to be filled.
 *
 * Read the data from the EEPROM.
 */
void eeprom_read( sEeprom &sdata )
{
#ifdef EEPROM_DEBUG
    Serial.println( F( "[eeprom_read]" ));
#endif
    memset( &sdata, '\0', sizeof( sdata ));
    uint16_t i;
    for( i=0 ; i<sizeof( sdata ); ++i ){
        (( uint8_t * ) &sdata )[i] = loadState(( uint8_t ) i );
    }
    eeprom_dump( sdata );
}

/**
 * eeprom_write:
 * @sdata: the sEeprom data structure to be written.
 *
 * Write the data to the EEPROM.
 */
void eeprom_write( sEeprom &sdata )
{
#ifdef EEPROM_DEBUG
    Serial.println( F( "[eeprom_write]" ));
#endif
    uint16_t i;
    for( i=0 ; i<sizeof( sdata ) ; ++i ){
        saveState( i, (( uint8_t * ) &sdata )[i] );
    }
    eeprom_dump( sdata );
}

/**
 * eeprom_raz:
 *
 * RAZ the user data of the EEPROM.
 */
void eeprom_raz( void )
{
#ifdef EEPROM_DEBUG
    Serial.print( F( "[eeprom_raz] begin=" )); Serial.println( millis());
#endif
    uint16_t i;
    for( i=0 ; i<256 ; ++i ){
        saveState(( uint8_t ) i, 0 );
    }
#ifdef EEPROM_DEBUG
    Serial.print( F( "[eeprom_raz] end=" )); Serial.println( millis());
#endif
}

