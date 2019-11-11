#ifndef __HUB_H__
#define __HUB_H__

#include "Door.h"
#include "Eeprom.h"
#include "learning.h"
#include "hub_def.h"
#include <pwiTimer.h>

/* a data structure which hosts our global variables
 *  it is to be used as a core global object
 */
struct hub_str {
    unsigned long  average_ms;
    Door          *door1;
    Door          *door2;
    Eeprom        *eeprom;
    pwiTimer      *oc_timer;
    pwiTimer      *bd_timer;
    learning_t    *learning;
};

#endif __HUB_H__

