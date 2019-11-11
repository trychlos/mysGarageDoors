
#include <core/MySensorsCore.h>
#include "Eeprom.h"
#include "learning.h"

/*
 * pwi 2019-10- 6 transform to a class
 */

// uncomment to debugging this file
//#define EEPROM_DEBUG

/**
 * Eeprom::Eeprom:
 * 
 * Constructor.
 *
 * Public.
 */
Eeprom::Eeprom( void )
{
    this->init_obj();
}

/**
 * Eeprom::computeAverage:
 *
 * Compute the average opening/closing duration.
 * 
 * Public.
 */
unsigned long Eeprom::computeAverage()
{
    uint8_t count = 0;
    unsigned long average = 0;
    if( this->d.door1_opening_ms > 0 ){
        average += this->d.door1_opening_ms;
        count += 1;
    }
    if( this->d.door1_closing_ms > 0 ){
        average += this->d.door1_closing_ms;
        count += 1;
    }
    if( this->d.door2_opening_ms > 0 ){
        average += this->d.door2_opening_ms;
        count += 1;
    }
    if( this->d.door2_closing_ms > 0 ){
        average += this->d.door2_closing_ms;
        count += 1;
    }
    if( count > 0 ){
        average /= count;
    }
#ifdef EEPROM_DEBUG
    Serial.print( F( "Eeprom::computeAverage() average=" ));
    Serial.println( average );
#endif
    return( average );
}

/**
 * Eeprom::dump:
 *
 * Dump the Eeprom content.
 * 
 * Public.
 */
void Eeprom::dump( void )
{
#ifdef EEPROM_DEBUG
    Serial.print( F( "Eeprom::dump()              mark='" )); Serial.print( this->d.mark ); Serial.println( "'" );
    Serial.print( F( "Eeprom::dump()           version=" )); Serial.println( this->d.version );
    Serial.print( F( "Eeprom::dump() unchanged_timeout=" )); Serial.print( this->d.unchanged_timeout_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()     min_published=" )); Serial.print( this->d.min_published_pct ); Serial.println( "%" );
    Serial.print( F( "Eeprom::dump()        react_time=" )); Serial.print( this->d.react_time_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()  learning_oc_wait=" )); Serial.print( this->d.learning_oc_wait_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()  learning_bd_wait=" )); Serial.print( this->d.learning_bd_wait_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()     move_debounce=" )); Serial.print( this->d.move_debounce_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()          btn_push=" )); Serial.print( this->d.btn_push_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()     door1_opening=" )); Serial.print( this->d.door1_opening_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()     door1_closing=" )); Serial.print( this->d.door1_closing_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()     door2_opening=" )); Serial.print( this->d.door2_opening_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()     door2_closing=" )); Serial.print( this->d.door2_closing_ms ); Serial.println( "ms" );
    Serial.print( F( "Eeprom::dump()    learning_phase=" )); Serial.println( learning_dump( this->d.learning_phase ));
    Serial.print( F( "Eeprom::dump()     learning_door=" )); Serial.println( this->d.learning_door );
#endif
}

/**
 * Eeprom::initLearningDatas:
 *
 * Initialize the doors learning datas.
 * 
 * Public.
 */
void Eeprom::initLearningDatas( void )
{
    this->d.door1_opening_ms = 0;
    this->d.door1_closing_ms = 0;
    this->d.door2_opening_ms = 0;
    this->d.door2_closing_ms = 0;
}

/**
 * Eeprom::read:
 * 
 * Read the EEPROM content into this object.
 *
 * Public.
 */
void Eeprom::read( void )
{
#ifdef EEPROM_DEBUG
    Serial.println( F( "Eeprom::read()" ));
#endif
    for( uint8_t i=0 ; i<sizeof( eeprom_t ); ++i ){
        (( uint8_t * ) &this->d )[i] = loadState( i );
    }
    // initialize with default values if mark not found
    if( this->d.mark[0] != 'P' || this->d.mark[1] != 'W' || this->d.mark[2] != 'I' || this->d.mark[3] != 0 ){
        this->reset();
    }
    if( this->d.version == 0 || this->d.version > EEPROM_VERSION ){
        this->reset();
    }
    this->dump();
}

/**
 * Eeprom::reset:
 *
 * RAZ the EEPROM, resetting it to default values.
 *
 * Public.
 */
void Eeprom::reset()
{
#ifdef EEPROM_DEBUG
    Serial.print( F( "Eeprom::reset() begin=" ));
    Serial.println( millis());
#endif
    this->init_full();
    strcpy( this->d.mark, "PWI" );
    this->d.version = EEPROM_VERSION;
    // user data
    this->d.unchanged_timeout_ms = 3600000;  // 1h
    this->d.min_published_pct = 10;
    this->d.react_time_ms = 250;
    this->d.learning_oc_wait_ms = 5000;      // 5s
    this->d.learning_bd_wait_ms = 5000;      // 5s
    this->d.move_debounce_ms = 500;
    this->d.btn_push_ms = 500;
    this->initLearningDatas();
    this->d.learning_phase = LEARNING_NONE;
    this->d.learning_door = 0;

    this->write();
#ifdef EEPROM_DEBUG
    Serial.print( F( "Eeprom::reset() end=" ));
    Serial.println( millis());
#endif
}

/**
 * Eeprom::setLearningDoor:
 * @id: the identifier of the next learning door.
 *
 * Record the new door identifier for the learning program.
 *
 * Public.
 */
void Eeprom::setLearningDoor( uint8_t id )
{
    this->d.learning_door = id;
}

/**
 * Eeprom::setLearningPhase:
 * @phase: the learning phase to be recorded.
 *
 * Record the new learning phase in the eeprom_t structure.
 *
 * Public.
 */
void Eeprom::setLearningPhase( uint8_t phase )
{
    this->d.learning_phase = phase;
}

/**
 * Eeprom::write:
 *
 * Write the data to the EEPROM.
 * 
 * Public.
 */
void Eeprom::write( void )
{
#ifdef EEPROM_DEBUG
    Serial.println( F( "Eeprom::write()" ));
#endif
    uint8_t i;
    for( i=0 ; i<sizeof( eeprom_t ) ; ++i ){
        saveState( i, (( uint8_t * ) &this->d )[i] );
    }
}

/**
 * Eeprom::init_obj:
 *
 * Initialize the Eeprom object.
 *
 * Private.
 */
void Eeprom::init_obj( void )
{
    memset( &this->d, '\0', sizeof( eeprom_t ));
}

/**
 * Eeprom::init_full:
 *
 * Initialize the Eeprom object and reset the whole EEPROM user space to zero.
 *
 * Private.
 */
void Eeprom::init_full()
{
    this->init_obj();

    for( uint16_t i=0 ; i<EEPROM_SIZE ; ++i ){
        saveState(( uint8_t ) i, 0 );
    }
}

