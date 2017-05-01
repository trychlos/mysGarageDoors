#include "door.h"
#include "doorglobal.h"
#include "until_now.h"

// uncomment to debugging this file
#define DOOR_DEBUG

#define DOOR_TIMER_DEBOUNCE       1000                /* debouncing delay (ms) */
static const char *st_debouncing  = "DoorDebouncingTimer";

#define DOOR_TIMER_PUSH            500                /* the duration of the push button pulse (ms) */
static const char *st_pushbutton  = "DoorPushButtonTimer";

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
    this->is_closed = false;

    this->enabled_pin = enabled_pin;
    pinMode( this->enabled_pin, INPUT_PULLUP );
    this->is_enabled = false;

    this->move_loop = DOOR_NOMOVE;
    this->last_move = DOOR_NOMOVE;
    this->last_begin_ms = 0;
    this->last_end_ms = 0;
    this->pos = 0;
    this->last_pos = 0;
    this->min_change = 0;
    
    this->debounce_timer.set( st_debouncing, DOOR_TIMER_DEBOUNCE, true, DoorDebounceCb, this );
    this->push_button_timer.set( st_pushbutton, DOOR_TIMER_PUSH, true, DoorPushButtonCb, this );
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
 * Door::getClosed:
 * 
 * Returns: %TRUE if the door is closed.
 * 
 * Public
 */
bool Door::getClosed( void )
{
    return( this->is_closed );
}

/**
 * Door::getEnabled:
 * 
 * Returns: %TRUE if the door is enabled.
 * 
 * Public
 */
bool Door::getEnabled( void )
{
    return( this->is_enabled );
}

/**
 * Door::getMove:
 * 
 * Returns: the current move identifier.
 * 
 * Public
 */
uint8_t Door::getMove( void )
{
    return( this->move_loop );
}

/**
 * Door::getPosition:
 * 
 * Returns: the current position.
 * 
 * Public
 */
uint8_t Door::getPosition( void )
{
    return( this->pos );
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
    if( this->readEnabled()){

        bool send_status = false;
        uint8_t prev_loop = this->move_loop;
        uint8_t cur_move = this->readCurrentMove();
    
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
            send_status = true;
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
            uint8_t diff = abs( this->pos - this->last_pos );
            if( diff >= this->min_change && this->min_change > 0 ){
                send_status = true;
                this->last_pos = this->pos;
            }
        }

        // closed status has it changed ?
        bool prev_closed = this->is_closed;
        if( this->readClosed() != prev_closed ){
            send_status = true;
        }
        
        if( send_status ){
            doorSendStatus( this );
        }
        
#ifdef DOOR_DEBUG
  /*
    } else {
        Serial.print( F( "[Door::runLoop] child_id=" ));
        Serial.print( this->child_id );
        Serial.println( F( " is disabled" ));
        */
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
 * Door::setMinPositionChange:
 * @min_change: the minimal position changed to be published.
 * 
 * Set the @min_change.
 * 
 * Public
 */
void Door::setMinPositionChange( uint8_t min_change )
{
    this->min_change = min_change;
}

/**
 * Door::MoveToStr:
 * @move_id: the move identifier.
 * 
 * Returns: a string which descrive the @move_id.
 * 
 * Static Public
 */
static const char * Door::MoveToStr( uint8_t move_id )
{
    const char *cstr = "UnknownMoveId";
    switch( move_id ){
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
 * Door::readClosed:
 * 
 * Returns: %TRUE if the door is closed.
 * 
 * Private
 */
bool Door::readClosed( void )
{
    bool was_closed = this->is_closed;
    byte value = digitalRead( this->closed_pin );
    this->is_closed = ( value == LOW );
#ifdef DOOR_DEBUG
    if( was_closed != this->is_closed ){
        Serial.print( F( "[Door::readClosed] child_id=" ));
        Serial.print( this->child_id );
        Serial.print( F( ", closed=" ));
        Serial.println( this->is_closed ? "True":"False" );
    }
#endif
    return( this->is_closed );
}

/**
 * Door::readEnabled:
 * 
 * Returns: %TRUE if the door is enabled.
 * 
 * Private
 */
bool Door::readEnabled( void )
{
    bool was_enabled = this->is_enabled;
    byte value = digitalRead( this->enabled_pin );
    this->is_enabled = ( value == HIGH );
#ifdef DOOR_DEBUG
    if( was_enabled != this->is_enabled ){
        Serial.print( F( "[Door::readEnabled] child_id=" ));
        Serial.print( this->child_id );
        Serial.print( F( ", enabled=" ));
        Serial.println( this->is_enabled ? "True":"False" );
    }
#endif
    return( this->is_enabled );
}

/**
 * Door::readCurrentMove:
 * 
 * Returns: the type of the current move after debounce.
 * 
 * Ignore any new move if the debouncing timer has been started.
 * 
 * Private
 */
uint8_t Door::readCurrentMove( void )
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

