/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-98  Elias Martenson (elias-m@algonet.se)
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

#include "prototypes.h"
#include "robotlist.h"
#include "robot.h"

RobotList *robot_list;
List *bullet_list = NULL;
int world_update_counter = 0;

static void init_simulation( void );

int in_progress = 0;

int init_simulation_file_map( char *filename )
{
  if( !load_map( filename ) ) {
    return 0;
  }
  init_simulation();
  return 1;
}

void init_simulation_random_map( int map_objects )
{
  init_simulation();
}

static void init_simulation( void )
{
  Robot *robot;

  robotlist_init_walk( robot_list );
  while( (robot = robotlist_walk_next( robot_list )) != NULL ) {
    robot_place_on_map( robot );
  }

  bullet_list = list_create();

  in_progress = 1;
}
