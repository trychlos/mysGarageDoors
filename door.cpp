#include "door.h"
#include "doorglobal.h"
#include "until_now.h"

// uncomment to debugging this file
#define DOOR_DEBUG

#define DOOR_DEBOUNCE       1000                /* debouncing delay (ms) */
#define DOOR_PUSH            250                /* the duration of the push button pulse (ms) */
#define DOOR_TIMEOUT       60000                /* sends a message each minute on no event */

/**
 * Door::Door:
 * @child_id: the main child identifier;
 *  other identifiers are directly deduced from this one.
 * @command_pin: the push bouton pin number (output).
 * @opening_pin: the 'currently opening move' input pin number.
 * @closing_pin: the 'currently closing move' input pin number.
 * @closed_pin: the door closed dry contact pin number (input).
 * 
 * Constructor.
 */
Door::Door( uint8_t child_id, uint8_t command_pin, uint8_t opening_pin, uint8_t closing_pin, uint8_t closed_pin, uint8_t enabled_pin )
{
    this->child_id = child_id;

    this->command_pin = command_pin;
    digitalWrite( this->command_pin, LOW );
    pinMode( this->command_pin, OUTPUT );

    this->opening_pin = opening_pin;
    digitalWrite( this->opening_pin, HIGH );
    pinMode( this->opening_pin, INPUT );

    this->closing_pin = closing_pin;
    digitalWrite( this->closing_pin, HIGH );
    pinMode( this->closing_pin, INPUT );

    this->closed_pin = closed_pin;
    digitalWrite( this->closed_pin, LOW );
    pinMode( this->closed_pin, INPUT );

    this->enabled_pin = enabled_pin;
    pinMode( this->enabled_pin, INPUT_PULLUP );
    this->is_enabled = false;

    this->last_msg = 0;
}

/**
 * Door::getChildId:
 * 
 * Returns: the main child identifier.
 * 
 * Public
 */
uint8_t Door::getChildId( void )
{
    return( this->child_id );
}

/**
 * Door::getCurrentMove:
 * 
 * Returns: the type of the current move after debounce.
 * 
 * Ignore this new move if this is the same that the last one, and the
 *  debouncing delay is not expired.
 * 
 * Public
 */
uint8_t Door::getCurrentMove( void )
{
    uint8_t current = DOOR_NOMOVE;
    byte value = digitalRead( this->opening_pin );
    if( value == HIGH ){
        current = DOOR_MOVEUP;
    } else {
        value = digitalRead( this->closing_pin );
        if( value == HIGH ){
            current = DOOR_MOVEDOWN;
        }
    }
    if( current != DOOR_NOMOVE ){
        bool ignore = false;
        if( current == this->last_move ){
            unsigned long delay = untilNow( millis(), this->last_end_ms );
            if( delay < DOOR_DEBOUNCE ){
                ignore = true;
            }
        }
        if( ignore ){
            current = DOOR_NOMOVE;
        }
    }
    return( current );
}

/**
 * Door::isEnabled:
 * 
 * Returns: %TRUE if the door is enabled.
 * 
 * Public
 */
bool Door::isEnabled( void )
{
    return( this->is_enabled );
}

/**
 * Door::requestInfo:
 * 
 * A C_REQ command has been received on the ACTOR child.
 * Send valuable informations to the controller.
 * 
 * Public
 */
void Door::requestInfo( void )
{
}

/**
 * Door::runLoop:
 * 
 * This is called on each main loop, for each known door.
 * 
 * Debouncing: the end of the move may be marked by a bounce.
 * This is less than one second. So we just ignore same move
 * before the end of the debouncing delay.
 * 
 * Public
 */
void Door::runLoop( unsigned long average_ms )
{
    if( this->getEnabled()){
    
        uint8_t prev_loop = this->move_loop;
        uint8_t cur_move = this->getCurrentMove();
    
        // move has changed
        // - either from no move to moving up or down (this is the beginning)
        // - either from moving to nomove (this is the ending)
        if( cur_move != prev_loop ){
            this->move_loop = cur_move;
            if( cur_move != DOOR_NOMOVE ){
                this->last_begin_ms = millis();
                this->last_end_ms = 0;
                this->last_move = move_loop;
            } else {
                this->last_end_ms = millis();
            }
            doorSendState( this->child_id, this->move_loop );
        }
    
        // if position has changed, send the new position
        // 0% is fully closed, 100% is fully opened
        if( cur_move != DOOR_NOMOVE && average_ms > 0 ){
            unsigned long duration = untilNow( millis(), this->last_begin_ms );
            doorSendPosition( this->child_id, ( uint8_t )( duration / average_ms * 100.0 ));
        }
        
#ifdef DOOR_DEBUG
    } else {
        Serial.print( F( "Door::runLoop] child_id=" ));
        Serial.print( this->child_id );
        Serial.println( F( " is disabled" ));
#endif
    }
}

void DoorPushButtonDown( void )
{
    MsTimer2::stop();
#ifdef DOOR_DEBUG
    Serial.print( F( "[Door:pushButton] child_id=" ));
    Serial.print( this->child_id );
    Serial.println( F( ": end of pulse" ));
#endif
}

/**
 * Door::pushButton:
 * 
 * A C_SET command has been received on the ACTOR child.
 * Action the open/close button.
 * 
 * Public
 */
void Door::pushButton( void )
{
    digitalWrite( this->command_pin, HIGH );
    MsTimer2::set( DOOR_PUSH, DoorPushButtonDown );
    MsTimer2::start();
#ifdef DOOR_DEBUG
    Serial.print( F( "[Door:pushButton] child_id=" ));
    Serial.print( this->child_id );
    Serial.println( F( ": start of pulse" ));
#endif
}

/**
 * Door::getEnabled:
 * 
 * Returns: %TRUE if the door is enabled.
 * 
 * Private
 */
bool Door::getEnabled( void )
{
    bool was_enabled = this->is_enabled;
    byte value = digitalRead( this->enabled_pin );
    this->is_enabled = ( value == HIGH );
#ifdef DOOR_DEBUG
    if( was_enabled != this->is_enabled ){
        Serial.print( F( "[Door::getEnabled] child_id=" ));
        Serial.print( this->child_id );
        Serial.print( F( ", enabled=" ));
        Serial.println( this->is_enabled ? "True":"False" );
    }
#endif
    return( this->is_enabled );
}

