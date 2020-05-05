/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1998  Elias Martenson (lokedhs@gmail.com)
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
#include <Xm/ScrolledW.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrollBar.h>
#include <Xm/Scale.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/ScrollBar.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include <stdio.h>
#include <math.h>

#include "batwin.h"
#include "robot.h"
#include "robotlist.h"
#include "map.h"
#include "g_proto.h"
#include "mem.h"

#define MIN(X,Y)		((X) < (Y) ? (X) : (Y))

extern Display *display;
extern Screen *screen;
extern Widget toplevel;

extern MapObject *gwo_start;

BattleWindow *batwins[ MAX_BATWINS ];
int multi_buffer_method = MULTIBUFFER_METHOD_NONE;
extern RobotList *robot_list;
extern int in_progress;

extern int timeout_active;

void init_batwins( void )
{
  int c;

  for( c = 0 ; c < MAX_BATWINS ; c++ ) {
    batwins[ c ] = NULL;
  }
}

void create_batwin( int follow )
{
  Widget batwin_form, zoom_label, zoom_scale;
  Widget scrolled_win;
  Widget drawing_area;
  XmString label_string;
  BattleWindow *ret;
  int battle_window_id;
  char tmp[ 128 ];
  char tmp2[ 128 ];
  Arg args[ 20 ];
  int ac;
  XGCValues values;
  Atom wm_delete_atom;
  XColor col;

  if( !in_progress ) {
    return;
  }

  for( battle_window_id = 0 ;
       batwins[ battle_window_id ] != NULL ;
       battle_window_id++ );


  ret = mymalloc( sizeof( BattleWindow ) );

  batwins[ battle_window_id ] = ret;


  /* dialog shell */
  ac = 0;
  if( follow == -1 ) {
    strcpy( tmp2, "Battle window" );
  }
  else {
    sprintf( tmp2, "Battle window - Following %s",
	     robotlist_get_robot_by_index( robot_list, follow )->robot_name );
  }
  XtSetArg( args[ ac ], XmNtitle, tmp2 );			ac++;
  XtSetArg( args[ ac ], XmNallowShellResize, True );		ac++;
  sprintf( tmp, "battleWindow%d", battle_window_id );
  ret->batwin_shell = XtCreatePopupShell( tmp, topLevelShellWidgetClass,
					  toplevel, args, ac );

  /* form */
  ac = 0;
  XtSetArg( args[ ac ], XmNwidth, 500 );			ac++;
  XtSetArg( args[ ac ], XmNheight, 500 );			ac++;
  batwin_form = XmCreateForm( ret->batwin_shell, "battleWindowForm",
				args, ac );
  XtManageChild( batwin_form );

  /* zoom label */
  ac = 0;
  label_string = XmStringCreateLocalized( "Zoom:" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNbottomAttachment, XmATTACH_FORM );	ac++;
  zoom_label = XmCreateLabel( batwin_form, "zoomLabel", args, ac );
  XtManageChild( zoom_label );
  XmStringFree( label_string );

  /* zoom scale */
  ac = 0;
  XtSetArg( args[ ac ], XmNorientation, XmHORIZONTAL );		ac++;
  XtSetArg( args[ ac ], XmNminimum, 600 );			ac++;
  XtSetArg( args[ ac ], XmNmaximum, 4000 );			ac++;
  XtSetArg( args[ ac ], XmNprocessingDirection, XmMAX_ON_RIGHT ); ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_WIDGET );	ac++;
  XtSetArg( args[ ac ], XmNleftWidget, zoom_label );		ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNbottomAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNheight, 20 );			ac++;
  zoom_scale = XmCreateScale( batwin_form, "zoomScale", args, ac );
  XtManageChild( zoom_scale );
  XtAddCallback( zoom_scale, XmNvalueChangedCallback, zoom_changed_callback,
		 ret );
  XtAddCallback( zoom_scale, XmNdragCallback, zoom_changed_callback, ret );

  /* scrolled window */
  ac = 0;
  XtSetArg( args[ ac ], XmNscrollingPolicy, XmAPPLICATION_DEFINED ); ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNbottomAttachment, XmATTACH_WIDGET );	ac++;
  XtSetArg( args[ ac ], XmNbottomWidget, zoom_scale );		ac++;
  scrolled_win = XmCreateScrolledWindow( batwin_form, "gameAreaScrolledWin",
					 args, ac );
  XtManageChild( scrolled_win );

  /* horizontal scroll bar */
  ac = 0;
  XtSetArg( args[ ac ], XmNorientation, XmHORIZONTAL );		ac++;
  XtSetArg( args[ ac ], XmNminimum, 0 );			ac++;
  XtSetArg( args[ ac ], XmNmaximum, 599 );			ac++;
  ret->horiz_scrollbar = XmCreateScrollBar( scrolled_win, "horizontalScrollBar",
					    args, ac );
  XtManageChild( ret->horiz_scrollbar );
  XtAddCallback( ret->horiz_scrollbar, XmNdragCallback,
		 horiz_scrollbar_changed_callback, ret );
  XtAddCallback( ret->horiz_scrollbar, XmNdecrementCallback,
		 horiz_scrollbar_changed_callback, ret );
  XtAddCallback( ret->horiz_scrollbar, XmNincrementCallback,
		 horiz_scrollbar_changed_callback, ret );
  XtAddCallback( ret->horiz_scrollbar, XmNpageDecrementCallback,
		 horiz_scrollbar_changed_callback, ret );
  XtAddCallback( ret->horiz_scrollbar, XmNpageIncrementCallback,
		 horiz_scrollbar_changed_callback, ret );

  /* vertical scroll bar */
  ac = 0;
  XtSetArg( args[ ac ], XmNorientation, XmVERTICAL );		ac++;
  XtSetArg( args[ ac ], XmNminimum, 0 );			ac++;
  XtSetArg( args[ ac ], XmNmaximum, 599 );			ac++;
  ret->vert_scrollbar = XmCreateScrollBar( scrolled_win, "verticalScrollBar",
					   args, ac );
  XtManageChild( ret->vert_scrollbar );
  XtAddCallback( ret->vert_scrollbar, XmNdragCallback,
		 vert_scrollbar_changed_callback, ret );
  XtAddCallback( ret->vert_scrollbar, XmNincrementCallback,
		 vert_scrollbar_changed_callback, ret );
  XtAddCallback( ret->vert_scrollbar, XmNdecrementCallback,
		 vert_scrollbar_changed_callback, ret );
  XtAddCallback( ret->vert_scrollbar, XmNpageIncrementCallback,
		 vert_scrollbar_changed_callback, ret );
  XtAddCallback( ret->vert_scrollbar, XmNpageDecrementCallback,
		 vert_scrollbar_changed_callback, ret );

  /* drawing area */
  ac = 0;
  XtSetArg( args[ ac ], XmNwidth, 600 );			ac++;
  XtSetArg( args[ ac ], XmNheight, 600 );			ac++;
  XtSetArg( args[ ac ], XmNbackground, BlackPixelOfScreen( screen ) ); ac++;
  drawing_area = XmCreateDrawingArea( scrolled_win, "drawingArea", args, ac );
  XtManageChild( drawing_area );
  XtAddCallback( drawing_area, XmNexposeCallback, batwin_redraw_callback, ret );
  XtAddCallback( drawing_area, XmNinputCallback, batwin_button_callback, ret );


  /* Set up the windows size changed callback */
  XtAddEventHandler( drawing_area, StructureNotifyMask, False,
		     batwin_size_changed_callback, ret );


  /* Set up the close window callback */
  wm_delete_atom = XmInternAtom( display, "WM_DELETE_WINDOW", False );
  XmAddWMProtocolCallback( ret->batwin_shell, wm_delete_atom,
			   batwin_close_callback, (XtPointer)ret );

  XtManageChild( ret->batwin_shell );



  /*
   *  Set up the batwin struct
   */
  ret->drawarea = drawing_area;
  ret->scrolled_win = scrolled_win;
  XtVaGetValues( scrolled_win,
		 /*		 XmNhorizontalScrollBar, &ret->horiz_scrollbar,
		 XmNverticalScrollBar, &ret->vert_scrollbar,
		 XmNclipWindow, &ret->clip_window,*/
		 XmNworkWindow, &ret->work_window,
		 NULL );
  ret->x = 0;
  ret->y = 0;
  ret->width = 600;		/* HARDCODED */
  ret->height = 600;		/* HARDCODED */
  ret->visible_width = 500;
  ret->visible_height = 500;
  ret->xdiv = (double)ret->width / MAP_MAX_X;
  ret->ydiv = (double)ret->height / MAP_MAX_Y;
  if( follow == -1 ) {
    ret->follow = -1;
  }
  else {
    ret->follow = robot_get_id( robotlist_get_robot_by_index( robot_list, follow ) );
  }

  /* draw GC */
  values.foreground = WhitePixelOfScreen( screen );
  values.background = BlackPixelOfScreen( screen );
  ret->draw_gc = XCreateGC( display, XtWindow( ret->drawarea ),
			    GCForeground | GCBackground, &values );

  /* scanner GC */
  col.red = 0x0000;
  col.green = 0x0000;
  col.blue = 0x4000;
  if( !XAllocColor( display, DefaultColormapOfScreen( screen ), &col ) ) {
    printf( "Can't allocate colour\n" );
  }
  values.foreground = col.pixel;
  values.background = BlackPixelOfScreen( screen );
  ret->scanner_gc = XCreateGC( display, XtWindow( ret->drawarea ),
			       GCForeground | GCBackground, &values );


  /* SET UP DOUBLE BUFFERING HERE!!!! */
