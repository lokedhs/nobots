/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-2002  Elias Martenson (elias-m@algonet.se)
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
#include <math.h>

#include "robot.h"
#include "octree.h"
#include "prototypes.h"
#include "callback_num.h"

#define DEG_TO_RAD(X)	(((double)(X) * (M_PI * 2)) / 360)

void move_robot( Robot *robot )
{
  double new_x, new_y;
  double curr_dir;

  curr_dir = DEG_TO_RAD( (double)robot->current_heading / 10 );
  new_x = robot->x_pos + cos( curr_dir ) * robot->current_speed;
  new_y = robot->y_pos + sin( curr_dir ) * robot->current_speed;

  if( intersect_obj( robot->x_pos, robot->y_pos, new_x, new_y ) ) {
    robot->current_speed = 0;
    robot->destination_speed = 0;
    make_cbvalues_call_callback( robot->program, CB_COLLIDE_WALL );
    return;
  }

  robot->x_pos = new_x;
  robot->y_pos = new_y;
}
