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

#include "config.h"

#include <stdio.h>

#include "mem.h"
#include "list.h"
#include "prototypes.h"
#include "map.h"
#include "octree.h"

#define MAX(X,Y)		((X) > (Y) ? (X) : (Y))
#define MIN(X,Y)		((X) < (Y) ? (X) : (Y))

extern Branch *world_walls;

extern MapObject *gwo_start;

static MapObject **objs;
static int objs_length;
static int num_objs;


static int octree_check_line( int x1, int y1, int x2, int y2,
			      int box_x1, int box_y1, int box_x2, int box_y2,
			      MapObject *obj )
{
  int intersect_x, intersect_y;
  int found = 0;

  /*
   *  check if the start or ending points of the line
   *  is within the box
   */
  if( (x1 >= box_x1 && x1 <= box_x2) &&
      (y1 >= box_y1 && y1 <= box_y2) ) {
    found = 1;
  }
  if( (x2 >= box_x1 && x2 <= box_x2) &&
      (y2 >= box_y1 && y2 <= box_y2) ) {
    found = 1;
  }

  /*
   *  check if the line crosses any of the box boundaries
   */
  if( found ||
      /* top */
      line_intersect( x1, y1, x2, y2, box_x1, box_y1, box_x2, box_y1,
		      &intersect_x, &intersect_y ) ||
      /* bottom */
      line_intersect( x1, y1, x2, y2, box_x1, box_y2, box_x2, box_y2,
		      &intersect_x, &intersect_y ) ||
      /* left */
      line_intersect( x1, y1, x2, y2, box_x1, box_y1, box_x1, box_y2,
		      &intersect_x, &intersect_y ) ||
      /* right */
      line_intersect( x1, y1, x2, y2, box_x2, box_y1, box_x2, box_y2,
		      &intersect_x, &intersect_y ) ) {
    /*
     *  the line is within or crosses the box
     */
    objs[ num_objs++ ] = obj;
    if( num_objs >= objs_length ) {
      objs_length += 10;
      objs = myrealloc( objs, objs_length * sizeof( MapObject * ) );
    }
    return 1;
  }
  return 0;
}


Branch *make_octree( int level, int x1, int y1, int x2, int y2 )
{
  Branch *ret = mymalloc( sizeof( Branch ) );
  MapObject *w;
  int obj_x1, obj_y1, obj_x2, obj_y2;

  objs = mymalloc( 10 * sizeof( MapObject * ) );
  objs_length = 10;
  num_objs = 0;

  ret->level = level;

  /*
   *  Check if there are any walls in this square
   */
  for( w = gwo_start ; w ; w = w->next ) {
    obj_x1 = w->valx;
    obj_y1 = w->valy;
    obj_x2 = w->valz;
    obj_y2 = w->valw;
    if( w->o_type == GWOOT_RECT ) {
      /* top line */
      if( octree_check_line( obj_x1, obj_y1, obj_x2, obj_y1, x1, y1, x2, y2, w ) )  continue;
      /* bottom line */
      if( octree_check_line( obj_x1, obj_y2, obj_x2, obj_y2, x1, y1, x2, y2, w ) )  continue;
      /* left line */
      if( octree_check_line( obj_x1, obj_y1, obj_x1, obj_y2, x1, y1, x2, y2, w ) )  continue;
      /* right line */
      if( octree_check_line( obj_x2, obj_y1, obj_x2, obj_y2, x1, y1, x2, y2, w ) )  continue;
    }
    else {
      octree_check_line( obj_x1, obj_y1, obj_x2, obj_y2, x1, y1, x2, y2, w );
    }
  }

  objs[ num_objs ] = NULL;

  if( num_objs > 0 && level > 0 ) {
    objs = myrealloc( objs, (num_objs + 1) * sizeof( MapObject * ) );
    ret->objs = objs;
    ret->x1 = x1;
    ret->y1 = y1;
    ret->x2 = x2;
    ret->y2 = y2;
    ret->ul = make_octree( level - 1, x1, y1, x2 / 2, y2 / 2 );
    ret->ur = make_octree( level - 1, x2 / 2, y1, x2, y2 / 2 );
    ret->ll = make_octree( level - 1, x1, y2 / 2, x2 / 2, y2 );
    ret->lr = make_octree( level - 1, x2 / 2, y2 / 2, x2, y2 );
    return ret;
  }
  else {
    if( num_objs == 0 ) {
      myfree( objs );
      myfree( ret );
      return NULL;
    }
    else {
      ret->ul = NULL;
      ret->ur = NULL;
      ret->ll = NULL;
      ret->lr = NULL;
      ret->objs = objs;
      return ret;
    }
  }
}

static int objlist_ptr;
static MapObject **objlist[ 8192 ];

MapObject ***octree_find_world_objs( int x1, int y1, int x2, int y2 )
{
  int nx1, ny1, nx2, ny2;

  nx1 = MIN( x1, x2 );
  ny1 = MIN( y1, y1 );
  nx2 = MAX( x1, x2 );
  ny2 = MAX( y1, y2 );

  objlist_ptr = 0;
  octree_find_objs( world_walls, nx1, ny1, nx2, ny2 );
  objlist[ objlist_ptr ] = NULL;

  return &objlist[ 0 ];
}

/*
 *  This function requires x1,y1 to be the top left corner
 *  and x2,y2 to be the lower right
 */