#ifdef USE_DOUBLE_BUFFER_EXTENSION
  if( multi_buffer_method == MULTIBUFFER_METHOD_DBLBUF ) {
    ret->dbe_back_buffer =
      XdbeAllocateBackBufferName( display,
				  XtWindow( ret->drawarea ),
				  XdbeBackground );
    ret->out_buffer = ret->dbe_back_buffer;
  }
#endif

#ifdef USE_MULTI_BUFFER_EXTENSION
  if( multi_buffer_method == MULTIBUFFER_METHOD_MBUF ) {
    XmbufCreateBuffers( display,
			XtWindow( ret->drawarea ), 2,
			MultibufferUpdateActionBackground,
			MultibufferUpdateHintFrequent,
			ret->buffers );
    ret->current_buffer = 0;
    ret->out_buffer = ret->buffers[ 1 ];
  }
#endif

  if( multi_buffer_method == MULTIBUFFER_METHOD_NONE ) {
    ret->out_buffer = XtWindow( drawing_area );
  }

  ret->battle_window_id = battle_window_id;

}


void close_batwin( BattleWindow *batwin )
{
  int battle_window_id = batwin->battle_window_id;

  XtDestroyWidget( batwin->batwin_shell );
  XFreeGC( display, batwin->draw_gc );
  XFreeGC( display, batwin->scanner_gc );
  myfree( batwins[ battle_window_id ] );
  batwins[ battle_window_id ] = NULL;
}


