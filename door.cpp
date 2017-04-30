#include "door.h"
#include "doorglobal.h"
#include "until_now.h"

// uncomment to debugging this file
#define DOOR_DEBUG

#define DOOR_TIMER_DEBOUNCE       1000                /* debouncing delay (ms) */
#define DOOR_TIMER_PUSH            500                /* the duration of the push button pulse (ms) */
#define DOOR_TIMER_TIMEOUT     3600000                /* sends a message each hour on no event */

static const char *st_debouncing  = "DoorDebouncingTimer";
static const char *st_pushbutton  = "DoorPushButtonTimer";
static const char *st_state       = "DoorStateMsgTimer";
static const char *st_position    = "DoorPositionMsgTimer";

/**
 * DoorDebounceCb:
 * 
 * Door debounce move callback.
 * 
 * The corresponding time is started when a move ends.
 * All changes during this delay are just ignored.
 * The callback does nothing,
 * but just allows other moves to be considered.
 */
void DoorDebounceCb( void *pdata )
{
}

/**
 * DoorPushButtonCb:
 * 
 * Door push button callback.
 * 
 * This may let us set an impulsion to the relay.
 * The width of the impulsion if the timer push.
 */
void DoorPushButtonCb( void *pdata )
{
    (( Door * ) pdata )->pushButton( LOW );
}

/**
 * DoorStateCb:
 * 
 * The timer is re-armed each time we are sending a message.
 * We resend the same message each time the timer expires.
 */
void DoorStateCb( void *pdata )
{
    (( Door * ) pdata )->sendState();
}

/**
 * DoorPositionCb:
 * 
 * The timer is re-armed each time we are sending a message.
 * We resend the same message each time the timer expires.
 */
void DoorPositionCb( void *pdata )
{
    (( Door * ) pdata )->sendPosition();
}

/**
 * Door::Door:
 * @child_id: the main child identifier;
 *  other identifiers are directly deduced from this one.
 * @command_pin: the push bouton pin number (output).
 * @opening_pin: the 'currently opening move' input pin number.
 * @closing_pin: the 'currently closing move' input pin number.
 * @closed_pin: the door closed dry contact pin number (input).
 * @enabled_pin: the pin is shortened to GND if the door is not enabled.
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

    this->move_loop = DOOR_NOMOVE;
    this->last_move = DOOR_NOMOVE;
    this->last_begin_ms = 0;
    this->last_end_ms = 0;
    this->pos = 0;
    
    this->debounce_timer.set(    st_debouncing, DOOR_TIMER_DEBOUNCE, true,  DoorDebounceCb,   this );
    this->push_button_timer.set( st_pushbutton, DOOR_TIMER_PUSH,     true,  DoorPushButtonCb, this );
    this->state_timer.set(       st_state,      DOOR_TIMER_TIMEOUT,  false, DoorStateCb,      this );
    this->position_timer.set(    st_position,   DOOR_TIMER_TIMEOUT,  false, DoorPositionCb,   this );
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
 * Ignore any new move if the debouncing timer has been started.
 * 
 * Public
 */
uint8_t Door::getCurrentMove( void )
{
    /* if the timer is started, we are currently debouncing
     *  the last move - just ignore others
     *  this->move_loop here is the move at the last loop before this one */
    if( this->debounce_timer.isStarted()){
        return( this->move_loop );
    }

    /* else return the current move
     *  starting the debouncing timer if we are stopping */
    uint8_t current = DOOR_NOMOVE;
    byte value = digitalRead( this->opening_pin );
    if( value == LOW ){
        current = DOOR_MOVEUP;
    } else {
        value = digitalRead( this->closing_pin );
        if( value == LOW ){
            current = DOOR_MOVEDOWN;
        }
    }
    if( current == DOOR_NOMOVE && current != this->move_loop ){
        this->debounce_timer.start();
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
            this->sendState();
        }
    
        // if position has changed, send the new position
        // 0% is fully closed, 100% is fully opened
        if( cur_move != DOOR_NOMOVE && average_ms > 0 ){
            unsigned long duration = untilNow( millis(), this->last_begin_ms );
            this->pos = duration * 100 / average_ms;
            if( this->pos > 100 ){
                this->pos = 100;
            }
            if( cur_move == DOOR_MOVEDOWN ){
                this->pos = 100 - this->pos;
            }
#ifdef DOOR_DEBUG
            Serial.print( F( "[Door::runLoop] duration=" ));
            Serial.print( duration );
            Serial.print( F( ", average=" ));
            Serial.print( average_ms );
            Serial.print( F( ", pos=" ));
            Serial.println( this->pos );
#endif
            this->sendPosition();
        }
        
#ifdef DOOR_DEBUG
    } else {
        Serial.print( F( "[Door::runLoop] child_id=" ));
        Serial.print( this->child_id );
        Serial.println( F( " is disabled" ));
#endif
    }
}

/**
 * Door::pushButton:
 * 
 * A C_SET command has been received on the ACTOR child.
 * Action the open/close button.
 * 
 * Public
 */
void Door::pushButton( byte level )
{
#ifdef DOOR_DEBUG
    Serial.print( F( "[Door::pushButton] child_id=" ));
    Serial.print( this->child_id );
    Serial.print( F( ": level=" ));
    Serial.println( level == HIGH ? "High":"Low" );
#endif
    digitalWrite( this->command_pin, level );
    if( level == HIGH ){
        this->push_button_timer.start();
    }
}

/**
 * Door::sendState:
 * 
 * Sends the Up/Down state message.
 * 
 * Public
 */
void Door::sendState( void )
{
#ifdef DOOR_DEBUG
    Serial.print( F( "[Door::sendState] child_id=" ));
    Serial.print( this->child_id );
    Serial.print( F( ": move=" ));
    Serial.print( this->move_loop );
    Serial.print( " (" );
    Serial.print( Door::StateToStr( this->move_loop ));
    Serial.println( ")" );
#endif
    doorSendState( this->child_id, this->move_loop );
    this->state_timer.restart();
}

/**
 * Door::sendPosition:
 * 
 * Sends the position message.
 * 
 * Public
 */
void Door::sendPosition( void )
{
#ifdef DOOR_DEBUG
    Serial.print( F( "[Door::sendPosition] child_id=" ));
    Serial.print( this->child_id );
    Serial.print( F( ": pos=" ));
    Serial.println( this->pos );
#endif
    doorSendPosition( this->child_id, this->pos );
    this->position_timer.restart();
}

/**
 * Door::StateToStr:
 * 
 * Returns: a string which descrive the move.
 * 
 * Static Public
 */
static const char * Door::StateToStr( uint8_t state )
{
    const char *cstr = "UnknwonState";
    switch( state ){
      case DOOR_NOMOVE:
        cstr = "DOOR_NOMOVE";
        break;
      case DOOR_MOVEUP:
        cstr = "DOOR_MOVEUP";
        break;
      case DOOR_MOVEDOWN:
        cstr = "DOOR_MOVEDOWN";
        break;
    }
    return( cstr );
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

