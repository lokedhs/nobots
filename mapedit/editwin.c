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

#include <Xm/ScrolledW.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/DrawingA.h>
#include <Xm/FileSB.h>
#include <Xm/TextF.h>
#include <Xm/RepType.h>

#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "mapcommon.h"
#include "modes.h"
#include "mapedit_proto.h"


XtAppContext app_context;

String fallback_res[] = {
  "*tearOffModel: TEAR_OFF_ENABLED",
  NULL
};

Display *display;
Screen *screen;
Window draw_window;

MapObject *gwo_start = NULL;
List *start_point_list = NULL;

Widget toplevel;
Widget drawing_area;
Widget load_fileselect;
Widget save_fileselect;

GC draw_gc;
GC invert_gc;
GC grid_gc;

double xdiv = (double)600 / MAP_MAX_X;
double ydiv = (double)600 / MAP_MAX_Y;
char map_filename[ 1024 ] = "";
int grid_active = 0, grid_visible = 0, grid_scale = 5000;
int draw_mode = MODE_LINE;

extern Widget grid_options_dialog;
extern Widget mode_options_form;

int main( int argc, char **argv )
{
  Widget main_window;
  Widget menu_bar, file_cascade, file_pulldown;
  Widget open_menu_button, save_menu_button, save_as_menu_button;
  Widget quit_menu_button;
  Widget file_menu_separator;
  Widget options_cascade, options_pulldown;
  Widget grid_options_menu_button;
  Widget draw_mode_button;
  Arg args[ 20 ];
  int ac;
  XmString label_string;

  toplevel = XtAppInitialize( &app_context, "Mapedit", NULL, 0, &argc, argv,
			      fallback_res, NULL, 0 );

  XmRepTypeInstallTearOffModelConverter();

  /* main window */
  ac = 0;
  XtSetArg( args[ ac ], XmNwidth, 400 );			ac++;
  XtSetArg( args[ ac ], XmNheight, 400 );			ac++;
  XtSetArg( args[ ac ], XmNscrollingPolicy, XmAUTOMATIC );	ac++;
  main_window = XmCreateMainWindow( toplevel, "mainWindow", args, ac );
  XtManageChild( main_window );

  /* menu bar */
  ac = 0;
  menu_bar = XmCreateMenuBar( main_window, "menuBar", args, ac );
  XtManageChild( menu_bar );

  /* file cascade */
  ac = 0;
  label_string = XmStringCreateLocalized( "File" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  file_cascade = XmCreateCascadeButton( menu_bar, "fileCascade", args, ac );
  XmStringFree( label_string );
  XtManageChild( file_cascade );

  /* file pulldown */
  ac = 0;
  file_pulldown = XmCreatePulldownMenu( file_cascade, "filePulldown", args, ac );
  /*  XtManageChild( file_pulldown );*/

  /* open menu button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Open" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  open_menu_button = XmCreatePushButton( file_pulldown, "open", args, ac );
  XmStringFree( label_string );
  XtManageChild( open_menu_button );
  XtAddCallback( open_menu_button, XmNactivateCallback,
		 load_map_pressed_callback, NULL );

  /* save menu button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Save" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  save_menu_button = XmCreatePushButton( file_pulldown, "save", args, ac );
  XmStringFree( label_string );
  XtManageChild( save_menu_button );
  XtAddCallback( save_menu_button, XmNactivateCallback,
		 save_map_pressed_callback, NULL );

  /* save as menu button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Save as" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  save_as_menu_button = XmCreatePushButton( file_pulldown, "saveAs", args, ac );
  XmStringFree( label_string );
  XtManageChild( save_as_menu_button );
  XtAddCallback( save_as_menu_button, XmNactivateCallback,
		 save_as_map_pressed_callback, NULL );

  /* separator */
  ac = 0;
  file_menu_separator = XmCreateSeparator( file_pulldown, "separator1",
					   args, ac );
  XtManageChild( file_menu_separator );

  /* quit menu button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Quit" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  quit_menu_button = XmCreatePushButton( file_pulldown, "quit", args, ac );
  XmStringFree( label_string );
  XtManageChild( quit_menu_button );
  XtAddCallback( quit_menu_button, XmNactivateCallback, quit_pressed_callback,
		 NULL );

  /* attach the file menu to the cascade button */
  XtVaSetValues( file_cascade,
		 XmNsubMenuId, file_pulldown,
		 NULL );

  /* options cascade */
  ac = 0;
  label_string = XmStringCreateLocalized( "Options" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  options_cascade = XmCreateCascadeButton( menu_bar, "optionsCascade",
					   args, ac );
  XmStringFree( label_string );
  XtManageChild( options_cascade );

  /* grid options pulldown */
  ac = 0;
  options_pulldown = XmCreatePulldownMenu( file_cascade, "optionsPulldown",
					   args, ac );

  /* grid options menu button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Grid options" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  grid_options_menu_button = XmCreatePushButton( options_pulldown, "grid",
						 args, ac );
  XmStringFree( label_string );
  XtManageChild( grid_options_menu_button );
  XtAddCallback( grid_options_menu_button, XmNactivateCallback,
		 grid_options_pressed_callback, NULL );

  /* draw mode menu button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Drawing options" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  draw_mode_button = XmCreatePushButton( options_pulldown, "drawMode",
					 args, ac );
  XmStringFree( label_string );
  XtManageChild( draw_mode_button );
  XtAddCallback( draw_mode_button, XmNactivateCallback,
		 draw_mode_pressed_callback, NULL );

  /* attach the options menu to the cascade */
  XtVaSetValues( options_cascade,
		 XmNsubMenuId, options_pulldown,
		 NULL );

  /* drawing area */
  ac = 0;
  XtSetArg( args[ ac ], XmNwidth, 600 );			ac++;
  XtSetArg( args[ ac ], XmNheight, 600 );			ac++;
  XtSetArg( args[ ac ], XmNbackground, BlackPixelOfScreen( XtScreen( toplevel ) ) ); ac++;
  drawing_area = XmCreateDrawingArea( main_window, "drawingArea", args, ac );
  XtManageChild( drawing_area );
  XtAddCallback( drawing_area, XmNexposeCallback, redraw_map_callback, NULL );
  XtAddCallback( drawing_area, XmNinputCallback, edit_ops, NULL );
  XtAddEventHandler( drawing_area, PointerMotionMask, False,
		     map_edit_motion_callback, NULL );


  XtRealizeWidget( toplevel );


  /* init globals */
  display = XtDisplay( toplevel );
  screen = XtScreen( toplevel );
  draw_window = XtWindow( drawing_area );


  /* create the gc */
  create_gcs();

  /* create load file selection box */
  load_fileselect = create_file_select( "loadFileSelect", "Load map",
					load_map_callback );

  /* create save file selection box */
  save_fileselect = create_file_select( "saveFileSelect", "Save map",
					save_map_callback );

  /* create grid options dialog */
  init_grid_optionswin();

  /* create mode options dialog */
  init_mode_options();


  XtAppMainLoop( app_context );

  return 0;
}


void create_gcs( void )
{
  XGCValues values;
  XColor screen_colour, exact_colour;

  /* draw gc */
  values.foreground = WhitePixelOfScreen( screen );
  draw_gc = XCreateGC( display, draw_window, GCForeground, &values );

  /* invert gc */
  values.foreground = BlackPixelOfScreen( screen );
  values.function = GXxor;
  invert_gc = XCreateGC( display, draw_window, GCForeground | GCFunction,
			 &values );

  /* grid gc */
  if( !XAllocNamedColor( display, DefaultColormapOfScreen( screen ),
			 "#0080B0", &screen_colour, &exact_colour ) ) {
    printf( "can't allocate colour\n" );
    exit( 1 );
  }
  values.foreground = screen_colour.pixel;
  grid_gc = XCreateGC( display, draw_window, GCForeground, &values );
}


void quit_pressed_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  exit( 0 );
}


void load_map_pressed_callback( Widget w, XtPointer user_data,
				XtPointer call_data )
{
  XtManageChild( load_fileselect );
}


void load_map_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  Widget file_name_field;
  char *filename;
  int ret;

  XtUnmanageChild( load_fileselect );

  file_name_field = XmFileSelectionBoxGetChild( load_fileselect, XmDIALOG_TEXT );
  filename = XmTextFieldGetString( file_name_field );
  ret = load_map_common( filename );
  strcpy( map_filename, filename );
  XtFree( filename );
  XClearWindow( display, draw_window );
  redraw_map();

  printf( "return code from load_map_common: %d\n", ret );
}


