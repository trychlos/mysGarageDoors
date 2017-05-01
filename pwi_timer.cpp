#include "pwi_timer.h"
#include "until_now.h"

// uncomment to debugging this file
#define TIMER_DEBUG

static pwiTimer *st_timers[PWITIMER_MAX];
static uint8_t   st_count = 0;

/**
 * pwiTimer::pwiTimer:
 * 
 * Constructor.
 */
pwiTimer::pwiTimer( void )
{
    this->delay_ms = 0;
    this->once = true;
    this->cb = NULL;
    this->user_data = NULL;
    this->start_ms = 0;
    this->label = NULL;
    this->debug = true;

    if( st_count == 0 ){
        memset( st_timers, '\0', sizeof( st_timers ));
    }
    if( st_count < PWITIMER_MAX ){
        st_timers[st_count++] = this;
    }
}

/**
 * pwiTimer::isStarted:
 * 
 * Returns: %TRUE if the timer is started.
 */
bool pwiTimer::isStarted( void )
{
    return( this->start_ms > 0 ); 
}

/**
 * pwiTimer::set:
 * @label: [allow-none]: a label to identify or qualify the timer;
 *  as we do not copy the string, it must stay valid during the life
 *  of the object.
 * @delay_ms: the duration of the timer; must be greater than zero.
 * @once: whether the @cb callback must be called only once, or regularly.
 * @cb: [allow-none]: the callback.
 * @user_data: [allow-none]: the user data to be passed to the callback.
 * 
 * Initialize the timer.
 * After having been initialized, the timer may be started.
 * The @cb callback will be called at the expiration of the @delay_ms.
 * 
 * If @once is %TRUE, the timer is then disabled, and will stay inactive
 * until started another time.
 * 
 * If @once is %FALSE, the @cb callback will ba called regularly each
 * @delay_ms.
 */
void pwiTimer::set( const char *label, unsigned long delay_ms, bool once, pwiTimerCb cb, void *user_data )
{
    if( strlen( label )){
        this->label = label;
    }
    this->delay_ms = delay_ms;
    this->once = once;
    this->cb = cb;
    this->user_data = user_data;
}

/**
 * pwiTimer::set:
 * @delay_ms: the duration of the timer; must be greater than zero.
 * 
 * Change the configured delay.
 */
void pwiTimer::set( unsigned long delay_ms )
{
    this->delay_ms = delay_ms;
}

/**
 * pwiTimer::setDebug:
 * @debug: whether this timer should be debugged.
 * 
 * Set the @debug flag.
 * 
 * This is mainly used for the main timer which expires very too often
 * to be valuably debugged.
 */
void pwiTimer::setDebug( bool debug )
{
    this->debug = debug;
}

/**
 * pwiTimer::start:
 * 
 * Start a timer.
 * 
 * The pre-set delay must be greater than zero.
 */
void pwiTimer::start( void )
{
    this->objStart( false );
}

/**
 * pwiTimer::restart:
 * 
 * Start the timer,
 * or restart it before its expiration thus reconducting a new delay.
 */
void pwiTimer::restart( void )
{
    this->objStart( true );
}

/**
 * pwiTimer::dump:
 * 
 * Dump.
 * 
 * Public Static
 */
static void pwiTimer::Dump( void )
{
#ifdef TIMER_DEBUG
    Serial.print( F( "[pwiTimer::dump]: st_count=" ));
    Serial.print( st_count );
    Serial.print( "/" );
    Serial.println( PWITIMER_MAX );
    for( uint8_t i=0 ; i<st_count ; ++i ){
        st_timers[i]->objDump( i );
    }
#endif
}

/**
 * pwiTimer::runLoop:
 * 
 * This function is meant to be called from the main loop.
 * 
 * Public Static
 */
static void pwiTimer::Loop( void )
{
    for( uint8_t i=0 ; i<st_count ; ++i ){
        st_timers[i]->objLoop();
    }
}

/**
 * pwiTimer::objDump:
 * 
 * Dump the object.
 * 
 * Private
 */
void pwiTimer::objDump( uint8_t idx )
{
#ifdef TIMER_DEBUG
    Serial.print( F( "[pwiTimer::objDump] idx=" ));
    Serial.print( idx );
    if( strlen( this->label )){
        Serial.print( F( ", label=" ));
        Serial.print( this->label );
    }
    Serial.print( F( ", delay_ms=" ));
    Serial.print( this->delay_ms );
    Serial.print( F( ", once=" ));
    Serial.print( once ? "True":"False" );
    Serial.print( F( ", cb=" ));
    Serial.print(( int ) cb );
    Serial.print( F( ", user_data=" ));
    Serial.print(( int ) user_data );
    Serial.print( F( ", debug=" ));
    Serial.println( debug ? "True":"False" );
#endif
}

/**
 * pwiTimer::objLoop:
 * 
 * If the timer is started, and has expired, then call the callback. 
 * Stop the timer is set for running once.
 * 
 * Private
 */
void pwiTimer::objLoop( void )
{
    if( this->start_ms > 0 ){
        unsigned long duration = untilNow( millis(), this->start_ms );
        if( duration > this->delay_ms ){
#ifdef TIMER_DEBUG
            if( this->debug ){
                Serial.print( F( "[pwiTimer::objLoop] " ));
                if( strlen( this->label )){
                    Serial.print( F( "label=" ));
                    Serial.print( this->label );
                    Serial.print( ", " );
                }
                Serial.print( F( "delay_ms=" ));
                Serial.print( this->delay_ms );
                Serial.print( F( ", start_ms=" ));
                Serial.print( this->start_ms );
                Serial.print( F( ", duration=" ));
                Serial.println( duration );
            }
#endif
            if( this->cb ){
                this->cb( this->user_data );
            }
            if( this->once ){
                this->start_ms = 0;
            } else {
                this->start_ms = millis();
            }
        }
    }
}

/**
 * pwiTimer::objStart:
 * @restart: whether we accept to re-start an already started timer.
 * 
 * Start or restart the timer.
 */
void pwiTimer::objStart( bool restart )
{
    if( this->delay_ms > 0 ){
        if( this->start_ms > 0 && !restart ){
            Serial.println( F( "[pwiTimer::start] ERROR: timer already started" ));
        } else {
            this->start_ms = millis();
        }
    } else {
        Serial.println( F( "[pwiTimer::start] ERROR: unable to start the timer while delay is not set" ));
    }
}

