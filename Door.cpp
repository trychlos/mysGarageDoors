#include <Arduino.h>
#include <core/MySensorsCore.h>
#include "Door.h"
#include "hub.h"

// uncomment to debugging this file
#define DOOR_DEBUG

extern void doorGetElapsed( uint8_t, unsigned long *, unsigned long * );

/**
 * DoorDebounceCb:
 * 
 * Door debouncing callback.
 */
void DoorDebounceCb( void *pdoor )
{
    (( Door * ) pdoor )->debounce();
}

/**
 * DoorPushButtonCb:
 * 
 * Door push button callback.
 * 
 * This may let us set an impulsion to the relay.
 * The width of the impulsion is the timer delay.
 */
void DoorPushButtonCb( void *pdoor )
{
    (( Door * ) pdoor )->pushButton( LOW );
}

/**
 * DoorUnchangedCb:
 * 
 * Door unchanged timeout callback.
 */
void DoorUnchangedCb( void *pdoor )
{
    (( Door * ) pdoor )->sendStatus();
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
    this->id = child_id;

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
    pinMode( this->closed_pin, INPUT_PULLUP );
    this->is_closed = false;

    this->enabled_pin = enabled_pin;
    pinMode( this->enabled_pin, INPUT_PULLUP );
    this->is_enabled = false;

    this->move_id = DOOR_NOMOVE;
    this->move_begin_ms = 0;
    this->pos = 0;
    this->last_pos = 0;
    this->min_change = 0;
}

/**
 * Door:debounce:
 * 
 * Is called from the debouncing timer callback.
 * Debounce the move transition.
 * Change the move_id if the current move is the same than at the start
 *  of the timer.
 * 
 * Public.
 */
void Door::debounce( void )
{
    uint8_t move_at_end = this->readCurrentMove( false );
    if( move_at_end == this->debounced_move ){
        this->move_id = this->debounced_move;
    }
}

/**
 * Door::getId:
 * 
 * Returns: the door child identifier.
 * 
 * Public.
 */
uint8_t Door::getId( void )
{
    return( this->id );
}

/**
 * Door::getClosed:
 * 
 * Returns: %TRUE if the door is closed.
 * 
 * Public.
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
 * Public.
 */
bool Door::getEnabled( void )
{
    return( this->is_enabled );
}

/**
 * Door::getMoveId:
 * 
 * Returns: the current move identifier.
 * 
 * Public
 */
uint8_t Door::getMoveId( void )
{
    return( this->move_id );
}

/**
 * Door::getPosition:
 * 
 * Returns: the current position.
 * 
 * Public.
 */
uint8_t Door::getPosition( void )
{
    return( this->pos );
}

/**
 * Door::present:
 * 
 * MySensors presentation.
 * 
 * Public.
 */
void Door::present()
{
#ifdef DOOR_DEBUG
    Serial.print( F( "Door::present() id=" ));
    Serial.println( this->id );
#endif
    //                                             1234567890123456789012345
    ::present( this->id+DOOR_MAIN,     S_DOOR, F( "DoorOpenClose" ));
    ::present( this->id+DOOR_UPDOWN,   S_DOOR, F( "DoorMoving" ));
    ::present( this->id+DOOR_POSITION, S_DOOR, F( "DoorPosition" ));
    ::present( this->id+DOOR_CLOSED,   S_DOOR, F( "DoorClosed" ));
    ::present( this->id+DOOR_OPENMS,   S_DOOR, F( "DoorOpenDuration" ));
    ::present( this->id+DOOR_CLOSEMS,  S_DOOR, F( "DoorCloseDuration" ));
}

/**
 * Door::pushButton:
 * 
 * Action the open/close button.
 * 
 * Public.
 */
void Door::pushButton( byte level )
{
#ifdef DOOR_DEBUG
    Serial.print( F( "Door::pushButton() id=" ));
    Serial.print( this->id );
    Serial.print( F( ": level=" ));
    Serial.println( level == HIGH ? "High":"Low" );
#endif
    digitalWrite( this->command_pin, level );
    if( level == HIGH ){
        this->btn_push_timer.start();
    }
}

/**
 * Door::runLoop:
 * @hub: the core data structure.
 * 
 * This is called on each main loop, for each known door.
 * 
 * Debouncing: the end of the move may be marked by a bounce.
 * This is less than one second. So we just ignore same move
 * before the end of the debouncing delay.
 * 
 * Public
 */
