/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-2020  Elias Martenson (lokedhs@gmail.com)
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

#ifndef ROBOT_H
#define ROBOT_H

#include "assembler.h"

typedef struct {

  char *robot_name;		/* Name of the robot */
  Program *program;		/* Robot program */
  char *robot_filename;		/* Robot definition filename */
  char *code_filename;		/* Code filename */
  char *robot_dir;		/* Directory where the robot is located */
  int id;			/* Robot ID */

  /* robot charasteristics */
  int accel_power;		/* Acceleration */
  int brake_power;		/* Brake power */
  int max_speed;		/* Maximum speed */
  int turn_speed;		/* Turning speed */
  int max_shields;		/* Maximum shields */
  int invulnerable;		/* Robot is invulnerable */

  /* time counter */
  int update_counter;		/* Number of updates */

  /* damage control */
  int current_shields;		/* Current shields */

  /* robots initial position */
  int start_x;			/* start x position */
  int start_y;			/* start y position */

  /* robot position */
  double x_pos;			/* X position */
  double y_pos;			/* Y position */
  int current_heading;		/* Heading */
  int destination_heading;	/* Heading the robot is turning to */

  /* robot movement */
  int current_speed;		/* Current speed */
  int destination_speed;	/* Speed the robot is changing to */
  
  /* weapon */
  int weapon_direction;		/* Direction the weapon is pointing */
  int weapon_turn_speed;	/* Speed the weapon can rotate */
  int weapon_power;		/* The ammount of damage the weapon inflicts */
  int weapon_speed;		/* Speed of the bullets */
  int weapon_range;		/* Range of the weapon */
  int weapon_recharge_speed;	/* Time to recharge */
  int weapon_recharge_status;	/* Time left before recharged */

  /* scanner */
  int last_scanner_direction;	/* Direction of last scan */
  int last_scanner_width;	/* Width of last scan */
  int scanner_recharge_speed;	/* Time to recharge */
  int scanner_recharge_status;	/* Time left before recharged */
  int scanner_range;		/* Scanner range */

  /* communication */
  Queue *waiting_messages;	/* Message waiting to be delivered */
  int listener_channel;		/* Channel the robot is listening to */

  /* timeouts */
  List *timeouts;		/* List of waiting timeouts */

} Robot;

#define robot_get_id(robot)		((robot)->id)
#define robot_get_name(robot)		((robot)->robot_name)
#define robot_get_x_coordinate(robot)	((int)((robot)->x_pos))
#define robot_get_y_coordinate(robot)	((int)((robot)->y_pos))
#define robot_get_heading(robot)		((robot)->current_heading)
#define robot_get_speed(robot)		((robot)->current_speed)
#define robot_get_maxspeed(robot)		((robot)->max_speed)
#define robot_get_weaponspeed(robot)	((robot)->weapon_speed)
#define robot_set_x_coordinate(robot,X)	((robot)->x_pos = (X))
#define robot_set_y_coordinate(robot,Y)	((robot)->y_pos = (Y))
#define robot_get_shields(robot)		((robot)->current_shields)
#define robot_get_update_counter(robot)	((robot)->update_counter)

#define robot_get_last_scanner_direction(robot)	\
					((robot)->last_scanner_direction)
#define robot_get_last_scanner_width(robot)	((robot)->last_scanner_width)
#define robot_set_last_scanner_width(robot,X)	\
					((robot)->last_scanner_width = (X))
#define robot_get_scanner_status(robot)	((robot)->scanner_recharge_status)

#define robot_get_scanner_range(robot)	((robot)->scanner_range)
#define robot_get_weapon_status(robot)	((robot)->weapon_recharge_status)
#define robot_set_weapon_status(robot,X)	((robot)->weapon_recharge_status = (X))
#define robot_get_weapon_power(robot)	((robot)->weapon_power)
#define robot_get_weapon_range(robot)	((robot)->weapon_range)
#define robot_get_invulnerable(robot)	((robot)->invulnerable)
#define robot_set_invulnerable(robot,X)	((robot)->invulnerable = (X))

Robot *robot_create( char * );
Robot *robot_create_with_id( char *, int );
void robot_delete( Robot * );
void robot_place_on_map( Robot * );
void robot_set_destination_speed( Robot *, int );
void robot_set_destination_heading( Robot *, int );
void robot_execute_instructions( Robot * );
void robot_update( Robot * );
void robot_take_damage( Robot *, int );
int robot_scan( Robot *, int, int );
void robot_wallscan( Robot *, int, int );
int robot_add_timeout( Robot *, int );
void robot_deleteTimeout( Robot *, int );

#endif
