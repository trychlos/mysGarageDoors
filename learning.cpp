#include <pwiCommon.h>
#include "learning.h"
#include "hub.h"

// uncomment to debugging this file
#define LEARNING_DEBUG

static char const learningUnknown[] PROGMEM             = "Unknown learning mode";
static char const learningNone[] PROGMEM                = "LEARNING_NONE";
static char const learningOpenHaveTo[] PROGMEM          = "LEARNING_OPENHAVETO";
static char const learningOpenWaitForMove[] PROGMEM     = "LEARNING_OPENWAITFORMOVE";
static char const learningOpening[] PROGMEM             = "LEARNING_OPENING";
static char const learningOpenWaitForTimeout[] PROGMEM  = "LEARNING_OPENWAITFORTIMEOUT";
static char const learningCloseHaveTo[] PROGMEM         = "LEARNING_CLOSEHAVETO";
static char const learningCloseWaitForMove[] PROGMEM    = "LEARNING_CLOSEWAITFORMOVE";
static char const learningClosing[] PROGMEM             = "LEARNING_CLOSING";
static char const learningCloseWaitForTimeout[] PROGMEM = "LEARNING_CLOSEWAITFORTIMEOUT";

/**
 * learning_dump:
 * @hub: the core structure.
 * 
 * Dump the learning datas.
 * 
 * Static.
 */
void learning_dump( hub_t *hub )
{
#ifdef LEARNING_DEBUG
    Serial.print( F( "learning_dump() phase=" ));
    Serial.print( hub->eeprom->d.learning_phase );
    Serial.print( F( " (" ));
    Serial.print( learning_toStr( hub->eeprom->d.learning_phase ));
    Serial.print( F( "), door=" ));
    Serial.print( hub->eeprom->d.learning_door );
    Serial.print( F( ", start_ms=" ));
    Serial.print( hub->learning->ms );
    Serial.print( F( ", now=" ));
    Serial.println( millis());
#endif
}

/**
 * learning_getNext:
 * @hub: the core structure.
 * 
 * Returns: the other door, or 0 if we are at the end.
 */
uint8_t learning_getNext( hub_t *hub, uint8_t cur_id )
{
    uint8_t next_door = 0;

    if( cur_id == 0 ){
        if( hub->door1->getEnabled()){
            next_door = hub->door1->getId();
        } else if( hub->door2->getEnabled()){
            next_door = hub->door2->getId();
        }
    } else if( cur_id == hub->door1->getId()){
        if( hub->door2->getEnabled()){
            next_door = hub->door2->getId();
        }
    }

    return( next_door );
}

/**
 * learning_onBetweenDoorsCb:
 * @hub: the core structure.
 * 
 * This is the timer callback between closing door 1 and opening door 2
 * It is triggered even if there is no more door to open/close.
 */
void learning_onBetweenDoorsCb( hub_t *hub )
{
    memset( hub->learning, '\0', sizeof( learning_t ));

    hub->eeprom->d.learning_door = learning_getNext( hub, hub->eeprom->d.learning_door );
    hub->eeprom->d.learning_phase = hub->eeprom->d.learning_door > 0 ? LEARNING_OPENHAVETO : LEARNING_NONE;
    hub->eeprom->write();

    if( hub->eeprom->d.learning_door == 0 ){
        hub->average_ms = hub->eeprom->computeAverage();
    }
}

/**
 * learning_onOpenCloseCb:
 * @hub: the core structure.
 * 
 * This is the timer callback between opening and closing a door.
 * We take advantage of it to position the next move phase.
 */
void learning_onOpenCloseCb( hub_t *hub )
{
    hub->eeprom->d.learning_phase = LEARNING_CLOSEHAVETO;
    /*
    msg.clear();
    send( msg.setSensor( st_eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( st_eeprom.learning_phase ));
    */
}

/**
 * learning_runProgram:
 * @hub: the core structure.
 * @phase: the currently running learning phase.
 * 
 * Run the learning program as a state machine.
 * 
 * Static.
 */
