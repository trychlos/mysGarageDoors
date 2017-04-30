#ifndef __DOOR_EEPROM_H__
#define __DOOR_EEPROM_H__

/*
 * MySensors let 256  bytes usable
 * 
 * pos  type           size  content
 * ---  -------------  ----  ------------------------------
 *   0  unsigned long     4  door 1 opening duration (ms)
 *   4  unsigned long     4  door 1 closing duration (ms)
 *   8  unsigned long     4  door 2 opening duration (ms)
 *  12  unsigned long     4  door 2 closing duration (ms)
 *  
 * Please note that this EEPROM code must be written in main .ino program
 * in order to take advantage of the MySensors library (which does not want
 * to be compiled from other source files).
 */

typedef struct {
    unsigned long door1_opening_ms;
    unsigned long door1_closing_ms;
    unsigned long door2_opening_ms;
    unsigned long door2_closing_ms;
}
  sEeprom;

unsigned long eeprom_compute_average( sEeprom &sdata );
void          eeprom_dump( sEeprom &sdata );
void          eeprom_read( sEeprom &sdata );
void          eeprom_write( sEeprom &sdata );
void          eeprom_raz( void );

#endif // __DOOR_EEPROM_H__

