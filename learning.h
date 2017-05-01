#ifndef __DOOR_LEARNING_H__
#define __DOOR_LEARNING_H__

#include "door.h"

/* The learning phases
 */
enum {
    LEARNING_NONE = 0,
    LEARNING_OPEN,                            // open the door
    LEARNING_WAITOPEN,                        // wait for actually opening
    LEARNING_OPENING,                         // door is opening, wait for end of move
    LEARNING_CLOSE,                           // close the door
    LEARNING_WAITCLOSE,                       // wait for actually closing
    LEARNING_CLOSING,                         // door is closing, wait for end of move
};

#define LEARNING_OPENCLOSE_WAIT     5000      /* wait 5 sec. between opening and closing a door */
#define LEARNING_BETWEENDOORS_WAIT  5000      /* wait 5 sec. between closing the first door and opening the second one */

/* a data structure to manage the learning program
 */
typedef struct {
    Door          *door;
    Door          *next;
    uint8_t        phase;
    unsigned long  ms;
    unsigned long *opening_ms;
    unsigned long *closing_ms;
}
  sLearning;

const char *learning_dump( uint8_t learning );
void        learning_dump( sLearning &learning );

#endif // __DOOR_LEARNING_H__

