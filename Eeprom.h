#ifndef __DOOR_EEPROM_H__
#define __DOOR_EEPROM_H__

#include <Arduino.h>

/*
 * The mysGarageDoors EEPROM structure.
 * 
 * MySensors let 256 bytes usable to the user program.
 * We use it to store this structure.
 * 
 * pos  type           size  content
 * ---  -------------  ----  ------------------------------
 *   0  char              4  mark
 *   4  unsigned long     4  unchanged timeout (ms)
 *   8  uint8_t           1  min published (pct)
 *   9  unsigned long     4  react time (ms)
 *  13  unsigned long     4  learning OC wait (ms)
 *  17  unsigned long     4  learning BD wait (ms)
 *  21  unsigned long     4  move debounce timeout (ms)
 *  25  unsigned long     4  button push pulse width (ms)
 *  29  unsigned long     4  door 1 opening duration (ms)
 *  33  unsigned long     4  door 1 closing duration (ms)
 *  37  unsigned long     4  door 2 opening duration (ms)
 *  41  unsigned long     4  door 2 closing duration (ms)
 *  45  uint8_t           1  current learning phase
 *  46  uint8_t           1  current learning door child id
 *  47  uint8_t           1  version
 *  48
 *
 * pwi 2019-10- 6 transform to a class
 *                add version number = 2
 */

typedef struct {
    /* a 'PWI' null-terminated string which marks the structure as initialized */
    char     mark[4];
    uint8_t  version;
    /* user data */
    uint32_t unchanged_timeout_ms;
    uint8_t  min_published_pct;
    uint32_t react_time_ms;
    uint32_t learning_oc_wait_ms;
    uint32_t learning_bd_wait_ms;
    uint32_t move_debounce_ms;
    uint32_t btn_push_ms;
    uint32_t door1_opening_ms;
    uint32_t door1_closing_ms;
    uint32_t door2_opening_ms;
    uint32_t door2_closing_ms;
    uint8_t  learning_phase;
    uint8_t  learning_door;
}
  eeprom_t;

#define EEPROM_SIZE     256
#define EEPROM_VERSION    2

class Eeprom {
    public:
                                  Eeprom( void );
        virtual unsigned long     computeAverage( void );
        virtual void              dump( void );
        virtual void              initLearningDatas( void );
        virtual void              read( void );
        virtual void              reset( void );
        virtual void              setLearningDoor( uint8_t id );
        virtual void              setLearningPhase( uint8_t phase );
        virtual void              write( void );

        // the eeprom_t structure is made public to get rid of all getters/setters
        //  which appear rather useless in our architecture
                eeprom_t          d;

    private:
        /* construction data
         */

        /* runtime data
         */

        /* private methods
         */
                void              init_obj( void );
                void              init_full( void );
};

#endif // __DOOR_EEPROM_H__

