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
#include <X11/Intrinsic.h>

#include "robot.h"
#include "robotlist.h"
#include "bullet.h"
#include "map.h"
#include "batwin.h"
#include "statswin.h"
#include "g_proto.h"
#include "list.h"

extern XtAppContext app_context;

extern RobotList *robot_list;
extern BattleWindow *batwins[];
extern List *statswindows;
extern int world_update_counter;

extern XtIntervalId timeout_id;
extern int timeout_active;

void update_loop( XtPointer user_data, XtIntervalId *interval_id )
{
  int c, c2, c3;
  StatsWindow *stats_window;
  Robot *robot;

  for( c3 = 0 ; c3 < 2 ; c3++ ) {
    for( c2 = 0 ; c2 < 50 ; c2++ ) {
      robotlist_init_walk( robot_list );
      while( (robot = robotlist_walk_next( robot_list )) != NULL ){ 
	robot_execute_instructions( robot );
      }
    }

    robotlist_init_walk( robot_list );
    while( (robot = robotlist_walk_next( robot_list )) != NULL ){ 
      robot_update( robot );
    }
    update_bullets();
    world_update_counter++;

    for( c = 0 ; c < MAX_BATWINS ; c++ ) {
      if( batwins[ c ] != NULL ) {
	draw_batwin( batwins[ c ] );
      }
    }
  }

  /*
   *  Here we have to clear the last scanner width of every robot.
   *  we can't do this i the g_redraw_batwin routine because then
   *  only the first window gets its scanner arc painted.
   */
  robotlist_init_walk( robot_list );
  while( (robot = robotlist_walk_next( robot_list )) != NULL ){ 
    robot_set_last_scanner_width( robot, 0 );
  }


  /*
   *  Update the stats windows
   */
  if( statswindows != NULL ) {
    list_init_walk( statswindows );
    while( (stats_window = list_walk_next_ptr( statswindows )) != NULL ) {
      update_stats_window( stats_window );
    }
  }


  timeout_id = XtAppAddTimeOut( app_context, 100, update_loop, NULL );
  timeout_active = 1;

}
