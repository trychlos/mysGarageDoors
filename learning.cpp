#include "learning.h"

// uncomment to debugging this file
#define LEARNING_DEBUG

/**
 * learning_dump:
 * 
 * Returns: the learning mode as a string.
 * 
 * Static
 */
const char *learning_dump( uint8_t learning )
{
    const char *str = "Unknown learning mode";
    
    switch( learning ){
        case LEARNING_NONE:
            str = "LEARNING_NONE";
            break;
        case LEARNING_OPEN:
            str = "LEARNING_OPEN";
            break;
        case LEARNING_WAITOPEN:
            str = "LEARNING_WAITOPEN";
            break;
        case LEARNING_OPENING:
            str = "LEARNING_OPENING";
            break;
        case LEARNING_CLOSE:
            str = "LEARNING_CLOSE";
            break;
        case LEARNING_WAITCLOSE:
            str = "LEARNING_WAITCLOSE";
            break;
        case LEARNING_CLOSING:
            str = "LEARNING_CLOSING";
            break;
    }
    return( str );
}

/**
 * learning_dump:
 * 
 * Dump the data structure.
 * 
 * Static
 */
void learning_dump( sLearning &learning )
{
#ifdef LEARNING_DEBUG
    Serial.print( F( "[learning_dump] door=" ));
    Serial.print(( int ) learning.door, HEX );
    Serial.print( F( ", next=" ));
    Serial.print(( int ) learning.next, HEX );
    Serial.print( F( ", phase=" ));
    Serial.print( learning_dump( learning.phase ));
    Serial.print( F( ", ms=" ));
    Serial.println( learning.ms );
#endif
}