void batwin_size_changed_callback( Widget w, XtPointer user_data, XEvent *e,
				   Boolean *cont )
{
  Dimension width, height;
  BattleWindow *batwin = (BattleWindow *)user_data;

  *cont = True;

  XtVaGetValues( batwin->work_window,
		 XmNwidth, &width,
		 XmNheight, &height,
		 NULL );

  batwin->visible_width = width;
  batwin->visible_height = height;

  XtVaSetValues( batwin->drawarea,
		 XmNwidth, batwin->visible_width,
		 XmNheight, batwin->visible_height,
		 NULL );

  batwin_set_scrollbars( batwin );
}


void horiz_scrollbar_changed_callback( Widget w, XtPointer user_data,
				       XtPointer call_data )
{
  XmScrollBarCallbackStruct *scrollbar_data =
    (XmScrollBarCallbackStruct *)call_data;
  BattleWindow *batwin = (BattleWindow *)user_data;

  batwin->x = scrollbar_data->value;

  if( !timeout_active ) {
    draw_batwin( batwin );
  }
}


void vert_scrollbar_changed_callback( Widget w, XtPointer user_data,
				       XtPointer call_data )
{
  XmScrollBarCallbackStruct *scrollbar_data =
    (XmScrollBarCallbackStruct *)call_data;
  BattleWindow *batwin = (BattleWindow *)user_data;

  batwin->y = scrollbar_data->value;

  if( !timeout_active ) {
    draw_batwin( batwin );
  }
}


