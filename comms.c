/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-98  Elias Martenson (elias.martenson@sun.se)
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "config.h"

#include <string.h>

#include "robot.h"
#include "robotlist.h"
#include "callback_num.h"
#include "comms.h"
#include "mem.h"
#include "prototypes.h"

extern RobotList *robot_list;


/*
 *  Returns the number of robots that receieved this message or -1 if
 *  it caused a runtime exception
 */
int send_message( Robot *robot, int dest, int message_id, int msglen,
		  int msgptr )
{
  MessageData *ret;
  int found = 0;
  Robot *w;

  if( msgptr + msglen > robot->program->mem_size ) {
    return -1;
  }

  robotlist_init_walk( robot_list );
  while( (w = robotlist_walk_next( robot_list )) != NULL ) {
    if( w != robot && 
	w->program->callbacks[ CB_MESSAGE_RECEIVED ] != -1 &&
	w->listener_channel == dest ) {
      if( found == 0 ) {
	ret = mymalloc( sizeof( MessageData ) );
	ret->refcnt = 0;
	ret->id = message_id;
	ret->len = msglen;
	ret->data = mymalloc( msglen * sizeof( int ) );
	memcpy( ret->data, &robot->program->mem[ msgptr ],
		msglen * sizeof( int ) );
      }
      found++;
      ret->refcnt++;
      QueuePushPtr( w->waiting_messages, ret );

      make_cbvalues_call_callback( w->program, CB_MESSAGE_RECEIVED, robot->id,
				   message_id, msglen );
    }
  }

  return found;
}


int receieve_message( Robot *robot, int msgptr )
{
  MessageData *message;

  if( QueueNumValues( robot->waiting_messages ) == 0 ) {
    return 0;
  }

  message = QueuePopPtr( robot->waiting_messages );

  if( msgptr + message->len > robot->program->mem_size ) {
    if( --message->refcnt == 0 ) {
      myfree( message->data );
      myfree( message );
    }
    return 0;
  }

  memcpy( &robot->program->mem[ msgptr ], message->data,
	  message->len * sizeof( int ) );

  if( --message->refcnt == 0 ) {
    myfree( message->data );
    myfree( message );
  }

  return 1;
}