void octree_find_objs( Branch *subbranch,
		       int x1, int y1, int x2, int y2 )
{
  /*
   *  if this is the bottom level, return the whole list
   */
  if( subbranch->level == 0 ) {
    if( subbranch->objs != NULL ) {
      objlist[ objlist_ptr++ ] = subbranch->objs;
    }
    return;
  }

  /*
   *  if no part of the box is covered, return NULL
   */
  if( x2 < subbranch->x1 ||
      x1 > subbranch->x2 ||
      y2 < subbranch->y1 ||
      y1 > subbranch->y2 ) {
    return;
  }

  /*
   *  if the box totally covers the branch size,
   *  return all of the objects
   */
  if( x1 <= subbranch->x1 &&
      y1 <= subbranch->y1 &&
      x2 >= subbranch->x2 &&
      y2 >= subbranch->y2 ) {
    objlist[ objlist_ptr++ ] = subbranch->objs;
    return;
  }

  /*
   *  check each of the subbranch parts
   */
  if( subbranch->ul != NULL ) {
    octree_find_objs( subbranch->ul, x1, y1, x2, y2 );
  }
  if( subbranch->ur != NULL ) {
    octree_find_objs( subbranch->ur, x1, y1, x2, y2 );
  }
  if( subbranch->ll != NULL ) {
    octree_find_objs( subbranch->ll, x1, y1, x2, y2 );
  }
  if( subbranch->lr != NULL ) {
    octree_find_objs( subbranch->lr, x1, y1, x2, y2 );
  }
}

static MapObject **linesearch_objlist_w;
static MapObject ***linesearch_objlist;
static int linesearch_curr_line;
int linesearch_ret[ 4 ];

static void linesearch_get_next_obj( void )
{
  if( linesearch_curr_line == 0 ) {
    do {
      linesearch_objlist_w++;
      if( ! *linesearch_objlist_w ) {
	linesearch_objlist++;
	linesearch_objlist_w = *linesearch_objlist;
      }
    } while( *linesearch_objlist && ! *linesearch_objlist_w );
  }
}

void start_linesearch( int x1, int y1, int x2, int y2 )
{
  linesearch_objlist = octree_find_world_objs( x1, y1, x2, y2 );
  linesearch_objlist_w = *linesearch_objlist;
  linesearch_curr_line = 0;
  if( linesearch_objlist == NULL ) {
    linesearch_get_next_obj();
  }
}

int get_next_line( void )
{
  if( ! *linesearch_objlist ) {
    return 0;
  }

  if( linesearch_objlist_w == NULL ) {
    printf( "entry one is null\n" );
  }

  if( *linesearch_objlist_w == NULL ) {
    printf( "entry: is null\n" );
  }

  if( (*linesearch_objlist_w)->o_type == GWOOT_RECT ) {
    linesearch_curr_line++;
    switch( linesearch_curr_line ) {
    case 1:
      /* top */
      linesearch_ret[ 0 ] = (*linesearch_objlist_w)->valx;
      linesearch_ret[ 1 ] = (*linesearch_objlist_w)->valy;
      linesearch_ret[ 2 ] = (*linesearch_objlist_w)->valz;
      linesearch_ret[ 3 ] = (*linesearch_objlist_w)->valy;
      break;
    case 2:
      /* bottom */
      linesearch_ret[ 0 ] = (*linesearch_objlist_w)->valx;
      linesearch_ret[ 1 ] = (*linesearch_objlist_w)->valw;
      linesearch_ret[ 2 ] = (*linesearch_objlist_w)->valz;
      linesearch_ret[ 3 ] = (*linesearch_objlist_w)->valw;
      break;
    case 3:
      /* left */
      linesearch_ret[ 0 ] = (*linesearch_objlist_w)->valx;
      linesearch_ret[ 1 ] = (*linesearch_objlist_w)->valy;
      linesearch_ret[ 2 ] = (*linesearch_objlist_w)->valx;
      linesearch_ret[ 3 ] = (*linesearch_objlist_w)->valw;
      break;
    case 4:
      /* right */
      linesearch_ret[ 0 ] = (*linesearch_objlist_w)->valz;
      linesearch_ret[ 1 ] = (*linesearch_objlist_w)->valy;
      linesearch_ret[ 2 ] = (*linesearch_objlist_w)->valz;
      linesearch_ret[ 3 ] = (*linesearch_objlist_w)->valw;
      linesearch_curr_line = 0;
      break;
    default:
      linesearch_curr_line = 0;
    }

  }
  else {
    linesearch_ret[ 0 ] = (*linesearch_objlist_w)->valx;
    linesearch_ret[ 1 ] = (*linesearch_objlist_w)->valy;
    linesearch_ret[ 2 ] = (*linesearch_objlist_w)->valz;
    linesearch_ret[ 3 ] = (*linesearch_objlist_w)->valw;
    linesearch_curr_line = 0;
  }

  linesearch_get_next_obj();

  if( linesearch_objlist_w && *linesearch_objlist_w == NULL ) {
    printf( "w is null\n" );
    if( *linesearch_objlist == NULL ) {
      printf( " lo is null\n" );
    }
    else {
      printf( "  lo is ! null\n" );
    }
  }

  return 1;
}

int intersect_obj( int x1, int y1, int x2, int y2 )
{
  int obj_x1, obj_y1, obj_x2, obj_y2;

  start_linesearch( x1, y1, x2, y2 );
  while( get_next_line() ) {
    obj_x1 = linesearch_ret[ 0 ];
    obj_y1 = linesearch_ret[ 1 ];
    obj_x2 = linesearch_ret[ 2 ];
    obj_y2 = linesearch_ret[ 3 ];

    if( line_intersect( x1, y1, x2, y2,
			obj_x1, obj_y1, obj_x2, obj_y2,
			NULL, NULL ) ) {
      return 1;
    }
  }
  return 0;
}
