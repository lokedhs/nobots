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

#ifndef ROBOTLIST_H
#define ROBOTLIST_H

#include "robot.h"

typedef struct {
  int num_robots;
  Robot **robots;
  int robot_list_size;
  int walkindex;
} RobotList;

#define robotlist_num_robots(robotlist)		((robotlist)->num_robots)
#define robotlist_get_robot(robotlist,X)	((robotlist)->robots[ X ])

RobotList *robotlist_create( void );
void robotlist_delete( RobotList * );
void robotlist_add_robot( RobotList *, Robot * );
void robotlist_replace_robot( RobotList *, Robot *, Robot * );
void robotlist_delete_robot( RobotList *, Robot * );
Robot *robotlist_get_robot_by_index( RobotList *, int );
void robotlist_init_walk( RobotList * );
Robot *robotlist_walk_next( RobotList * );

#endif
