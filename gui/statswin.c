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
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Scale.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include <stdio.h>

#include "statswin.h"
#include "g_proto.h"
#include "robot.h"
#include "robotlist.h"
#include "list.h"
#include "mem.h"

List *statswindows = NULL;

extern Display *display;
extern Screen *screen;
extern Widget toplevel;
extern RobotList *robot_list;

StatsWindow *create_stats_window( int robot_list_index )
{
  StatsWindow *ret = mymalloc( sizeof( StatsWindow ) );
  Widget stats_form;
  Widget title_label;
  char tmp[ 128 ];
  Arg args[ 20 ];
  int ac;
  XmString label_string;
  Atom wm_delete_atom;
  int robot_id;

  robot_id = robot_get_id( robotlist_get_robot_by_index( robot_list,
							 robot_list_index ) );

  /* dialog shell */
  ac = 0;
  XtSetArg( args[ ac ], XmNtitle, "Stats" );			ac++;
  XtSetArg( args[ ac ], XmNallowShellResize, True );		ac++;
  sprintf( tmp, "statsWindow%d", robot_id );
  ret->stats_shell = XtCreatePopupShell( tmp, topLevelShellWidgetClass,
					 toplevel, args, ac);

  /* form */
  ac = 0;
  XtSetArg( args[ ac ], XmNwidth, 200 );			ac++;
  XtSetArg( args[ ac ], XmNheight, 100 );			ac++;
  stats_form = XmCreateForm( ret->stats_shell, "statsForm", args, ac );
  XtManageChild( stats_form );

  /* title label */
  ac = 0;
  sprintf( tmp, "Stats for: %s",
	   robotlist_get_robot( robot_list, robot_id )->robot_name );
  label_string = XmStringCreateLocalized( tmp );
  XtSetArg( args[ ac ], XmNlabelString, label_string );         ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );     ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );    ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_FORM );      ac++;
  title_label = XmCreateLabel( stats_form, "titleLabel", args, ac );
  XmStringFree( label_string );
  XtManageChild( title_label );

  /* shields */
  ret->shields_widget = statswin_create_scale( stats_form, "shields",
					       title_label,
					       "Shields:", 10000 );

  /* speed */
  ret->speed_widget = statswin_create_scale( stats_form, "speed",
					     ret->shields_widget,
					     "Speed:", 500 );

  wm_delete_atom = XmInternAtom( display, "WM_DELETE_WINDOW", False );
  XmAddWMProtocolCallback( ret->stats_shell, wm_delete_atom,
			   statswin_close_callback, (XtPointer)ret );

  XtManageChild( ret->stats_shell );
  XtRealizeWidget( ret->stats_shell );


  /*
   *  Init stats window structure
   */
  ret->robot_id = robot_id;
  ret->last_shields = 0;
  ret->last_speed = 0;
  ret->last_heading = 0;

  if( statswindows == NULL ) {
    statswindows = list_create();
  }
  list_add_to_tail_ptr( statswindows, ret );

  return ret;
}


Widget statswin_create_scale( Widget parent, char *name,
			      Widget label_top,
			      char *title, int scale_max )
{
  Arg args[ 20 ];
  int ac;
  XmString label_string;
  Widget label_widget, scale_widget;
  char str[ 128 ];

  /* label */
  sprintf( str, "%sLabel", name );
  ac = 0;
  label_string = XmStringCreateLocalized( title );
  XtSetArg( args[ ac ], XmNlabelString, label_string );			ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );		ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_WIDGET );		ac++;
  XtSetArg( args[ ac ], XmNtopWidget, label_top );			ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_OPPOSITE_FORM );	ac++;
  XtSetArg( args[ ac ], XmNrightOffset, -100 );				ac++;
  XtSetArg( args[ ac ], XmNheight, 30 );				ac++;
  label_widget = XmCreateLabel( parent, str, args, ac );
  XmStringFree( label_string );
  XtManageChild( label_widget );

  /* scale */
  ac = 0;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_WIDGET );		ac++;
  XtSetArg( args[ ac ], XmNleftWidget, label_widget );			ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_WIDGET );		ac++;
  XtSetArg( args[ ac ], XmNtopWidget, label_top );			ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );		ac++;
  XtSetArg( args[ ac ], XmNheight, 30 );				ac++;
  XtSetArg( args[ ac ], XmNscaleHeight, 16 );				ac++;
  XtSetArg( args[ ac ], XmNorientation, XmHORIZONTAL );			ac++;
  XtSetArg( args[ ac ], XmNshowValue, True );				ac++;
  XtSetArg( args[ ac ], XmNslidingMode, "thermometer" );		ac++;
  XtSetArg( args[ ac ], XmNminimum, 0 );				ac++;
  XtSetArg( args[ ac ], XmNmaximum, scale_max );			ac++;
  XtSetArg( args[ ac ], XmNsensitive, False );				ac++;
  scale_widget = XmCreateScale( parent, name, args, ac );
  XtManageChild( scale_widget );

  return scale_widget;
}

  





void statswin_close_callback( Widget w, XtPointer user_data,
			      XtPointer call_data )
{
  StatsWindow *stats_window = (StatsWindow *)user_data;

  close_statswin( stats_window );
}


void close_statswin( StatsWindow *stats_window )
{
  XtDestroyWidget( stats_window->stats_shell );
  myfree( stats_window );
  list_delete_ptr( statswindows, stats_window );
  printf( "deleted, num: %d\n", list_size( statswindows ) );
}  


void update_stats_window( StatsWindow *stats_window )
{
  Robot *robot = robotlist_get_robot( robot_list, stats_window->robot_id );

  if( robot == NULL ) {
    printf("closing id=%d\n",stats_window->robot_id);
    close_statswin( stats_window );
    return;
  }

  /* UPDATE THE STATS */

  if( stats_window->last_shields != robot_get_shields( robot ) ) {
    XtVaSetValues( stats_window->shields_widget,
		   XmNvalue, robot_get_shields( robot ),
		   NULL );
    stats_window->last_shields = robot_get_shields( robot );
  }

  if( stats_window->last_speed != robot_get_speed( robot ) ) {
    XtVaSetValues( stats_window->speed_widget,
		   XmNvalue, robot_get_speed( robot ),
		   NULL );
    stats_window->last_speed = robot_get_speed( robot );
  }



}