void Door::runLoop( hub_t *hub )
{
    bool enabled_changed = this->readEnabled();
    bool closed_changed = this->readClosed();

    if( enabled_changed || closed_changed ){
        this->sendStatus();
    }

    if( this->getEnabled()){
        
        bool move_changed = false;
        uint8_t cur_move = this->readCurrentMove();
    
        // move has changed
        // - either from no move to moving up or down (this is the beginning)
        // - either from moving to nomove (this is the ending)
        if( cur_move != this->move_id ){
            if( cur_move != DOOR_NOMOVE ){
                this->move_begin_ms = millis();
            }
            this->move_id = cur_move;
            move_changed = true;
        }
    
        // if position has changed, send the new position if greater than the
        //  minimal change allowed to be published
        //  0% is fully closed, 100% is fully opened
        if(( move_changed || cur_move != DOOR_NOMOVE ) && hub->average_ms > 0 ){
            if( !this->is_closed ){
                unsigned long duration = millis() - this->move_begin_ms;
                this->pos = ( uint8_t )(( float )( duration * 100.0 / hub->average_ms ));
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
                Serial.print( hub->average_ms );
                Serial.print( F( ", pos=" ));
                Serial.println( this->pos );
#endif
            }
            uint8_t diff = abs( this->pos - this->last_pos );
            if( closed_changed || ( diff >= this->min_change && this->min_change > 0 )){
                move_changed = true;
                this->last_pos = this->pos;
            }
        }

        if( move_changed ){
            this->sendMove();
        }
    }
}

/** 
 * Door::sendMove:
 *  
 * Send "opening", "closing" and "position" messages when a move is detected.
 */
void Door::sendMove( void )
{
    MyMessage msg;

    if( this->getEnabled()){

        // send the move status
        uint8_t child_id = this->getId();
        uint8_t move_id = this->getMoveId();
        uint8_t pos = this->getPosition();

        switch( move_id ){
            case DOOR_NOMOVE:
                msg.clear();
                send( msg.setSensor( child_id+DOOR_UPDOWN ).setType( V_UP ).set( false ));
                msg.clear();
                send( msg.setSensor( child_id+DOOR_UPDOWN ).setType( V_DOWN ).set( false ));
                break;
            case DOOR_MOVEUP:
                msg.clear();
                send( msg.setSensor( child_id+DOOR_UPDOWN ).setType( V_UP ).set( true ));
                msg.clear();
                send( msg.setSensor( child_id+DOOR_UPDOWN ).setType( V_DOWN ).set( false ));
                break;
            case DOOR_MOVEDOWN:
                msg.clear();
                send( msg.setSensor( child_id+DOOR_UPDOWN ).setType( V_UP ).set( false ));
                msg.clear();
                send( msg.setSensor( child_id+DOOR_UPDOWN ).setType( V_DOWN ).set( true ));
                break;
        }

        // send the position
        msg.clear();
        send( msg.setSensor( child_id+DOOR_POSITION ).setType( V_VAR1 ).set( pos ));

#ifdef DOOR_DEBUG
        Serial.print( F( "Door::sendMove() id=" ));
        Serial.print( child_id );
        Serial.print( F( ", move=" ));
        Serial.print( Door::MoveToStr( move_id ));
        Serial.print( F( ", position=" ));
        Serial.println( pos );
#endif
    }
}

/** 
 * Door:sendStatus:
 * 
 * Called when dumping the configuration
 */
void Door::sendStatus( void )
{
    MyMessage msg;
    uint8_t child_id = this->getId();
    bool enabled = this->getEnabled();
    bool closed = this->getClosed();
    unsigned long openms, closems;
    doorGetElapsed( child_id, &openms, &closems );
    
    msg.clear();
    send( msg.setSensor( child_id+DOOR_MAIN ).setType( V_VAR1 ).set( enabled ));
    msg.clear();
    send( msg.setSensor( child_id+DOOR_CLOSED ).setType( V_VAR1 ).set( closed ));
    msg.clear();
    send( msg.setSensor( child_id+DOOR_OPENMS ).setType( V_VAR1 ).set( openms ));
    msg.clear();
    send( msg.setSensor( child_id+DOOR_CLOSEMS ).setType( V_VAR1 ).set( closems ));

#ifdef DOOR_DEBUG
        Serial.print( F( "Door::sendStatus() id=" ));
        Serial.print( child_id );
        Serial.print( F( ", enabled=" ));
        Serial.print( enabled ? "True":"False" );
        Serial.print( F( ", closed=" ));
        Serial.print( closed ? "True":"False" );
        Serial.print( F( ", opening_ms=" ));
        Serial.print( openms );
        Serial.print( F( ", closing_ms=" ));
        Serial.println( closems );
#endif
}

/**
 * Door::setBtnPushDelay:
 * @delay_ms: the duration of the button push (ms).
 * 
 * Set the @delay_ms.
 * 
 * Public
 */
void Door::setBtnPushDelay( unsigned long delay_ms )
{
    this->btn_push_timer.setup( "ButtonPush", delay_ms, true, DoorPushButtonCb, this );
}

/**
 * Door::setMinPublished:
 * @min_change: the minimal position changed to be published.
 * 
 * Set the @min_change.
 * 
 * Public
 */
void Door::setMinPublished( uint8_t min_change )
{
    this->min_change = min_change;
}

/**
 * Door::setMoveId:
 * @move_id: the move identifier.
 * 
 * Set the @move_id.
 * 
 * Public
 */
void Door::setMoveId( uint8_t move_id )
{
    this->move_id = move_id;
}

/**
 * Door::setMoveDebounceDelay:
 * @delay_ms: the debounce timeout of a move (ms).
 * 
 * Set the @delay_ms.
 * 
 * Public
 */
