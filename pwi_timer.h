#ifndef __PWI_TIMER_H__
#define __PWI_TIMER_H__

#include <Arduino.h>

// max count of allocatable pwiTimer's
// the array is statically allocated as a global static variable in .cpp file
#define PWITIMER_MAX 15

/*
 * This is a very simple timer which let us trigger a function at the end
 * of a predefined delay.
 * Contrarily to other standard real timers, this one does not rely on any
 * of the internal Arduino timers, nor on any interrupts. Instead this one
 * relies on being called repeatidly on each loop.
 * 
 * Do not expect a great precision here. In best case, it will be about 1-2 ms.
 * The precision may decrease until 10-20 ms if the main loop is too long.
 * 
 * Synopsys:
 * 
 * Define a new timer:
 *   pwiTimer timer.set( delay_ms, once, cb, user_data );
 *   
 *   It is caracterized by the delay, the callback and the user_data.
 *   This timer is not reentrant: once it has been started, it cannot
 *   be reused before having been stopped before.
 * 
 * Start a defined timer:
 *   timer.start();
 *   
 *   The predefined timer is started.
 *   At end of the predefined delay, the callback will be called with
 *   the passed-in user data. At this time, the timer is automatically
 *   disabled, and should be re-started for another use.
 *   This is a one time call timer.
 *   
 * Once more time: this simplissim timer relies on being called by the main loop.
 */

typedef void ( *pwiTimerCb )( void * );

class pwiTimer {
    public:
        pwiTimer( void );
        bool          isStarted();
        void          set( const char *label, unsigned long delay_ms, bool once, pwiTimerCb cb, void *user_data = NULL );
        void          set( unsigned long delay_ms );
        void          setDebug( bool debug );
        void          start( void );
        void          restart( void );

        static void   Dump();
        static void   Loop();

    private:
        unsigned long delay_ms;
        bool          once;
        pwiTimerCb    cb;
        void         *user_data;
        unsigned long start_ms;
        char         *label;
        bool          debug;

        void          objDump( uint8_t idx );
        void          objLoop( void );
        void          objStart( bool restart );
};

#endif // __PWI_TIMER_H__

