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
  Circbuffer *waiting_messages;	/* Message waiting to be delivered */
  int listener_channel;		/* Channel the robot is listening to */

  /* timeouts */
  List *timeouts;		/* List of waiting timeouts */

} Robot;

#define RobotGetID(robot)		((robot)->id)
#define RobotGetXCoordinate(robot)	((int)((robot)->x_pos))
#define RobotGetYCoordinate(robot)	((int)((robot)->y_pos))
#define RobotGetHeading(robot)		((robot)->current_heading)
#define RobotGetSpeed(robot)		((robot)->current_speed)
#define RobotGetMaxSpeed(robot)		((robot)->max_speed)
#define RobotGetWeaponSpeed(robot)	((robot)->weapon_speed)
#define RobotSetXCoordinate(robot,X)	((robot)->x_pos = (X))
#define RobotSetYCoordinate(robot,Y)	((robot)->y_pos = (Y))
#define RobotGetShields(robot)		((robot)->current_shields)
#define RobotGetUpdateCounter(robot)	((robot)->update_counter)

#define RobotGetLastScannerDirection(robot)	\
					((robot)->last_scanner_direction)
#define RobotGetLastScannerWidth(robot)	((robot)->last_scanner_width)
#define RobotSetLastScannerWidth(robot,X)	\
					((robot)->last_scanner_width = (X))
#define RobotGetScannerStatus(robot)	((robot)->scanner_recharge_status)

#define RobotGetScannerRange(robot)	((robot)->scanner_range)
#define RobotGetWeaponStatus(robot)	((robot)->weapon_recharge_status)
#define RobotSetWeaponStatus(robot,X)	((robot)->weapon_recharge_status = (X))
#define RobotGetWeaponPower(robot)	((robot)->weapon_power)
#define RobotGetWeaponRange(robot)	((robot)->weapon_range)
#define RobotGetInvulnerable(robot)	((robot)->invulnerable)
#define RobotSetInvulnerable(robot,X)	((robot)->invulnerable = (X))

Robot *RobotCreate( char * );
Robot *RobotCreateWithID( char *, int );
void RobotDelete( Robot * );
void RobotPlaceOnMap( Robot * );
void RobotSetDestinationSpeed( Robot *, int );
void RobotSetDestinationHeading( Robot *, int );
void RobotExecuteInstructions( Robot * );
void RobotUpdate( Robot * );
void RobotTakeDamage( Robot *, int );
int RobotScan( Robot *, int, int );
void RobotWallScan( Robot *, int, int );
int RobotAddTimeout( Robot *, int );
void RobotDeleteTimeout( Robot *, int );

#endif