void batwin_close_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  BattleWindow *batwin = (BattleWindow *)user_data;

  close_batwin( batwin );
}


void batwin_redraw_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  BattleWindow *batwin = (BattleWindow *)user_data;

  draw_batwin( batwin );
}


void batwin_button_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  XmDrawingAreaCallbackStruct *drawing_area_data =
    (XmDrawingAreaCallbackStruct *)call_data;
  XEvent *e = drawing_area_data->event;
  BattleWindow *batwin = (BattleWindow *)user_data;

  if( e->type == ButtonPress ) {
    center_batwin( batwin, e->xbutton.x + batwin->x, e->xbutton.y + batwin->y );
  }

  if( !timeout_active ) {
    draw_batwin( batwin );
  }
}


void zoom_changed_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  XmScaleCallbackStruct *scale_data = (XmScaleCallbackStruct *)call_data;
  BattleWindow *batwin = (BattleWindow *)user_data;

  int xvalue, yvalue;

  xvalue = batwin->x + batwin->visible_width / 2;
  xvalue /= batwin->xdiv;
  yvalue = batwin->y + batwin->visible_height / 2;
  yvalue /= batwin->ydiv;

  batwin->width = scale_data->value;
  batwin->height = scale_data->value;
  batwin->xdiv = (double)batwin->width / MAP_MAX_X;
  batwin->ydiv = (double)batwin->height / MAP_MAX_Y;

  center_batwin( batwin, xvalue * batwin->xdiv, yvalue * batwin->ydiv );

  if( !timeout_active ) {
    draw_batwin( batwin );
  }

}


void draw_batwin( BattleWindow *batwin )
{
  int x, y;
  int heading;
#ifdef USE_DOUBLE_BUFFER_EXTENSION
  XdbeSwapInfo sinfo;
#endif
  Robot *robot;



  if( batwin->follow != -1 ) {
    if( robotlist_get_robot( robot_list, batwin->follow ) == NULL ) {
      close_batwin( batwin );
      return;
    }

    center_batwin( batwin,
		   (robot_get_x_coordinate( robotlist_get_robot( robot_list,
								 batwin->follow ) ) *
		    batwin->xdiv),
		   (robot_get_y_coordinate( robotlist_get_robot( robot_list,
								 batwin->follow ) ) *
		    batwin->ydiv) );
  }




  if( multi_buffer_method == MULTIBUFFER_METHOD_NONE ) {
    XClearWindow( display, XtWindow( batwin->drawarea ) );
  }


  /*
   *  Draw the background dots
   */
  for( y = 0 ; y < MAP_MAX_Y ; y += 5000 ) {
    for( x = 0 ; x < MAP_MAX_X ; x += 5000 ) {
      XDrawPoint( display, batwin->out_buffer, batwin->draw_gc,
		  x * batwin->xdiv - batwin->x, y * batwin->ydiv - batwin->y );
    }
  }


  /*
   *  Draw the scanner
   */
  robotlist_init_walk( robot_list );
  while( (robot = robotlist_walk_next( robot_list )) != NULL ) {
    int dir1;
    int xrange, yrange;

    if( robot_get_last_scanner_width( robot ) > 0 ) {
      dir1 = (robot_get_last_scanner_direction( robot ) -
	      robot_get_last_scanner_width( robot ) / 2 ) % 3600;
      xrange = robot_get_scanner_range( robot ) * batwin->xdiv;
      yrange = robot_get_scanner_range( robot ) * batwin->ydiv;

      XFillArc( display, batwin->out_buffer, batwin->scanner_gc,
		robot_get_x_coordinate( robot ) * batwin->xdiv - xrange - batwin->x,
		robot_get_y_coordinate( robot ) * batwin->ydiv - yrange - batwin->y,
		xrange * 2, yrange * 2,
		(360 * 64) - (dir1 * 64 / 10),
		-(robot_get_last_scanner_width( robot ) * 64 / 10) );
    }
  }


  /*
   *  Draw the objects
   */
  draw_objects( display, batwin->out_buffer, batwin->draw_gc,
		batwin->x, batwin->y, batwin->xdiv, batwin->ydiv );


  /*
   *  Draw the robots
   */
  robotlist_init_walk( robot_list );
  while( (robot = robotlist_walk_next( robot_list )) != NULL ) {
    x = robot_get_x_coordinate( robot );
    y = robot_get_y_coordinate( robot );
    heading = robot_get_heading( robot );
    draw_rot_line( batwin, x, y, heading, 250, 0, -250, 100 );
    draw_rot_line( batwin, x, y, heading, 250, 0, -250, -100 );
    draw_rot_line( batwin, x, y, heading, -250, 100, -250, -100 );
  }


  /*
   *  Draw the bullets
   */
  draw_bullets( batwin );


  /*
   *  Switch buffers
   */

#ifdef USE_DOUBLE_BUFFER_EXTENSION
  if( multi_buffer_method == MULTIBUFFER_METHOD_DBLBUF ) {
    sinfo.swap_window = XtWindow( batwin->drawarea );
    sinfo.swap_action = XdbeBackground;
    XdbeSwapBuffers( display, &sinfo, 1 );
  }
#endif

#ifdef USE_MULTI_BUFFER_EXTENSION
  if( multi_buffer_method == MULTIBUFFER_METHOD_MBUF ) {
    XmbufDisplayBuffers( display, 1, &batwin->buffers[ batwin->current_buffer ],
			 0, 10 );

    if( batwin->current_buffer == 0 ) {
      batwin->current_buffer = 1;
    }
    else {
      batwin->current_buffer = 0;
    }

    batwin->out_buffer = batwin->buffers[ batwin->current_buffer ];
  }
#endif


}


