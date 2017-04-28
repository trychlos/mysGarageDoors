#ifndef __DOOR_H__
#define __DOOR_H__

#include <Arduino.h>

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

/* The learning phases
 */
enum {
    LEARNING_NONE = 0,
    LEARNING_START,
    LEARNING_OPEN1,
    LEARNING_OPENWAIT1,
    LEARNING_CLOSE1,
    LEARNING_CLOSEWAIT1,
    LEARNING_OPEN2,
    LEARNING_OPENWAIT2,
    LEARNING_CLOSE2,
    LEARNING_CLOSEWAIT2,
};

#define LEARNING_WAIT 1000 /* wait 1 sec. between open and close */

class Door {
    public:
        Door( uint8_t child_id, uint8_t command_pin, uint8_t opening_pin, uint8_t closing_pin, uint8_t closed_pin, uint8_t enabled_pin );

        uint8_t       getChildId( void );
        uint8_t       getCurrentMove( void );
        bool          isEnabled( void );
        void          requestInfo( void );
        void          runLoop( unsigned long average_ms );
        void          pushButton( void );

    private:
        // initialization
        uint8_t       child_id;
        uint8_t       command_pin;
        uint8_t       opening_pin;
        uint8_t       closing_pin;
        uint8_t       closed_pin;
        uint8_t       enabled_pin;

        // the move status for the current loop
        uint8_t       move_loop;

        // the last actual move (not the last move status)
        uint8_t       last_move;
        unsigned long last_begin_ms;
        unsigned long last_end_ms;

        // pushing the button
        unsigned long  button_pushed_ms;

        // the last sent message
        unsigned long last_msg;

        bool           getEnabled( void );
        bool           is_enabled;
};

#endif // __DOOR_H__

