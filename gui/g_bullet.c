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

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#ifdef USE_DOUBLE_BUFFER_EXTENSION
#include <X11/extensions/Xdbe.h>
#endif
#ifdef USE_MULTI_BUFFER_EXTENSION
#include <X11/extensions/multibuf.h>
#endif

#include "bullet.h"
#include "list.h"
#include "batwin.h"

extern Display *display;
extern Screen *screen;
extern List *bullet_list;

void draw_bullets( BattleWindow *batwin )
{
  int sw;
  Bullet *bullet;

  list_init_walk( bullet_list );
  while( (bullet = list_walk_next_ptr( bullet_list )) != NULL ) {

    sw = 100 * batwin->xdiv;
    if( sw <= 0 )  sw = 1;

    XDrawArc( display, batwin->out_buffer, batwin->draw_gc,
	      bullet_get_x_coordinate( bullet ) * batwin->xdiv - sw / 2 - batwin->x,
	      bullet_get_y_coordinate( bullet ) * batwin->ydiv - sw / 2 - batwin->y,
	      sw, sw, 0, 360 * 64 );

  }

}
