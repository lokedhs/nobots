/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1998  Elias Martenson (elias.martenson@sun.se)
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

#include <X11/Xlib.h>
#ifdef USE_DOUBLE_BUFFER_EXTENSION
#include <X11/extensions/Xdbe.h>
#endif
#ifdef USE_MULTI_BUFFER_EXTENSION
#include <X11/extensions/multibuf.h>
#endif
#include <Xm/Xm.h>

#include "map.h"
#include "g_proto.h"


extern MapObject *gwo_start;


void draw_objects( Display *display, Drawable win, GC draw_gc,
		   int x, int y, double xdiv, double ydiv )
{
  MapObject *w;

  for( w = gwo_start ; w ; w = w->next ) {
    switch( w->o_type ) {
    case GWOOT_RECT:
      XDrawRectangle( display, win, draw_gc,
		      w->valx * xdiv - x,
		      w->valy * ydiv - y,
		      (w->valz - w->valx) * xdiv,
		      (w->valw - w->valy) * ydiv );
      break;

    case GWOOT_LINE:
      XDrawLine( display, win, draw_gc,
		 w->valx * xdiv - x,
		 w->valy * ydiv - y,
		 w->valz * xdiv - x,
		 w->valw * ydiv - y );
      break;

    }
  }
}
