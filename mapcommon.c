/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-98  Magnus Ahlden (mga@algonet.se)
 *                           Elias Martenson (elias.martenson@sun.se)
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
#include <string.h>
#include <ctype.h>

#include "list.h"
#include "mapcommon.h"
#include "mem.h"

extern MapObject *gwo_start;
extern List *start_point_list;

#define CHECK_NUM_ARGS(X)	do { if( num_args != (X) ) {	\
					fclose( fp );		\
					return 0; } } while( 0 )

int load_map_common( char *filename )
{
  FILE *fp;
  char str[ 1024 ];
  char *keyword, *keyword_start;
  char *tmp;
  char *args[ 10 ];
  int num_args;

  clear_map();

  if( (fp = fopen( filename, "r" )) == NULL ) {
    return 0;
  }

  while( fgets( str, 1022, fp ) != NULL ) {
    if( str[ strlen( str ) - 1 ] == '\n' ) {
      str[ strlen(str ) - 1 ] = 0;
    }

    /* find start of keyword */
    for( keyword_start = str ; *keyword_start && isspace( *keyword_start ) ;
	 keyword++ );

    /* drop if comment or empty line */
    if( *keyword_start == 0 || *keyword_start == '#' ) {
      continue;
    }

    /* find the arguments */
    keyword = strtok( keyword_start, " \t" );
    num_args = 0;
    while( (tmp = strtok( NULL, " \t" )) != NULL ) {
      args[ num_args++ ] = tmp;
    }

    if( strcmp( keyword, "box" ) == 0 ) {
      CHECK_NUM_ARGS( 4 );
      add_oobj( GWOOT_RECT,
		strtol( args[ 0 ], NULL, 0 ),
		strtol( args[ 1 ], NULL, 0 ),
		strtol( args[ 2 ], NULL, 0 ),
		strtol( args[ 3 ], NULL, 0 ) );
    }
    else if( strcmp( keyword, "line" ) == 0 ) {
      CHECK_NUM_ARGS( 4 );
      add_oobj( GWOOT_LINE,
		strtol( args[ 0 ], NULL, 0 ),
		strtol( args[ 1 ], NULL, 0 ),
		strtol( args[ 2 ], NULL, 0 ),
		strtol( args[ 3 ], NULL, 0 ) );
    }
    else if( strcmp( keyword, "startpoint" ) == 0 ) {
      CHECK_NUM_ARGS( 2 );
      add_startpoint( strtol( args[ 0 ], NULL, 0 ),
		      strtol( args[ 1 ], NULL, 0 ) );
    }
    else {
      fclose( fp );
      return 0;
    }

  }


  fclose( fp );

  return 1;
}



void clear_map( void )
{
  MapObject *tmp;
  MapObject *tmp2;

  if( gwo_start != NULL ) {
    tmp = gwo_start;
    do {
      tmp2 = tmp->next;
      myfree( tmp );
      tmp = tmp2;
    } while( tmp != NULL );
  }
  gwo_start = NULL;

  if( start_point_list != NULL ) {
    while( !ListIsEmpty( start_point_list ) ) {
      StartPoint *tmp;

      tmp = ListGetLastPtr( start_point_list );
      myfree( tmp );
      ListDeleteLast( start_point_list );
    }
    ListDelete( start_point_list );
  }
  start_point_list = ListCreate();
}


#define FIX_COORD(X,MIN,MAX)	((X) < (MIN) ? \
				 (MIN) : ((X) > (MAX) ? (MAX) : (X)))

void add_oobj( int type, int x1, int y1, int x2, int y2 )
{
  MapObject *tmp = mymalloc( sizeof( MapObject ) );
  MapObject *tmp2;

  tmp->o_type = type;
  tmp->next = NULL;

  tmp->valx = FIX_COORD( x1, 0, MAP_MAX_X );
  tmp->valy = FIX_COORD( y1, 0, MAP_MAX_Y );
  tmp->valz = FIX_COORD( x2, 0, MAP_MAX_X );
  tmp->valw = FIX_COORD( y2, 0, MAP_MAX_Y );
  if( gwo_start == NULL ) {
    gwo_start = tmp;
  }
  else {
    for( tmp2 = gwo_start ; tmp2->next != NULL ; tmp2 = tmp2->next );
    tmp2->next = tmp;
  }

}


void add_startpoint( int x, int y )
{
  StartPoint *startpoint;

  if( start_point_list == NULL ) {
    start_point_list = ListCreate();
  }
  startpoint = mymalloc( sizeof( StartPoint ) );
  startpoint->x = FIX_COORD( x, 0, MAP_MAX_X );
  startpoint->y = FIX_COORD( y, 0, MAP_MAX_Y );
  ListAddToTailPtr( start_point_list, startpoint );
}
