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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "mem.h"
#include "lang.h"
#include "robot.h"
#include "bullet.h"
#include "queue.h"
#include "callback_num.h"
#include "comms.h"
#include "timeout.h"
#include "robotlist.h"
#include "prototypes.h"


#define MAX(X,Y)	((X) > (Y) ? (X) : (Y))
#define MIN(X,Y)	((X) < (Y) ? (X) : (Y))


typedef void LogFunc( char *, ... );
extern LogFunc *log_print;

extern RobotList *robot_list;
extern int world_update_counter;
extern List *start_point_list;

Robot *current_robot;


/*
 *  Create a new instance of the Robot
 *
 *  In:
 *      robot_def: Name of the robot definition file
 *
 *  Out:
 *      The newly created robot instance
 *      or NULL if the Robot could not be created
 *
 */

static int robot_id_counter = 0;

Robot *robot_create( char *def_file )
{
  return robot_create_with_id( def_file, robot_id_counter++ );
}

Robot *robot_create_with_id( char *def_file, int newid )
{
  Robot *ret = mymalloc( sizeof( Robot ) );
  int cwd_size;

  if( !parse_def_file( ret, def_file ) ) {
    log_print( "robot load failed" );
    myfree( ret );
    return NULL;
  }

  cwd_size = 0;
  do {
    cwd_size += 256;
    ret->robot_dir = getcwd( NULL, cwd_size );
  } while( ret->robot_dir == NULL );

  ret->robot_filename = mystrdup( def_file );
  ret->start_x = 0;
  ret->start_y = 0;
  ret->x_pos = 0;
  ret->y_pos = 0;
  ret->current_heading = 0;
  ret->destination_heading = 0;
  ret->current_speed = 0;
  ret->destination_speed = 0;
  ret->current_shields = ret->max_shields;
  ret->update_counter = 0;
  ret->id = newid;
  ret->invulnerable = 0;

  ret->weapon_recharge_status = 0;
  ret->weapon_direction = 0;

  ret->scanner_recharge_status = 0;
  ret->last_scanner_direction = 0;
  ret->last_scanner_width = 0;

  ret->waiting_messages = queue_create();
  ret->listener_channel = 0;

  ret->timeouts = list_create();

  log_print( "Loaded robot: %s, id: %d", ret->robot_name, ret->id );

  return ret;
}


void robot_delete( Robot *robot )
{
  CallbackValues *cbvalues;
  MessageData *message;
  Timeout *timeout;

  /* Delete the list of waiting callbacks */
  while( queue_num_values( robot->program->waiting_callbacks ) > 0 ) {
    cbvalues = queue_pop_ptr( robot->program->waiting_callbacks );
    myfree( cbvalues );
  }
  queue_delete( robot->program->waiting_callbacks );

  if( robot->program->current_callback_values != NULL ) {
    myfree( robot->program->current_callback_values );
  }

  /* Delete the stacks */
  myfree( robot->program->usrstack );
  myfree( robot->program->sysstack );

  /* Delete memory */
  myfree( robot->program->mem );

  /* Delete the code */
  myfree( robot->program->code );

  /* Delete the program */
  myfree( robot->program );

  /* Delete the robot name */
  myfree( robot->robot_name );

  /* Code filename */
  myfree( robot->code_filename );

  /* Robot definition filename */
  myfree( robot->robot_filename );

  /* Robot dir */
  /* note that this is free() because the buffer was allocated using getcwd() */
  free( robot->robot_dir );

  /* Delete the list of waiting messages */
  while( queue_num_values( robot->waiting_messages ) > 0 ) {
    message = queue_pop_ptr( robot->waiting_messages );
    if( --message->refcnt == 0 ) {
      myfree( message->data );
      myfree( message );
    }
  }
  queue_delete( robot->waiting_messages );

  /* Delete the list of waiting timeouts */
  while( list_has_one_entry( robot->timeouts ) ) {
    timeout = list_get_last_ptr( robot->timeouts );
    myfree( timeout );
    list_delete_last( robot->timeouts );
  }
  list_delete( robot->timeouts );

  /* Delete all bullets owned by this robot */
  delete_bullets_from_robot( robot );

  /* Delete the robot struct itself */
  myfree( robot );
}


