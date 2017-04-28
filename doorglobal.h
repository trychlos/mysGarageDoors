#ifndef __DOORGLOBAL_H__
#define __DOORGLOBAL_H__

/* Declare here the global function which are called from the Door class
 */

void doorSendState( uint8_t child_id, uint8_t move_id );
void doorSendPosition( uint8_t child_id, uint8_t pos );

#endif // __DOORGLOBAL_H__

