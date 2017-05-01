#ifndef __DOOR_H__
#define __DOOR_H__

#include "pwi_timer.h"

/* Door children:
 *  - the 'main' child identifier is declared in the main program;
 *    this is also the target of the C_SET/C_REQ requests from the controller.
 *  - the 'up' and 'down' children let us send up/down binary messages.
 *  - the 'position' child let us send position messages.
 */
enum {
    DOOR_MAIN = 0,
    DOOR_UP,
    DOOR_DOWN,
    DOOR_POSITION,
    DOOR_CLOSED,
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

        uint8_t       getChildId( void );
        bool          getClosed( void );
        bool          getEnabled( void );
        uint8_t       getMove( void );
        uint8_t       getPosition( void );
        void          runLoop( unsigned long average_ms );
        void          pushButton( byte level = HIGH );
        void          sendStatus( void );
        void          setMinPositionChange( uint8_t min_change );

        static const char *MoveToStr( uint8_t move_id );

    private:
        // initialization
        uint8_t       child_id;
        uint8_t       command_pin;
        uint8_t       opening_pin;
        uint8_t       closing_pin;
        uint8_t       closed_pin;
        uint8_t       enabled_pin;

        // whether this door is enabled
        bool          is_enabled;

        // whether this door is closed
        bool          is_closed;

        // the move status for the current loop
        uint8_t       move_loop;

        // the last actual move
        // (which is not the move status from the previous loop)
        uint8_t       last_move;
        unsigned long last_begin_ms;
        unsigned long last_end_ms;

        // the last known position
        // 0% is fully closed, 100% is fully opened
        uint8_t       pos;

        // the last sent position
        uint8_t       last_pos;
        uint8_t       min_change;

        // some timers
        pwiTimer      debounce_timer;
        pwiTimer      push_button_timer;

        // private functions
        bool          readClosed( void );
        bool          readEnabled( void );
        uint8_t       readCurrentMove( void );
};

#endif // __DOOR_H__

