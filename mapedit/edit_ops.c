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

#include <math.h>

#include <Xm/DrawingA.h>

#include "list.h"
#include "mem.h"
#include "mapcommon.h"
#include "mapedit_proto.h"
#include "modes.h"

#define SWAP(X,Y)		do {			\
				  int tmpx = (X);	\
				  (X) = (Y);		\
				  (Y) = tmpx;		\
				} while( 0 )

extern Display *display;
extern Screen *screen;
extern GC draw_gc;
extern GC invert_gc;

extern Window draw_window;

extern double xdiv;
extern double ydiv;
extern int grid_active, grid_visible, grid_scale;
extern int draw_mode;
extern MapObject *gwo_start;
extern List *start_point_list;

static int is_drawing = 0;
static int start_x, start_y;
static int old_x, old_y;

void edit_ops( Widget w, XtPointer user_data, XtPointer call_data )
{
  XmDrawingAreaCallbackStruct *drawing_area_data =
    (XmDrawingAreaCallbackStruct *)call_data;
  XEvent *e = drawing_area_data->event;

  switch( e->type ) {
  case ButtonPress:
    if( e->xbutton.button == 1 ) {
      switch( draw_mode ) {
      case MODE_DELETE:
	delete_object( e->xbutton.x, e->xbutton.y );
	XClearWindow( display, draw_window );
	redraw_map();
	break;

      case MODE_STARTPOINT:
	add_startpoint( e->xbutton.x / xdiv, e->xbutton.y / ydiv );
	XClearWindow( display, draw_window );
	redraw_map();
	break;

      default:
	start_x = e->xbutton.x / xdiv;
	start_y = e->xbutton.y / ydiv;
	align_to_grid( &start_x, &start_y );
	old_x = start_x;
	old_y = start_y;
	align_to_grid( &old_x, &old_y );
	draw_obj( start_x * xdiv, start_y * ydiv,
		  start_x * xdiv, start_y * xdiv );

	is_drawing = 1;
      }
    }
    break;

  case ButtonRelease:
    if( e->xbutton.button == 1 ) {
      if( is_drawing ) {

	align_to_grid( &start_x, &start_y );
	align_to_grid( &old_x, &old_y );

	draw_obj( start_x * xdiv, start_y * ydiv, old_x * xdiv, old_y * ydiv );

	switch( draw_mode ) {
	case MODE_LINE:
	  add_oobj( GWOOT_LINE, start_x, start_y, old_x, old_y );
	  break;

	case MODE_BOX:
	  if( old_x < start_x ) {
	    SWAP( start_x, old_x );
	  }
	  if( old_y < start_y ) {
	    SWAP( start_y, old_y );
	  }
	  add_oobj( GWOOT_RECT, start_x, start_y, old_x, old_y );
	  break;

	}
	redraw_map();
	is_drawing = 0;
      }
    }
    break;
  }
}


void map_edit_motion_callback( Widget w, XtPointer user_data, XEvent *e,
			       Boolean *cont )
{
  *cont = False;

  if( is_drawing ) {
    draw_obj( start_x * xdiv, start_y * ydiv, old_x * xdiv, old_y * ydiv );

    old_x = e->xmotion.x / xdiv;
    old_y = e->xmotion.y / ydiv;
    align_to_grid( &old_x, &old_y );

    draw_obj( start_x * xdiv, start_y * ydiv, old_x * xdiv, old_y * ydiv );

  }
}


void align_to_grid( int *x, int *y )
{
  if( grid_active ) {
    *x = (*x + grid_scale / 2) / grid_scale * grid_scale;
    *y = (*y + grid_scale / 2) / grid_scale * grid_scale;
  }
}


void draw_obj( int x1, int y1, int x2, int y2 )
{
  int new_x = x1, new_y = y1, new_width = x2 - x1, new_height = y2 - y1;

  if( draw_mode == MODE_LINE ) {
    XDrawLine( display, draw_window, invert_gc, x1, y1, x2, y2 );
  }
  else {

    if( x2 < x1 ) {
      new_x -= (x1 - x2);
      new_width = -new_width;
    }

    if( y2 < y1 ) {
      new_y -= (y1 - y2);
      new_height = -new_height;
    }

    XDrawRectangle( display, draw_window, invert_gc, new_x, new_y,
		    new_width, new_height );
  }
}


#define NEAR_LINE 4

void delete_object( int x, int y )
{
  MapObject *w = gwo_start, *last_obj = NULL;
  int to_delete;
  double x1, y1, x2, y2;
  StartPoint *startpoint;
  ListEntry *entry, *tmp;

  while( w != NULL ) {

    to_delete = 0;

    x1 = w->valx * xdiv;
    y1 = w->valy * ydiv;
    x2 = w->valz * xdiv;
    y2 = w->valw * ydiv;

    switch( w->o_type ) {
    case GWOOT_RECT:
      if( x > x1 - NEAR_LINE && x < x2 + NEAR_LINE &&
	  ((y > y1 - NEAR_LINE && y < y1 + NEAR_LINE) ||
	   (y > y2 - NEAR_LINE && y < y2 + NEAR_LINE)) ) {
	to_delete = 1;
      }
      else if( y > y1 - NEAR_LINE && y < y2 + NEAR_LINE &&
	       ((x > x1 - NEAR_LINE && x < x1 + NEAR_LINE) ||
		(x > x2 - NEAR_LINE && x < x2 + NEAR_LINE)) ) {
	to_delete = 1;
      }
      break;

    case GWOOT_LINE:
      if( line_distance( x, y, x1, y1, x2, y2 ) <= NEAR_LINE ) {
	to_delete = 1;
      }
      break;
    }

    if( to_delete ) {
      if( last_obj == NULL ) {
	gwo_start = w->next;
      }
      else {
	last_obj->next = w->next;
      }
    }
    else {
      last_obj = w;
    }
    w = w->next;
  }


  if( start_point_list != NULL ) {
    entry = start_point_list->first;
    while( entry != NULL ) {
      startpoint = entry->val.ptr;
      tmp = entry;
      entry = entry->next;
      if( x > (startpoint->x * xdiv) - NEAR_LINE &&
	  x < (startpoint->x * xdiv) + NEAR_LINE &&
	  y > (startpoint->y * ydiv) - NEAR_LINE &&
	  y < (startpoint->y * ydiv) + NEAR_LINE ) {
	myfree( startpoint );
	list_deleteListEntry( start_point_list, tmp );
      }
    }
  }

}


double line_distance( int px, int py, int x1, int y1, int x2, int y2 )
{
  double length, r, s;

  length = pow( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1), 0.5 );
  r = (((double)(y1 - py) * (y1 - y2)) - ((double)(x1 - px) * (x2 - x1))) /
    (length*length);
  s = (((double)(y1 - py) * (x2 - x1)) - ((double)(x1 - px) * (y2 - y1))) /
    (length*length);

  if( r < 0.0 ) {
    return sqrt( (double)(px-x1)*(px-x1) + (double)(py-y1)*(py-y1) );
  }
  else if( r > 1.0 ) {
    return sqrt( (double)(px-x2)*(px-x2) + (double)(py-y2)*(py-y2) );
  }
  else {
    return fabs( s * length );
  }
}
