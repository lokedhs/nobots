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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include "mem.h"
#include "lang.h"
#include "robot.h"
#include "prototypes.h"

typedef struct {
  char *name;
  int type;
  int offset;
} DefFileEntry;

typedef void LogFunc( char *, ... );
extern LogFunc *log_print;

extern char src_file[];

static char deffile_name[ 1024 ];
static int deffile_line;

#define DF_STRING	0
#define DF_INT		1

#define offsetof(S,E)			((int)(&((S *)0)->E))
#define arraysize(A)			(sizeof(A)/sizeof((A)[0]))

static DefFileEntry def_file_defaults[] = {
  { "name",		DF_STRING,	offsetof( Robot, robot_name ) },
  { "code",		DF_STRING,	offsetof( Robot, code_filename ) },
  { "brake",		DF_INT,		offsetof( Robot, brake_power ) },
  { "accel",		DF_INT,		offsetof( Robot, accel_power ) },
  { "turnspeed",	DF_INT,		offsetof( Robot, turn_speed ) },
  { "maxspeed",		DF_INT,		offsetof( Robot, max_speed ) },
  { "shields",		DF_INT,		offsetof( Robot, max_shields ) },
  { "scannerrecharge",	DF_INT,		offsetof( Robot, scanner_recharge_speed ) },
  { "scannerrange",	DF_INT,		offsetof( Robot, scanner_range ) },
  { "weaponspeed",	DF_INT,		offsetof( Robot, weapon_speed ) },
  { "weaponrecharge",	DF_INT,		offsetof( Robot, weapon_recharge_speed ) },
  { "weaponpower",	DF_INT,		offsetof( Robot, weapon_power ) },
  { "weaponrange",	DF_INT,		offsetof( Robot, weapon_range ) },
};


int parse_def_file( Robot *robot, char *file )
{
  FILE *fp;
  char str[ 1024 ];
  char *keyword_start;
  char *value_start;
  char *w;
  int c;

  deffile_line = 0;
  strcpy( deffile_name, file );

  robot->robot_name = NULL;
  robot->code_filename = NULL;
  robot->brake_power = 0;
  robot->accel_power = 0;
  robot->turn_speed = 0;
  robot->max_speed = 0;
  robot->max_shields = 0;
  robot->scanner_recharge_speed = 0;
  robot->scanner_range = 0;
  robot->weapon_speed = 0;
  robot->weapon_recharge_speed = 0;
  robot->weapon_power = 0;
  robot->weapon_range = 0;

  if( (fp = fopen( file, "r" )) == NULL ) {
    deffile_error( "can't open file: %s", file );
    return 0;
  }

  while( fgets( str, 1022, fp ) != NULL ) {

    deffile_line++;

    if( str[ strlen( str ) - 1 ] == '\n' ) {
      str[ strlen( str ) - 1 ] = 0;
    }

    /* check for end of file keyword */
    if( strcmp( str, "end" ) == 0 ) {
      break;
    }

    /* find start of keyword */
    for( keyword_start = str ; *keyword_start && isspace( *keyword_start ) ;
	 keyword_start++ );

    /* drop if comment or empty line */
    if( *keyword_start == 0 || *keyword_start == '#' ) {
      continue;
    }

    /* find end of keyword */
    /* part one: find : character */
    for( w = keyword_start ; *w && *w != ':' ; w++ );

    /* error if end of string */
    if( *w == 0 ) {
      deffile_error( "syntax error, no value" );
      return 0;
    }

    /* remember position of value */
    value_start = w + 1;

    /* part two: find end of string */
    for( w-- ; w > str && isspace( *w ) ; w-- );

    /* error if no keyword found */
    if( w <= str ) {
      deffile_error( "syntax error, no keyword" );
      return 0;
    }

    /* mark end of keyword */
    w[ 1 ] = 0;

    /* find start of value */
    for( ; *value_start && isspace( *value_start ) ; value_start++ );

    /* error if end of string */
    if( *value_start == 0 ) {
      deffile_error( "syntax error, no value" );
      return 0;
    }

    /* find end of value */
    for( w = value_start + strlen( value_start ) - 1 ;
	 w >= value_start && isspace( *w ) ; w-- );

    /* error if no value found */
    if( w < value_start ) {
      deffile_error( "syntax error, no value found" );
      return 0;
    }

    /* mark end of value */
    w[ 1 ] = 0;


    /* find keyword in defaults list */
    for( c = 0 ; c < arraysize( def_file_defaults ) ; c++ ) {
      if( strcmp( keyword_start, def_file_defaults[ c ].name ) == 0 ) {
	break;
      }
    }
    if( c == arraysize( def_file_defaults ) ) {
      deffile_error( "unknown keyword: %s", keyword_start );
      return 0;
    }

    if( def_file_defaults[ c ].type == DF_STRING ) {
      if( *(char **)(((char *)robot) + def_file_defaults[ c ].offset) != NULL ) {
	deffile_error( "value already set: %s", keyword_start );
	return 0;
      }
      *(char **)(((char *)robot) + def_file_defaults[ c ].offset) =
	mystrdup( value_start );
    }
    else {
      /*      if( *(int *)(((char *)robot) + def_file_defaults[ c ].offset) != 0 ) {
	fprintf( stderr, "value already set: %s\n", keyword_start );
	return 0;
      }*/
      *(int *)(((char *)robot) + def_file_defaults[ c ].offset) =
	strtol( value_start, NULL, 0 );
    }

  }

  if( robot->robot_name == NULL ) {
    robot->robot_name = mystrdup( file );
  }

  if( robot->code_filename != NULL ) {
    robot->program = new_program( robot->code_filename );
  }
  else {
    strcpy( src_file, file );
    robot->program = new_program_fd( fp, deffile_line );
  }

  fclose( fp );

  if( robot->program == NULL ) {
    return 0;
  }


  return 1;
}


void deffile_error( char *format, ... )
{
  va_list ap;
  char str[ 1024 ];

  va_start( ap, format );
  vsprintf( str, format, ap );
  log_print( "def file error in %s, line %d: %s", deffile_name, deffile_line,
	     str );
  va_end( ap );
}
