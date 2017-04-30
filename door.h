#ifndef __DOOR_H__
#define __DOOR_H__

#include "pwi_timer.h"

/* Door children:
 *  - the 'actor' child identifier is declared in the main program;
 *    this is also the target of the C_SET/C_REQ requests.
 *  - the 'cover' child let us send position messages.
 */
enum {
    DOOR_ACTOR = 0,
    DOOR_COVER,
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
        uint8_t       getCurrentMove( void );
        bool          isEnabled( void );
        void          requestInfo( void );
        void          runLoop( unsigned long average_ms );
        void          pushButton( byte level = HIGH );
        void          sendState( void );
        void          sendPosition( void );

        static const char *StateToStr( uint8_t state );

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

        // some timers
        pwiTimer      debounce_timer;
        pwiTimer      push_button_timer;
        pwiTimer      state_timer;
        pwiTimer      position_timer;

        // private functions
        bool          getEnabled( void );
};

#endif // __DOOR_H__