void Door::setMoveDebounceDelay( unsigned long delay_ms )
{
    this->debounce_timer.setup( "MoveDebounce", delay_ms, true, DoorDebounceCb, this );
}

/**
 * Door::setUnchangedDelay:
 * @delay_ms: the unchanged timeout (ms).
 * 
 * Set the @delay_ms.
 * 
 * Public
 */
void Door::setUnchangedDelay( unsigned long delay_ms )
{
    this->unchanged_timer.setup( "Unchanged", delay_ms, false, DoorUnchangedCb, this );
    this->unchanged_timer.restart();
}

/**
 * Door::setup:
 * @hub: the core object.
 * 
 * Setup the @Door MySensors object.
 * 
 * Public
 */
void Door::setup( hub_t *hub )
{
    this->setBtnPushDelay( hub->eeprom->d.btn_push_ms );
    this->setMinPublished( hub->eeprom->d.min_published_pct );
    this->setMoveDebounceDelay( hub->eeprom->d.move_debounce_ms );
    this->setUnchangedDelay( hub->eeprom->d.unchanged_timeout_ms );
}

/**
 * Door::MoveToStr:
 * @move_id: the move identifier.
 * 
 * Returns: a string which descrive the @move_id.
 * 
 * Static Public
 */
const char *Door::MoveToStr( uint8_t move_id )
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
 * Door::GetLearningDoor:
 * @hub: the core structure.
 * 
 * Returns: the Door object which is actually running in the learning program, or null.
 * 
 * Static Public
 */
Door *Door::GetLearningDoor( hub_t *hub )
{
    Door *learning_door = NULL;
    uint8_t door_id = hub->eeprom->d.learning_door;
    if( door_id == hub->door1->getId()){
        learning_door = hub->door1;
    } else if( door_id == hub->door2->getId()){
        learning_door = hub->door2;
    }
    return( learning_door );
}

/**
 * Door::readClosed:
 * 
 * Returns: %TRUE if the status has changed.
 * 
 * Force the position to zero (%) if the door is closed.
 * 
 * Private
 */
bool Door::readClosed( void )
{
    bool changed = false;
    bool was_closed = this->is_closed;
    byte value = digitalRead( this->closed_pin );
    this->is_closed = ( value == LOW );
    if( this->is_closed != was_closed ){
        changed = true;
        if( this->is_closed ){
            this->pos = 0;
        }
#ifdef DOOR_DEBUG
        Serial.print( F( "Door::readClosed() id=" ));
        Serial.print( this->id );
        Serial.print( F( ", closed=" ));
        Serial.println( this->is_closed ? "True":"False" );
#endif
    }
    return( changed );
}

/**
 * Door::readEnabled:
 * 
 * Returns: %TRUE if the status has changed.
 * 
 * Private
 */
bool Door::readEnabled( void )
{
    bool changed = false;
    bool was_enabled = this->is_enabled;
    byte value = digitalRead( this->enabled_pin );
    this->is_enabled = ( value == HIGH );
    if( this->is_enabled != was_enabled ){
        changed = true;
#ifdef DOOR_DEBUG
        Serial.print( F( "Door::readEnabled() id=" ));
        Serial.print( this->id );
        Serial.print( F( ", enabled=" ));
        Serial.println( this->is_enabled ? "True":"False" );
#endif
    }
    return( changed );
}

/**
 * Door::readCurrentMove:
 * @debounce: whether a potential move transition has to be debounced.
 *  Set to %false when called from the debouncing timer itself.
 * 
 * Returns: the type of the current move after debounce.
 * 
 * The debouncing timer is started on each move transition.
 * Ignore any new move while the debouncing timer is running.
 * 
 * Private.
 */
uint8_t Door::readCurrentMove( bool debounce )
{
    /* if the timer is started, we are currently debouncing
     *  the last move - just ignore others
     *  this->move_loop here is the move at the last loop before this one */
    if( this->debounce_timer.isStarted()){
        return( this->move_id );
    }

    /* else return the current move
     *  starting the debouncing timer if we are stopping */
    uint8_t current = DOOR_NOMOVE;
    byte value = digitalRead( this->opening_pin );
#ifdef DOOR_DEBUG
    /*
    Serial.print( F( "[Door::readCurrentMove] child_id=" ));
    Serial.print( this->child_id );
    Serial.print( F( ", opening_pin=" ));
    Serial.println( value ? "High":"Low" );
    */
#endif
    if( value == LOW ){
        current = DOOR_MOVEUP;
    } else {
        value = digitalRead( this->closing_pin );
#ifdef DOOR_DEBUG
        /*
        Serial.print( F( "[Door::readCurrentMove] child_id=" ));
        Serial.print( this->child_id );
        Serial.print( F( ", closing_pin=" ));
        Serial.println( value ? "High":"Low" );
        */
#endif
        if( value == LOW ){
            current = DOOR_MOVEDOWN;
        }
    }
    if( debounce && current != this->move_id ){
        this->debounced_move = current;
        this->debounce_timer.start();
        current = this->move_id;
    }

    return( current );
}

