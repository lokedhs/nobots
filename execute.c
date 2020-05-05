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
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "lang.h"
#include "robot.h"
#include "robotlist.h"
#include "prototypes.h"

static void log_print_stderr( char *, ... );

extern RobotList *robot_list;
extern int world_update_counter;

typedef void LogFunc( char *, ... );
LogFunc *log_print = log_print_stderr;

void init_mainwin( int *, char ** );
void main_loop( void );

int xmain( void )
{
  int x = 100000;
  int y = 100000;
  int a, r;

  c_to_ar( x, y, &a, &r );
  printf( "a:%d r:%d\n", a, r );

  return 0;
}


int main( int argc, char **argv )
{
  robot_list = robotlist_create();

  srandom( time( NULL ) );
  init_mainwin( &argc, argv );
  main_loop();

  return 0;
}

static void log_print_stderr( char *format, ... )
{
  va_list ap;

  va_start( ap, format );
  vfprintf( stderr, format, ap );
  fprintf( stderr, "\n" );
  va_end( ap );
}
