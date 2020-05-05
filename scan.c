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

#include "config.h"

#include <stdlib.h>

#include "robot.h"
#include "robotlist.h"
#include "map.h"
#include "callback_num.h"
#include "octree.h"
#include "prototypes.h"

extern Branch *world_walls;

extern RobotList *robot_list;

extern int linesearch_ret[];


#define ABS(X)			((X) < 0 ? -(X) : (X))

void scan_for_robots( Robot *robot, int scan_direction, int scan_width )
{
  int x, y;
  Robot *target;
  int target_angle;
  int target_dist;
  int this_x, this_y;
  int enemy_found = 0;

  this_x = robot_get_x_coordinate( robot );
  this_y = robot_get_y_coordinate( robot );

  robotlist_init_walk( robot_list );
  while( (target = robotlist_walk_next( robot_list )) != NULL ) {
    if( robot != target ) {
      x = robot_get_x_coordinate( target );
      y = robot_get_y_coordinate( target );
      c_to_ar( x - this_x, y - this_y, &target_angle, &target_dist );
      if( target_dist <= robot_get_scanner_range( robot ) &&
	  ABS( direction_diff( scan_direction, target_angle ) ) < scan_width / 2 ) {

	if( !intersect_obj( this_x, this_y, x, y ) ) {
	  enemy_found = 1;
	  make_cbvalues_call_callback( robot->program, CB_ENEMY_FOUND, x, y,
				       target_angle, target_dist,
				       robot_get_heading( target ),
				       robot_get_speed( target ),
				       robot_get_id( target ) );
	}

      }
    }
  }

  if( !enemy_found ) {
    make_cbvalues_call_callback( robot->program, CB_ENEMY_NOT_FOUND );
  }
}

static int desti_x, desti_y, dist;

static void check_intersect( int robot_x, int robot_y,
			     int scan_dest_x, int scan_dest_y,
			     int obj_x1, int obj_y1,
			     int obj_x2, int obj_y2 )
{
  int intersect_x, intersect_y;
  int angle, radius;

  if( line_intersect( robot_x, robot_y, scan_dest_x, scan_dest_y,
		      obj_x1, obj_y1, obj_x2, obj_y2,
		      &intersect_x, &intersect_y ) ) {
    c_to_ar( intersect_x - robot_x, intersect_y - robot_y,
	     &angle, &radius );
    if( radius < dist ) {
      desti_x = intersect_x;
      desti_y = intersect_y;
      dist = radius;
    }
  }
}

void scan_for_walls( Robot *robot, int direction, int range )
{
  int scan_dest_x, scan_dest_y;
  int robot_x, robot_y;
  int obj_x1, obj_y1, obj_x2, obj_y2;

  dist = MAP_MAX_X*2;

  robot_x = robot_get_x_coordinate( robot );
  robot_y = robot_get_y_coordinate( robot );
  ar_to_c( direction, range, &scan_dest_x, &scan_dest_y );
  scan_dest_x += robot_x;
  scan_dest_y += robot_y;


  start_linesearch( robot_x, robot_y, scan_dest_x, scan_dest_y );
  while( get_next_line() ) {

    obj_x1 = linesearch_ret[ 0 ];
    obj_y1 = linesearch_ret[ 1 ];
    obj_x2 = linesearch_ret[ 2 ];
    obj_y2 = linesearch_ret[ 3 ];

    check_intersect( robot_x, robot_y, scan_dest_x, scan_dest_y,
		     obj_x1, obj_y1, obj_x2, obj_y2 );
  }

  if( dist == MAP_MAX_X*2 ) {
    /* no wall was found */
    make_cbvalues_call_callback( robot->program, CB_WALL_NOT_FOUND );
  }
  else {
    /* wall was found */
    make_cbvalues_call_callback( robot->program, CB_WALL_FOUND,
				 desti_x, desti_y, dist );
  }
}