/*
 *  Place the robot at a random position
 */
void robot_place_on_map( Robot *robot )
{
  List *free_pos_list = list_create();
  StartPoint *startpoint;
  Robot *w;
  int index_save;
  int found;

  /*
   *  Create a list of all free starting positions
   */
  if( start_point_list != NULL ) {
    index_save = robot_list->walkindex;
    list_init_walk( start_point_list );
    while( (startpoint = list_walk_next_ptr( start_point_list )) != NULL ) {
      found = 0;
      robotlist_init_walk( robot_list );
      while( (w = robotlist_walk_next( robot_list )) != NULL ) {
	if( startpoint->x == w->start_x &&
	    startpoint->y == w->start_y ) {
	  found = 1;
	  break;
	}
      }
      if( !found ) {
	list_add_to_tail_ptr( free_pos_list, startpoint );
      }
    }
    robot_list->walkindex = index_save;
  }

  /*
   *  If there are no free starting positions, just place the
   *  robot at a random position
   */
  if( list_is_empty( free_pos_list ) ) {
    robot->start_x = 50 + (random() % (MAP_MAX_X - 100));
    robot->start_y = 50 + (random() % (MAP_MAX_Y - 100));
  }
  else {
    startpoint = list_get_pos_ptr( free_pos_list,
				random() % list_size( free_pos_list ) );
    robot->start_x = startpoint->x;
    robot->start_y = startpoint->y;
  }
  robot->x_pos = robot->start_x;
  robot->y_pos = robot->start_y;

  list_delete( free_pos_list );
}

/*
 *  Set the robot destination speed
 */

void robot_set_destination_speed( Robot *robot, int newspeed )
{
  if( newspeed < 0 ) {
    robot->destination_speed = 0;
  }
  else if( newspeed > robot->max_speed ) {
    robot->destination_speed = robot->max_speed;
  }
  else {
    robot->destination_speed = newspeed;
  }
}


/*
 *  Set the robot destination heading
 */

void robot_set_destination_heading( Robot *robot, int newheading )
{
  robot->destination_heading = fix_direction( newheading );
}


/*
 *  Execute a timeslice,
 *  the cycle counter value is currently hardcoded to 10
 *  (i.e. 10 cycles for each time unit in the game)
 */

void robot_execute_instructions( Robot *robot )
{
  int c;
  CallbackValues *cbvalues;
  MessageData *message;
  Timeout *timeout;

  current_robot = robot;

  for( c = 0 ; c < 10 ; c++ ) {
    if( !evaluate_instruction( robot->program ) ) {
      /*
       *  Runtime error.
       *  Clear the user and system stacks, clear the callbacks.
       *  Set the PC to 0 (restart the program)
       */
      stack_delete( robot->program->usrstack );
      stack_delete( robot->program->sysstack );
      robot->program->usrstack = stack_create();
      robot->program->sysstack = stack_create();


      while( queue_num_values( robot->program->waiting_callbacks ) > 0 ) {
	cbvalues = queue_pop_ptr( robot->program->waiting_callbacks );
	myfree( cbvalues );
      }

      while( queue_num_values( robot->waiting_messages ) > 0 ) {
	message = queue_pop_ptr( robot->waiting_messages );
	if( --message->refcnt == 0 ) {
	  myfree( message->data );
	  myfree( message );
	}
      }

      while( list_has_one_entry( robot->timeouts ) ) {
	timeout = list_get_last_ptr( robot->timeouts );
	myfree( timeout );
	list_delete_last( robot->timeouts );
      }

      robot->program->pc = 0;
    }
  }
}


/*
 *  Update the robot status
 *
 *  Operations performed:
 *
 *    Update the update counter
 *    Turn the robot
 *    Update the robot speed
 *    Move the robot
 *    Count down the weapon and scanner recharge values
 *    Check the timeouts
 */

