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

#include <stdio.h>
#include <string.h>

#include "robotlist.h"
#include "robot.h"
#include "mem.h"

void g_delete_robot( int );

RobotList *robotlist_create( void )
{
  RobotList *ret = mymalloc( sizeof( RobotList ) );

  ret->num_robots = 0;
  ret->robots = mymalloc( sizeof( Robot * ) * 10 );
  ret->robot_list_size = 10;

  return ret;
}

void robotlist_delete( RobotList *robotlist )
{
  int c;

  for( c = 0 ; c < robotlist->num_robots ; c++ ) {
    if( robotlist->robots[ c ] != NULL ) {
      robot_delete( robotlist->robots[ c ] );
    }
  }
  myfree( robotlist->robots );
  myfree( robotlist );
}

void robotlist_add_robot( RobotList *robotlist, Robot *robot )
{
  if( robotlist->num_robots >= robotlist->robot_list_size ) {
    robotlist->robot_list_size += 10;
    robotlist->robots = myrealloc( robotlist->robots,
				   (sizeof( Robot * ) *
				    robotlist->robot_list_size) );
  }
  robotlist->robots[ robotlist->num_robots++ ] = robot;
}

void robotlist_replace_robot( RobotList *robotlist, Robot *robot,
			      Robot *new_robot )
{
  int c;
  int n;

  for( c = 0, n = 0 ; c < robotlist->num_robots &&
	 robotlist->robots[ c ] != robot ; c++ ) {
    if( robotlist->robots[ c ] != NULL ) {
      n++;
    }
  }

  if( robotlist->robots[ c ] != robot ) {
    fprintf( stderr, "trying to replace nonexistent robot\n" );
    exit( 1 );
  }

  robotlist->robots[ c ] = new_robot;
}

void robotlist_delete_robot( RobotList *robotlist, Robot *robot )
{
  int c;
  int n;

  for( c = 0, n = 0 ; c < robotlist->num_robots &&
	 robotlist->robots[ c ] != robot ; c++ ) {
    if( robotlist->robots[ c ] != NULL ) {
      n++;
    }
  }

  if( robotlist->robots[ c ] != robot ) {
    fprintf( stderr, "trying to delete nonexistent robot\n" );
    exit( 1 );
  }

  g_delete_robot( n );

  robot_delete( robotlist->robots[ c ] );
  robotlist->robots[ c ] = NULL;
}

Robot *robotlist_get_robot_by_index( RobotList *robotlist, int index )
{
  int c;
  int ic;

  for( ic = 0, c = 0 ; c < robotlist->num_robots ; ic++, c++ ) {
    while( c < robotlist->num_robots && robotlist->robots[ c ] == NULL ) {
      c++;
    }
    if( ic == index ) {
      return robotlist->robots[ c ];
    }
  }

  printf( "nonexistant robot index\n" );

  return NULL;
}    

void robotlist_init_walk( RobotList *robotlist )
{
  robotlist->walkindex = 0;
}

Robot *robotlist_walk_next( RobotList *robotlist )
{
  Robot *robot;

  do {
    if( robotlist->walkindex >= robotlist->num_robots ) {
      return NULL;
    }
    robot = robotlist->robots[ robotlist->walkindex++ ];
  } while( robot == NULL );

  return robot;
}