void learning_runProgram( hub_t *hub, uint8_t phase )
{
    Door *learning_door = Door::GetLearningDoor( hub );
    if( learning_door ){
        switch( phase ){

            // the door is ready to be opened
            case LEARNING_OPENHAVETO:
                learning_setFromEeprom( hub );
                hub->learning->ms = millis();
                hub->eeprom->d.learning_phase = LEARNING_OPENWAITFORMOVE;
                learning_door->pushButton();
                /*
                msg.clear();
                send( msg.setSensor( st_eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( st_eeprom.learning_phase ));
                */
                break;

            // waiting for the opening move takes place
            case LEARNING_OPENWAITFORMOVE:
                if( learning_door->getMoveId() == DOOR_MOVEUP ){
                    hub->eeprom->d.learning_phase = LEARNING_OPENING;
                    /*
                    msg.clear();
                    send( msg.setSensor( st_eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( st_eeprom.learning_phase ));
                    */
                }
                break;

            // the door is opening (the button has been pushed)
            //  waiting for the end of the move
            case LEARNING_OPENING:
                if( learning_door->getMoveId() == DOOR_NOMOVE && !hub->oc_timer->isStarted()){
                    *( hub->learning->opening_ms ) = millis() - hub->learning->ms;
                    hub->eeprom->write();
                    learning_door->sendStatus();
                    /*
                    msg.clear();
                    send( msg.setSensor( st_eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( st_eeprom.learning_phase ));
                    */
                    hub->oc_timer->start();
                    hub->eeprom->d.learning_phase = LEARNING_OPENWAITFORTIMEOUT;
                    /*
                    msg.clear();
                    send( msg.setSensor( st_eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( st_eeprom.learning_phase ));
                    */
                }
                break;

            // the door is ready to be closed
            case LEARNING_CLOSEHAVETO:
                learning_setFromEeprom( hub );
                hub->learning->ms = millis();
                hub->eeprom->d.learning_phase = LEARNING_CLOSEWAITFORMOVE;
                learning_door->pushButton();
                /*
                msg.clear();
                send( msg.setSensor( st_eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( st_eeprom.learning_phase ));
                */
                break;

            // waiting for the closing move takes place
            case LEARNING_CLOSEWAITFORMOVE:
                if( learning_door->getMoveId() == DOOR_MOVEDOWN ){
                    hub->eeprom->d.learning_phase = LEARNING_CLOSING;
                    /*
                    msg.clear();
                    send( msg.setSensor( st_eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( st_eeprom.learning_phase ));
                    */
                }
                break;

            // the door is closing (the button has been pushed)
            //  waiting for the end of the move
            case LEARNING_CLOSING:
                if( learning_door->getMoveId() == DOOR_NOMOVE && !hub->bd_timer->isStarted()){
                    *( hub->learning->closing_ms ) = millis() - hub->learning->ms;
                    hub->eeprom->write();
                    learning_door->sendStatus();
                    /*
                    msg.clear();
                    send( msg.setSensor( st_eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( st_eeprom.learning_phase ));
                    */
                    hub->eeprom->d.learning_phase = LEARNING_CLOSEWAITFORTIMEOUT;
                    hub->bd_timer->start();
                    /*
                    msg.clear();
                    send( msg.setSensor( st_eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( st_eeprom.learning_phase ));
                    */
                }
                break;
        }
    }
}

/**
 * learning_setFromEeprom:
 * @hub: the core structure.
 * 
 * Initialize the @learning structure with the data read from @eeprom.
 * 
 * Static.
 */
void learning_setFromEeprom( hub_t *hub )
{
    Door *learning_door = Door::GetLearningDoor( hub );

    if( hub->eeprom->d.learning_door == hub->door1->getId()){
        hub->learning->opening_ms = &hub->eeprom->d.door1_opening_ms;
        hub->learning->closing_ms = &hub->eeprom->d.door1_closing_ms;

    } else if( hub->eeprom->d.learning_door == hub->door2->getId()){
        hub->learning->opening_ms = &hub->eeprom->d.door2_opening_ms;
        hub->learning->closing_ms = &hub->eeprom->d.door2_closing_ms;
    }
}

/**
 * learning_startProgram:
 * @hub: the core structure.
 * 
 * Start the learning program (if it is not already running).
 */
void learning_startProgram( hub_t *hub )
{
    uint8_t learning_phase = hub->eeprom->d.learning_phase;

    if( learning_phase == LEARNING_NONE ){

        hub->learning->ms = 0;
        hub->learning->opening_ms = 0;
        hub->learning->closing_ms = 0;

        hub->eeprom->d.learning_phase = LEARNING_OPENHAVETO;
        hub->eeprom->d.learning_door = learning_getNext( hub, 0 );
        hub->eeprom->initLearningDatas();
        hub->eeprom->write();
        hub->average_ms = 0;
        
        if( hub->eeprom->d.learning_door > 0 ){
            /*
            msg.clear();
            send( msg.setSensor( eeprom.learning_door+DOOR_POSITION ).setType( V_VAR1 ).set( eeprom.learning_phase ));
            */
        }
    }
}

/**
 * learning_toStr:
 * 
 * Returns: the learning phase as a string.
 * 
 * Static.
 */
const __FlashStringHelper *learning_toStr( uint8_t phase )
{
    const __FlashStringHelper *str = PGMSTR( learningUnknown );
    
    switch( phase ){
        case LEARNING_NONE:
            str = PGMSTR( learningNone );
            break;
        case LEARNING_OPENHAVETO:
            str = PGMSTR( learningOpenHaveTo );
            break;
        case LEARNING_OPENWAITFORMOVE:
            str = PGMSTR( learningOpenWaitForMove );
            break;
        case LEARNING_OPENING:
            str = PGMSTR( learningOpening );
            break;
        case LEARNING_OPENWAITFORTIMEOUT:
            str = PGMSTR( learningOpenWaitForTimeout );
            break;
        case LEARNING_CLOSEHAVETO:
            str = PGMSTR( learningCloseHaveTo );
            break;
        case LEARNING_CLOSEWAITFORMOVE:
            str = PGMSTR( learningCloseWaitForMove );
            break;
        case LEARNING_CLOSING:
            str = PGMSTR( learningClosing );
            break;
        case LEARNING_CLOSEWAITFORTIMEOUT:
            str = PGMSTR( learningCloseWaitForTimeout );
            break;
    }
    return( str );
}