void save_map_pressed_callback( Widget w, XtPointer user_data,
				XtPointer call_data )
{
  if( map_filename[ 0 ] != 0 ) {
    save_map( map_filename );
  }
}


void save_as_map_pressed_callback( Widget w, XtPointer user_data,
				   XtPointer call_data )
{
  XtManageChild( save_fileselect );
}


void grid_options_pressed_callback( Widget w, XtPointer user_data,
				    XtPointer call_data )
{
  XtManageChild( grid_options_dialog );
}


void draw_mode_pressed_callback( Widget w, XtPointer user_data,
				 XtPointer call_data )
{
  XtManageChild( mode_options_form );
}


void save_map_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  Widget file_name_field;
  char *filename;

  XtUnmanageChild( save_fileselect );

  file_name_field = XmFileSelectionBoxGetChild( save_fileselect, XmDIALOG_TEXT );
  filename = XmTextFieldGetString( file_name_field );
  save_map( filename );
  XtFree( filename );
}


void redraw_map_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  redraw_map();
}


void redraw_map( void )
{
  MapObject *tmp;
  int c;
  StartPoint *startpoint;

  if( grid_visible ) {
    for( c = 0 ; c < MAP_MAX_X ; c += grid_scale ) {
      XDrawLine( display, draw_window, grid_gc,
		 c * xdiv, 0, c * xdiv, MAP_MAX_Y * ydiv );
    }
    for( c = 0 ; c < MAP_MAX_Y ; c += grid_scale ) {
      XDrawLine( display, draw_window, grid_gc,
		 0, c * ydiv, MAP_MAX_X * xdiv, c * ydiv );
    }
  }



  for( tmp = gwo_start ; tmp != NULL ; tmp = tmp->next ) {
    switch( tmp->o_type ) {
    case GWOOT_RECT:
      XDrawRectangle( display, draw_window, draw_gc,
		      tmp->valx * xdiv, tmp->valy * ydiv,
		      (tmp->valz - tmp->valx) * xdiv,
		      (tmp->valw - tmp->valy) * ydiv );
      break;

    case GWOOT_LINE:
      XDrawLine( display, draw_window, draw_gc,
		 tmp->valx * xdiv, tmp->valy * ydiv,
		 tmp->valz * xdiv, tmp->valw * ydiv );
      break;

    }
  }


#define SZ 5

  if( start_point_list != NULL ) {
    list_init_walk( start_point_list );
    while( (startpoint = list_walk_next_ptr( start_point_list )) != NULL ) {
      XDrawLine( display, draw_window, draw_gc,
		 startpoint->x * xdiv - SZ, startpoint->y *ydiv - SZ,
		 startpoint->x * xdiv + SZ, startpoint->y *ydiv  + SZ);
      XDrawLine( display, draw_window, draw_gc,
		 startpoint->x * xdiv - SZ, startpoint->y *ydiv + SZ,
		 startpoint->x * xdiv + SZ, startpoint->y *ydiv  - SZ);
    }
  }

}


void save_map( char *filename )
{
  FILE *fp;
  MapObject *w;
  StartPoint *startpoint;

  if( (fp = fopen( filename, "w" )) == NULL ) {
    printf( "can't open file\n" );
    return;
  }

  for( w = gwo_start ; w != NULL ; w = w->next ) {
    switch( w->o_type ) {
    case GWOOT_RECT:
      fprintf( fp, "box %d %d %d %d\n", w->valx, w->valy, w->valz, w->valw );
      break;

    case GWOOT_LINE:
      fprintf( fp, "line %d %d %d %d\n", w->valx, w->valy, w->valz, w->valw );
      break;
    }
  }

  if( start_point_list != NULL ) {
    list_init_walk( start_point_list );
    while( (startpoint = list_walk_next_ptr( start_point_list )) != NULL ) {
      fprintf( fp, "startpoint %d %d\n", startpoint->x, startpoint->y );
    }
  }

  fclose( fp );

}
