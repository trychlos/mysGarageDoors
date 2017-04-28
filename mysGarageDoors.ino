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

MyMessage msgCustom( 0, V_CUSTOM );
MyMessage msgUp( 0, V_UP );
MyMessage msgDown( 0, V_DOWN );
MyMessage msgPosition( 0, V_PERCENTAGE );

/* **************************************************************************************
 * Door management
 * - up to two doors
 */
#ifdef HAVE_DOOR
#include "door.h"
#include "doorglobal.h"
#include "eeprom.h"
#include "until_now.h"

// Doors objects
Door door1( CHILD_ID_DOOR1,  5,  2,  3,  4,  6 );
Door door2( CHILD_ID_DOOR2, A2, A5, A4, A3, A1 );

// the data get from / set to the EEPROM
sEeprom eeprom;
unsigned long average_ms = 0;
uint8_t learning_phase = LEARNING_NONE;
unsigned long learning_ms = 0;

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
    present( child_id+DOOR_ACTOR, S_CUSTOM );
    present( child_id+DOOR_COVER, S_COVER );
#endif
}

/*
 * Send the current state to the controller
 */
void doorSendState( uint8_t child_id, uint8_t move_id )
{
    msgUp.setSensor( child_id+DOOR_COVER );
    msgDown.setSensor( child_id+DOOR_COVER );
    
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
#ifdef DEBUG_ENABLED
    Serial.print( F( "[doorSendState] child_id=" ));
    Serial.print( child_id );
    Serial.print( F( ", move_id=" ));
    Serial.println( move_id );
#endif
}

/*
 * Send the current position to the controller
 * 0% is fully closed, 100% is fully opened
 */
void doorSendPosition( uint8_t child_id, uint8_t pos )
{
    msgPosition.setSensor( child_id+DOOR_COVER );
    send( msgPosition.set( pos ));
#ifdef DEBUG_ENABLED
    Serial.print( F( "[doorSendPosition] child_id=" ));
    Serial.print( child_id );
    Serial.print( F( ", pos=" ));
    Serial.println( pos );
#endif
}
#endif // HAVE_DOOR

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
    average_ms = ( eeprom.door1_opening_ms + eeprom.door1_closing_ms + eeprom.door2_opening_ms + eeprom.door2_closing_ms ) / 4;
}

void loop()      
{
#ifdef DEBUG_ENABLED
    Serial.println( F( "[loop]" ));
#endif
#ifdef HAVE_DOOR
    door1.runLoop( average_ms );
    door2.runLoop( average_ms );

    unsigned long duration;
    uint8_t current_move;
    switch( learning_phase ){
        case LEARNING_START:
            if( door1.isEnabled()){
                door1.pushButton();
                learning_phase = LEARNING_OPEN1;
                learning_ms = millis();
            } else if( door2.isEnabled()){
                learning_phase = LEARNING_CLOSEWAIT1;
                learning_ms = millis();
            }
            break;
        case LEARNING_OPEN1:
            duration = untilNow( millis(), learning_ms );
            if( duration > LEARNING_WAIT ){
                current_move = door1.getCurrentMove();
                if( current_move == DOOR_NOMOVE ){
                    learning_phase = LEARNING_OPENWAIT1;
                    learning_ms = millis();
                    eeprom.door1_opening_ms = duration;
                }
            }
            break;
        case LEARNING_OPENWAIT1:
            duration = untilNow( millis(), learning_ms );
            if( duration > LEARNING_WAIT ){
                door1.pushButton();
                learning_phase = LEARNING_CLOSE1;
                learning_ms = millis();
            }
            break;
        case LEARNING_CLOSE1:
            duration = untilNow( millis(), learning_ms );
            if( duration > LEARNING_WAIT ){
                current_move = door1.getCurrentMove();
                if( current_move == DOOR_NOMOVE ){
                    learning_phase = LEARNING_CLOSEWAIT1;
                    learning_ms = millis();
                    eeprom.door1_closing_ms = duration;
                }
            }
            break;
        case LEARNING_CLOSEWAIT1:
            duration = untilNow( millis(), learning_ms );
            if( duration > LEARNING_WAIT ){
                door2.pushButton();
                learning_phase = LEARNING_OPEN2;
                learning_ms = millis();
            }
            break;
        case LEARNING_OPEN2:
            duration = untilNow( millis(), learning_ms );
            if( duration > LEARNING_WAIT ){
                current_move = door2.getCurrentMove();
                if( current_move == DOOR_NOMOVE ){
                    learning_phase = LEARNING_OPENWAIT2;
                    learning_ms = millis();
                    eeprom.door2_opening_ms = duration;
                }
            }
            break;
        case LEARNING_OPENWAIT2:
            duration = untilNow( millis(), learning_ms );
            if( duration > LEARNING_WAIT ){
                door2.pushButton();
                learning_phase = LEARNING_CLOSE2;
                learning_ms = millis();
            }
            break;
        case LEARNING_CLOSE2:
            duration = untilNow( millis(), learning_ms );
            if( duration > LEARNING_WAIT ){
                current_move = door2.getCurrentMove();
                if( current_move == DOOR_NOMOVE ){
                    learning_phase = LEARNING_CLOSEWAIT2;
                    learning_ms = millis();
                    eeprom.door2_closing_ms = duration;
                }
            }
            break;
        case LEARNING_CLOSEWAIT2:
            duration = untilNow( millis(), learning_ms );
            if( duration > LEARNING_WAIT ){
                learning_phase = LEARNING_NONE;
                eeprom_write( eeprom );
                average_ms = ( eeprom.door1_opening_ms + eeprom.door1_closing_ms + eeprom.door2_opening_ms + eeprom.door2_closing_ms ) / 4;
            }
            break;
    }
#endif
    wait( 200 );
}

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
    uint8_t cmd = message.getCommand();
    uint8_t req;
    
    if( message.sensor == CHILD_MAIN ){
        if( cmd == C_SET ){
            req = getPayloadInt( message );
            switch( req ){
                case 1:
                    eeprom_raz();
                    break;
                case 2:
                    runLearningProgram();
                    break;
            }
        }
      
    } else if( message.sensor == CHILD_ID_DOOR1+DOOR_ACTOR || message.sensor == CHILD_ID_DOOR2+DOOR_ACTOR ){
        if( cmd == C_SET ){
            req = getPayloadInt( message );
            if( req == 1 ){
                if( message.sensor == CHILD_ID_DOOR1+DOOR_ACTOR ){
                    door1.pushButton();
                } else {
                    door2.pushButton();
                }
            }
        }
        if( cmd == C_REQ ){
            req = getPayloadInt( message );
            if( req == 1 ){
                if( message.sensor == CHILD_ID_DOOR1+DOOR_ACTOR ){
                    door1.requestInfo();
                } else {
                    door2.requestInfo();
                }
            }
        }
    }
}

uint8_t getPayloadInt( const MyMessage &message )
{
    char buffer[2*MAX_PAYLOAD+1];
    memset( buffer, '\0', sizeof( buffer ));
    message.getString( buffer );
    if( strlen( buffer )){
        return( atoi( buffer ));
    }
    return( 0 );
}

void runLearningProgram( void )
{
    learning_phase = LEARNING_START;
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

