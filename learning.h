#ifndef __LEARNING_H__
#define __LEARNING_H__

#include <Arduino.h>
#include "hub_def.h"

/* The learning phases
 */
enum {
    LEARNING_NONE = 0,
    LEARNING_OPENHAVETO,                      // have to open the door
    LEARNING_OPENWAITFORMOVE,                 // have pushed the button, wait for moving up
    LEARNING_OPENING,                         // door is opening, wait for end of move
    LEARNING_OPENWAITFORTIMEOUT,              // end of opening, wait for open/close timeout
    LEARNING_CLOSEHAVETO,                     // have to close the door
    LEARNING_CLOSEWAITFORMOVE,                // have pushed the button, wait for actually moving down
    LEARNING_CLOSING,                         // door is closing, wait for end of move
    LEARNING_CLOSEWAITFORTIMEOUT,             // end of closing, wait for between doors timeout
};

/* a data structure to manage the learning program
 */
typedef struct {
    unsigned long  ms;
    unsigned long *opening_ms;
    unsigned long *closing_ms;
}
  learning_t;

void                       learning_dump( hub_t *hub );
uint8_t                    learning_getNext( hub_t *hub, uint8_t cur_id );
void                       learning_onBetweenDoorsCb( hub_t *hub );
void                       learning_onOpenCloseCb( hub_t *hub );
void                       learning_runProgram( hub_t *hub, uint8_t phase );
void                       learning_setFromEeprom( hub_t *hub );
void                       learning_startProgram( hub_t *hub );
const __FlashStringHelper *learning_toStr( uint8_t phase );

#endif // __LEARNING_H__

