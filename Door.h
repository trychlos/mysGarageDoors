#ifndef __DOOR_H__
#define __DOOR_H__

#include "hub_def.h"
#include <pwiTimer.h>

/* Door MySensors children:
 *  - the 'main' child identifier is declared in the main program;
 *    this is also the target of the C_SET/C_REQ requests from the controller.
 *  - the 'up' and 'down' children let us send up/down binary messages.
 *  - the 'position' child let us send position messages.
 */
enum {
    DOOR_MAIN = 0,
    DOOR_UPDOWN,          // 1
    DOOR_POSITION,        // 2
    DOOR_CLOSED,          // 3
    DOOR_OPENMS,          // 4
    DOOR_CLOSEMS,         // 5
};

/* The door current move
 */
enum {
    DOOR_NOMOVE = 1,
    DOOR_MOVEUP,
    DOOR_MOVEDOWN,
};

class Door {
    public:
                                  Door( uint8_t child_id, uint8_t command_pin, uint8_t opening_pin, uint8_t closing_pin, uint8_t closed_pin, uint8_t enabled_pin );

        virtual void              debounce( void );
        virtual uint8_t           getId( void );
        virtual bool              getClosed( void );
        virtual bool              getEnabled( void );
        virtual uint8_t           getMoveId( void );
        virtual uint8_t           getPosition( void );
        virtual void              present();
        virtual void              pushButton( byte level=HIGH );
        virtual void              runLoop( hub_t *hub );
        virtual void              sendMove( void );
        virtual void              sendStatus( void );
        virtual void              setBtnPushDelay( unsigned long delay_ms );
        virtual void              setMinPublished( uint8_t min_change );
        virtual void              setMoveId( uint8_t move_id );
        virtual void              setMoveDebounceDelay( unsigned long delay_ms );
        virtual void              setUnchangedDelay( unsigned long delay_ms );
        virtual void              setup( hub_t *hub );

        static  const char       *MoveToStr( uint8_t move_id );
        static  Door             *GetLearningDoor( hub_t *hub );

    private:
        /* construction data
         */
                uint8_t           id;
                uint8_t           command_pin;
                uint8_t           opening_pin;
                uint8_t           closing_pin;
                uint8_t           closed_pin;
                uint8_t           enabled_pin;

        /* runtime data
         */
                bool              is_enabled;               // whether this door is enabled
                bool              is_closed;                // whether this door is closed (from the contact)

        // the current move_id after debouncing
                uint8_t           move_id;
                unsigned long     move_begin_ms;
                uint8_t           debounced_move;

        // the last known position
        // 0% is fully closed, 100% is fully opened
                uint8_t           pos;

        // the last sent position
                uint8_t           last_pos;
                uint8_t           min_change;

        // some timers
                pwiTimer          debounce_timer;
                pwiTimer          btn_push_timer;
                pwiTimer          unchanged_timer;

        /* private functions
         */
                bool              readClosed( void );
                bool              readEnabled( void );
                uint8_t           readCurrentMove( bool debounce=true );
};

#endif // __DOOR_H__