void center_batwin( BattleWindow *batwin, int x, int y )
{
  batwin->x = x - batwin->visible_width / 2;
  if( batwin->x >= batwin->width - batwin->visible_width ) {
    batwin->x = batwin->width - batwin->visible_width;
  }
  if( batwin->x < 0 ) {
    batwin->x = 0;
  }

  batwin->y = y - batwin->visible_height / 2;
  if( batwin->y >= batwin->height - batwin->visible_height ) {
    batwin->y = batwin->height - batwin->visible_height;
  }
  if( batwin->y < 0 ) {
    batwin->y = 0;
  }

  batwin_set_scrollbars( batwin );
}


void batwin_set_scrollbars( BattleWindow *batwin )
{
  int nwidth, nheight;
  int nx, ny;

  nwidth = MIN( batwin->visible_width, batwin->width );
  nheight = MIN( batwin->visible_height, batwin->height );

  nx = MIN( batwin->x, batwin->width - nwidth );
  ny = MIN( batwin->y, batwin->height - nheight );

  XtVaSetValues( batwin->horiz_scrollbar,
		 XmNmaximum, batwin->width,
		 XmNsliderSize, nwidth,
		 XmNpageIncrement, nwidth / 2,
		 XmNvalue, nx,
		 NULL );

  XtVaSetValues( batwin->vert_scrollbar,
		 XmNmaximum, batwin->height,
		 XmNsliderSize, nheight,
		 XmNpageIncrement, nheight / 2,
		 XmNvalue, ny,
		 NULL );

}


void draw_rot_line( BattleWindow *batwin, int x, int y, int degree,
		    int x1, int y1, int x2, int y2 )
{
  int sx1, sy1, sx2, sy2;

  rotate_point( batwin, x, y, x1, y1, degree, &sx1, &sy1 );
  rotate_point( batwin, x, y, x2, y2, degree, &sx2, &sy2 );
  XDrawLine( display, batwin->out_buffer, batwin->draw_gc,
	     sx1 - batwin->x, sy1 - batwin->y,
	     sx2 - batwin->x, sy2 - batwin->y );
}


void rotate_point( BattleWindow *batwin, int base_x, int base_y, int x, int y,
		   int degree, int *out_x, int *out_y )
{
  double degval = degree * M_PI * 2 / 3600;
  double radius, angle;

  radius = sqrt( x*x + y*y );
  angle = acos( x / radius );
  if( y < 0 )  angle = M_PI * 2 - angle;
  angle += degval;
  if( angle >= M_PI * 2 )  angle -= M_PI * 2;

  *out_x = cos( angle ) * radius;
  *out_y = sin( angle ) * radius;

  *out_x += base_x;
  *out_x *= batwin->xdiv;

  *out_y += base_y;
  *out_y *= batwin->ydiv;
}