void robot_update( Robot *robot )
{
  int turn_val;
  Timeout *timeout;
  int end_timeouts;

  /* update the update counter */
  robot->update_counter++;

  /* update the robot rotation */
  if( (turn_val = direction_diff( robot->current_heading,
				  robot->destination_heading )) > 0 ) {
    /* right turn */
    robot->current_heading += MIN( turn_val, robot->turn_speed );
  }
  else if( turn_val < 0 ) {
    /* left turn */
    robot->current_heading -= MIN( -turn_val, robot->turn_speed );
  }
  robot->current_heading = fix_direction( robot->current_heading );

  /* update the robot speed */
  if( robot->destination_speed < robot->current_speed ) {
    /* braking */
    if( robot->current_speed - robot->destination_speed > robot->brake_power ) {
      robot->current_speed = MAX( robot->current_speed -
				  robot->brake_power, 0 );
    }
    else {
      robot->current_speed = robot->destination_speed;
    }
  }
  else if( robot->destination_speed > robot->current_speed ) {
    /* accelerating */
    robot->current_speed = MIN( robot->current_speed +
				robot->accel_power,
				MAX( robot->destination_speed,
				     robot->max_speed ) );
  }

  move_robot( robot );

  /* count down the weapon recharge value */
  robot->weapon_recharge_status = MAX( robot->weapon_recharge_status -
				       robot->weapon_recharge_speed, 0 );

  /* count down the scanner recharge value */
  robot->scanner_recharge_status = MAX( robot->scanner_recharge_status -
					robot->scanner_recharge_speed, 0 );


  /* check the timeouts */
  end_timeouts = 0;
  while( !list_is_empty( robot->timeouts ) && !end_timeouts ) {
    timeout = list_get_first_ptr( robot->timeouts );
    if( timeout->attime <= world_update_counter ) {
      make_cbvalues_call_callback( robot->program, CB_TIMEOUT, timeout->id );
      myfree( timeout );
      list_delete_first( robot->timeouts );
    }
    else {
      end_timeouts = 1;
    }
  }

      

}

void robot_take_damage( Robot *robot, int damage )
{
  int damage2 = MIN( damage, robot->current_shields );

  if( !robot->invulnerable ) {
    robot->current_shields -= damage2;
    if( robot->current_shields > 0 ) {
      make_cbvalues_call_callback( robot->program, CB_TAKE_DAMAGE, damage2 );
    }
    else {
      log_print( "Robot %s destroyed", robot->robot_name );
      robotlist_delete_robot( robot_list, robot );
    }
  }
}

int robot_scan( Robot *robot, int scan_direction, int scan_width )
{

  if( robot->scanner_recharge_status > 0 ) {
    return robot->scanner_recharge_status;
  }

  robot->last_scanner_direction = scan_direction;
  robot->last_scanner_width = scan_width;
  robot->scanner_recharge_status = scan_width * 10;

  scan_for_robots( robot, scan_direction, scan_width );
  return 0;
}

void robot_wallscan( Robot *robot, int direction, int range )
{
  scan_for_walls( robot, direction, range );
}

int robot_add_timeout( Robot *robot, int attime )
{
  ListEntry *entry;
  ListEntry *timeout_pos = NULL;
  Timeout *ret = mymalloc( sizeof( Timeout ) );
  static int timeout_id = 0;

  ret->attime = attime + world_update_counter;
  ret->id = timeout_id++;

  entry = robot->timeouts->first;

  while( entry != NULL ) {
    if( timeout_pos == NULL &&
	((Timeout *)(entry->val.ptr))->attime <= attime ) {
      timeout_pos = entry;
    }
    if( ((Timeout *)(entry->val.ptr))->id == ret->id ) {
      entry = robot->timeouts->first;
      timeout_pos = NULL;
      ret->id = timeout_id++;
    }
    else {
      entry = entry->next;
    }
  }

  if( entry == NULL ) {
    list_add_to_tail_ptr( robot->timeouts, ret );
  }
  else {
    list_add_after_ptr( robot->timeouts, timeout_pos, ret );
  }

  return ret->id;
}

void robot_deleteTimeout( Robot *robot, int timeout_id )
{
  ListEntry *entry;
  Timeout *timeout;

  entry = robot->timeouts->first;
  while( entry != NULL ) {
    timeout = entry->val.ptr;
    if( timeout->id == timeout_id ) {
      myfree( timeout );
      list_delete_list_entry( robot->timeouts, entry );
      break;
    }
  }
}
