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

#include <X11/Xlib.h>
#ifdef USE_DOUBLE_BUFFER_EXTENSION
#include <X11/extensions/Xdbe.h>
#endif
#ifdef USE_MULTI_BUFFER_EXTENSION
#include <X11/extensions/multibuf.h>
#endif
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/TextF.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "robot.h"
#include "robotlist.h"
#include "prototypes.h"
#include "g_proto.h"
#include "list.h"

typedef void LogFunc( char *, ... );
extern LogFunc *log_print;

XtAppContext app_context;
Widget toplevel;
Widget robot_list_widget;
Display *display;
Screen *screen;

Widget robot_list_popup;
Widget map_random_checkbox;
Widget map_file_checkbox;
Widget num_objects_field;
Widget map_file_filename_field;
Widget map_file_selection_button;

int in_progress = 0;

extern Widget fileselect;
extern Widget logwin_shell;
extern Widget log_text;
extern Widget map_fileselect;
extern RobotList *robot_list;
extern List *bullet_list;
extern int multi_buffer_method;

XtIntervalId timeout_id;
int timeout_active = 0;

String fallback_res[] = {
  "*XmPushButton.marginLeft: 5",
  "*XmPushButton.marginRight: 5",
  "*XmPushButton.marginTop: 2",
  "*XmPushButton.marginBottom: 2",
  NULL
};

void init_mainwin( int *argc, char **argv )
{
  Widget main_form;
  Widget map_frame, map_frame_label, map_form;
  Widget map_random_rowcolumn, num_objects_label;
  Widget map_file_rowcolumn;
  Widget robot_list_frame, robot_list_frame_label, robot_list_form;
  Widget robot_list_add, robot_list_delete, robot_list_reload, button_rowcolumn;
  Widget controls_rowcolumn, start_button, pause_button;
  Widget quit_button, show_log_button;
  Arg args[ 20 ];
  int ac;
  XmString label_string;
  XmString title_label, open_window_label, open_stats_window_label, debug_label;
  XmString invulnerable_label;
  int mbuf_eventbase, mbuf_errorbase;
  int dbe_major, dbe_minor;

  toplevel = XtAppInitialize( &app_context, "Robot", NULL, 0, argc, argv,
			      fallback_res, NULL, 0 );


  /* check multi buffer method */
#ifdef USE_MULTI_BUFFER_EXTENSION
  if( XmbufQueryExtension( XtDisplay( toplevel ), &mbuf_eventbase,
			   &mbuf_errorbase ) ) {
    printf( "using Multi-Buffer extension\n" );
    multi_buffer_method = MULTIBUFFER_METHOD_MBUF;
  }
#endif

#ifdef USE_DOUBLE_BUFFER_EXTENSION
  if( multi_buffer_method == MULTIBUFFER_METHOD_NONE ) {
    if( XdbeQueryExtension( XtDisplay( toplevel ), &dbe_major, &dbe_minor ) ) {
      printf( "using DOUBLE-BUFFER extension\n" );
      multi_buffer_method = MULTIBUFFER_METHOD_DBLBUF;
    }
  }
#endif



  /* main form */
  ac = 0;
  XtSetArg( args[ ac ], XmNhorizontalSpacing, 5 );		ac++;
  main_form = XmCreateForm( toplevel, "form", args, ac );
  XtManageChild( main_form );

  /* controls rowcolumn */
  ac = 0;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNbottomAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNorientation, XmHORIZONTAL );		ac++;
  XtSetArg( args[ ac ], XmNisAligned, True );			ac++;
  XtSetArg( args[ ac ], XmNentryAlignment, XmALIGNMENT_CENTER );	ac++;
  controls_rowcolumn = XmCreateRowColumn( main_form, "controlsRowColumn",
					  args, ac );
  XtManageChild( controls_rowcolumn );

  /* start button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Start" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  start_button = XmCreatePushButton( controls_rowcolumn, "start", args, ac );
  XmStringFree( label_string );
  XtManageChild( start_button );
  XtAddCallback( start_button, XmNactivateCallback, start_game_callback, NULL );

  /* pause button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Pause" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  pause_button = XmCreatePushButton( controls_rowcolumn, "pause", args, ac );
  XmStringFree( label_string );
  XtManageChild( pause_button );
  XtAddCallback( pause_button, XmNactivateCallback, pause_game_callback, NULL );

  /* quit button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Quit" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  quit_button = XmCreatePushButton( controls_rowcolumn, "quit", args, ac );
  XmStringFree( label_string );
  XtManageChild( quit_button );
  XtAddCallback( quit_button, XmNactivateCallback, quit_program_callback, NULL );

  /* show log button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Show log window" );	ac++;
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  show_log_button = XmCreatePushButton( controls_rowcolumn, "showLogWindow",
					args, ac );
  XmStringFree( label_string );
  XtManageChild( show_log_button );
  XtAddCallback( show_log_button, XmNactivateCallback, show_log_window_callback,
		 NULL );

  /* map frame */
  ac = 0;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNbottomAttachment, XmATTACH_WIDGET );	ac++;
  XtSetArg( args[ ac ], XmNbottomWidget, controls_rowcolumn );	ac++;
  map_frame = XmCreateFrame( main_form, "mapFrame", args, ac );
  XtManageChild( map_frame );

  /* map frame label */
  ac = 0;
  label_string = XmStringCreateLocalized( "Map" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNchildType, XmFRAME_TITLE_CHILD );	ac++;
  map_frame_label = XmCreateLabel( map_frame, "mapFrameLabel", args, ac );
  XmStringFree( label_string );
  XtManageChild( map_frame_label );

  /* map form */
  ac = 0;
  map_form = XmCreateForm( map_frame, "mapForm", args, ac );
  XtManageChild( map_form );

  /* map_random_rowcolumn */
  ac = 0;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNorientation, XmHORIZONTAL );		ac++;
  map_random_rowcolumn = XmCreateRowColumn( map_form, "mapRandomRowColumn",
					     args, ac );
  XtManageChild( map_random_rowcolumn );

  /* map random checkbox */
  ac = 0;
  label_string = XmStringCreateLocalized( "Create random" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNindicatorType, XmONE_OF_MANY );	ac++;
  XtSetArg( args[ ac ], XmNset, True );				ac++;
  map_random_checkbox = XmCreateToggleButton( map_random_rowcolumn,
					      "mapRandom", args, ac );
  XmStringFree( label_string );
  XtManageChild( map_random_checkbox );
  XtAddCallback( map_random_checkbox, XmNvalueChangedCallback,
		 map_random_checkbox_callback, NULL );

  /* num objects label */
  ac = 0;
  label_string = XmStringCreateLocalized( "Number of objects:" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNmarginLeft, 20 );			ac++;
  num_objects_label = XmCreateLabel( map_random_rowcolumn, "numObjectsLabel",
				     args, ac );
  XmStringFree( label_string );
  XtManageChild( num_objects_label );

  /* num objects field */
  ac = 0;
  XtSetArg( args[ ac ], XmNcolumns, 6 );			ac++;
  XtSetArg( args[ ac ], XmNvalue, "4" );			ac++;
  num_objects_field = XmCreateTextField( map_random_rowcolumn,
					 "numObjectsField", args, ac );
  XtManageChild( num_objects_field );

  /* map file rowcolumn */
  ac = 0;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_WIDGET );	ac++;
  XtSetArg( args[ ac ], XmNtopWidget, map_random_rowcolumn );	ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNorientation, XmHORIZONTAL );		ac++;
  map_file_rowcolumn = XmCreateRowColumn( map_form, "mapFileRowColumn",
					  args, ac );
  XtManageChild( map_file_rowcolumn );

  /* map file checkbox */
  ac = 0;
  label_string = XmStringCreateLocalized( "Use file" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNindicatorType, XmONE_OF_MANY );	ac++;
  map_file_checkbox = XmCreateToggleButton( map_file_rowcolumn,
					    "mapFile", args, ac );
  XmStringFree( label_string );
  XtManageChild( map_file_checkbox );
  XtAddCallback( map_file_checkbox, XmNvalueChangedCallback,
		 map_file_checkbox_callback, NULL );

  /* map file filename field */
  ac = 0;
  XtSetArg( args[ ac ], XmNcolumns, 30 );			ac++;
  XtSetArg( args[ ac ], XmNsensitive, False );			ac++;
  XtSetArg( args[ ac ], XmNvalue, "/export/home/elias/robot/city.map" ); ac++;
  map_file_filename_field = XmCreateTextField( map_file_rowcolumn,
					       "mapFileFilenameField",
					       args, ac );
  XtManageChild( map_file_filename_field );

  /* map file file selection button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Select" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNsensitive, False );			ac++;
  map_file_selection_button = XmCreatePushButton( map_file_rowcolumn,
						  "mapFileSelectionButton",
						  args, ac );
  XmStringFree( label_string );
  XtManageChild( map_file_selection_button );
  XtAddCallback( map_file_selection_button, XmNactivateCallback,
		 map_file_selection_callback, NULL );

  /* robot list frame */
  ac = 0;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNbottomAttachment, XmATTACH_WIDGET );	ac++;
  XtSetArg( args[ ac ], XmNbottomWidget, map_frame );		ac++;
  XtSetArg( args[ ac ], XmNmarginWidth, 5 );			ac++;
  XtSetArg( args[ ac ], XmNmarginHeight, 5 );			ac++;
  robot_list_frame = XmCreateFrame( main_form, "robotListFrame", args, ac );
  XtManageChild( robot_list_frame );

  /* robot list frame label */
  ac = 0;
  label_string = XmStringCreateLocalized( "Robots" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNchildType, XmFRAME_TITLE_CHILD );	ac++;
  robot_list_frame_label = XmCreateLabel( robot_list_frame, "frameLabel",
					  args, ac );
  XmStringFree( label_string );
  XtManageChild( robot_list_frame_label );

  /* robot list form */
  ac = 0;
  XtSetArg( args[ ac ], XmNchildType, XmFRAME_WORKAREA_CHILD );	ac++;
  robot_list_form = XmCreateForm( robot_list_frame, "robotListForm", args, ac );
  XtManageChild( robot_list_form );

  /* button rowcolumn */
  ac = 0;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNorientation, XmVERTICAL );		ac++;
  XtSetArg( args[ ac ], XmNisAligned, True );			ac++;
  XtSetArg( args[ ac ], XmNentryAlignment, XmALIGNMENT_CENTER );	ac++;
  button_rowcolumn = XmCreateRowColumn( robot_list_form, "robotsRowColumn",
					args, ac );
  XtManageChild( button_rowcolumn );

  /* robot list */
  ac = 0;
  XtSetArg( args[ ac ], XmNvisibleItemCount, 10 );		ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_WIDGET );	ac++;
  XtSetArg( args[ ac ], XmNrightWidget, button_rowcolumn );	ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNbottomAttachment, XmATTACH_FORM );	ac++;
  robot_list_widget = XmCreateScrolledList( robot_list_form, "robotList",
					    args, ac );
  XtManageChild( robot_list_widget );
  XtAddCallback( robot_list_widget, XmNdefaultActionCallback,
		 robot_list_callback, NULL );
  XtAddEventHandler( robot_list_widget, ButtonPressMask, False,
		     robot_list_menu_callback, NULL );

  /* robot list popup */
  title_label = XmStringCreateLocalized( "Robot options" );
  open_window_label = XmStringCreateLocalized( "Open robot view" );
  open_stats_window_label = XmStringCreateLocalized( "Open stats window" );
  debug_label = XmStringCreateLocalized( "Debug" );
  invulnerable_label = XmStringCreateLocalized( "Invulnerable" );
  robot_list_popup = XmVaCreateSimplePopupMenu( robot_list_widget,
						"robotListPopup",
						robot_list_popup_callback,
						XmVaTITLE, title_label,
						XmVaSEPARATOR,
						XmVaCASCADEBUTTON, open_window_label, NULL,
						XmVaCASCADEBUTTON, open_stats_window_label, NULL,
						XmVaCASCADEBUTTON, debug_label, NULL,
						XmVaCASCADEBUTTON, invulnerable_label, NULL,
						NULL );
  XmStringFree( title_label );
  XmStringFree( open_window_label );
  XmStringFree( open_stats_window_label );
  XmStringFree( debug_label );
  XmStringFree( invulnerable_label );

  /* add button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Add" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  robot_list_add = XmCreatePushButton( button_rowcolumn, "add", args, ac );
  XmStringFree( label_string );
  XtManageChild( robot_list_add );
  XtAddCallback( robot_list_add, XmNactivateCallback, add_robot_callback, NULL );

  /* delete button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Delete" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  robot_list_delete = XmCreatePushButton( button_rowcolumn, "delete", args, ac );
  XmStringFree( label_string );
  XtManageChild( robot_list_delete );
  XtAddCallback( robot_list_delete, XmNactivateCallback, delete_robot_callback,
		 NULL );

  /* reload button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Reload" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  robot_list_reload = XmCreatePushButton( button_rowcolumn, "reload", args, ac );
  XmStringFree( label_string );
  XtManageChild( robot_list_reload );
  XtAddCallback( robot_list_reload, XmNactivateCallback, reload_robot_callback,
		 NULL );


  XtRealizeWidget( toplevel );

  /* init some globals */
  display = XtDisplay( toplevel );
  screen = XtScreen( toplevel );

  /* init fileselector */
  init_fileselect();
  init_batwins();
  init_msgwin();
  init_logwin();
  init_map_fileselect();
}

void robot_list_menu_callback( Widget w, XtPointer user_data, XEvent *e,
			       Boolean *cont )
{
  if( e->xbutton.button == 3 ) {
    XmMenuPosition( robot_list_popup, &e->xbutton );
    XtManageChild( robot_list_popup );
  }
}


void add_robot_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  XtManageChild( fileselect );
}

void delete_robot_callback( Widget w, XtPointer user_data,
			    XtPointer call_data )
{
  int pos_count;
  int *pos_list;
  int c;
  Robot *robot;

  XmListGetSelectedPos( robot_list_widget, &pos_list, &pos_count );

  for( c = pos_count - 1 ; c >= 0 ; c-- ) {
    robot = robotlist_get_robot_by_index( robot_list, pos_list[ c ] - 1 );
    robotlist_delete_robot( robot_list, robot );
  }
  XtFree( (XtPointer)pos_list );
}

void reload_robot_callback( Widget w, XtPointer user_data,
			    XtPointer call_data )
{
  int pos_count;
  int *pos_list;
  int c;
  
  XmListGetSelectedPos( robot_list_widget, &pos_list, &pos_count );
  for( c = 0 ; c < pos_count ; c++ ) {
    reload_robot( pos_list[ c ] - 1 );
  }
  XtFree( (XtPointer)pos_list );
}

void reload_robot( int robot_index )
{
  Robot *robot;
  Robot *new_robot;

  robot = robotlist_get_robot_by_index( robot_list, robot_index );

  if( chdir( robot->robot_dir ) == -1 ) {
    log_print( "Could not change to directory: %s", robot->robot_dir );
    return;
  }
  new_robot = robot_create_with_id( robot->robot_filename, robot_get_id( robot ) );
  if( new_robot != NULL ) {
    robotlist_replace_robot( robot_list, robot, new_robot );
    new_robot->id = robot->id;
    new_robot->start_x = robot->start_x;
    new_robot->start_y = robot->start_y;
    new_robot->x_pos = new_robot->start_x;
    new_robot->y_pos = new_robot->start_y;
    robot_delete( robot );
  }
}

void quit_program_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  exit( 0 );
}

void start_game_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  Boolean from_file;
  char *str;
  int ret;
  Robot *robot;
  XmString tmp_label;

  if( in_progress ) {
    return;
  }

  XtVaGetValues( map_file_checkbox,
		 XmNset, &from_file,
		 NULL );

  if( from_file ) {
    str = XmTextFieldGetString( map_file_filename_field );
    ret = load_map( str );
    XtFree( str );
    if( !ret ) {
      show_msgwin( "Error while loading map file" );
      return;
    }
  }
  else {
    str = XmTextFieldGetString( num_objects_field );
    init_map( atoi( str ) );
    XtFree( str );
  }

  robotlist_init_walk( robot_list );
  while( (robot = robotlist_walk_next( robot_list )) != NULL ) {
    robot_place_on_map( robot );
  }

  in_progress = 1;

  bullet_list = list_create();
  create_batwin( -1 );

  tmp_label = XmStringCreateLocalized( "Stop" );
  XtVaSetValues( w,
		 XmNlabelString, tmp_label,
		 NULL );
  XmStringFree( tmp_label );

  timeout_id = XtAppAddTimeOut( app_context, 1000, update_loop, NULL );
  timeout_active = 1;
}

void pause_game_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  if( timeout_active ) {
    XtRemoveTimeOut( timeout_id );
    timeout_active = 0;
  }
  else {
    XtAppAddTimeOut( app_context, 100, update_loop, NULL );
    timeout_active = 1;
  }
}

void show_log_window_callback( Widget w, XtPointer user_data,
			       XtPointer call_data )
{
  XtManageChild( logwin_shell );
}

void map_random_checkbox_callback( Widget w, XtPointer user_data,
				   XtPointer call_data )
{
  XtVaSetValues( map_random_checkbox,
		 XmNset, True,
		 NULL );

  XtVaSetValues( map_file_checkbox,
		 XmNset, False,
		 NULL );

  XtVaSetValues( num_objects_field,
		 XmNsensitive, True,
		 NULL );

  XtVaSetValues( map_file_filename_field,
		 XmNsensitive, False,
		 NULL );

  XtVaSetValues( map_file_selection_button,
		 XmNsensitive, False,
		 NULL );
}

void map_file_checkbox_callback( Widget w, XtPointer user_data,
				 XtPointer call_data )
{
  XtVaSetValues( map_file_checkbox,
		 XmNset, True,
		 NULL );

  XtVaSetValues( map_random_checkbox,
		 XmNset, False,
		 NULL );

  XtVaSetValues( num_objects_field,
		 XmNsensitive, False,
		 NULL );

  XtVaSetValues( map_file_filename_field,
		 XmNsensitive, True,
		 NULL );

  XtVaSetValues( map_file_selection_button,
		 XmNsensitive, True,
		 NULL );
}

void map_file_selection_callback( Widget w, XtPointer user_data,
				  XtPointer call_data )
{
  XtManageChild( map_fileselect );
}

void robot_list_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  XmListCallbackStruct *list_call_data = (XmListCallbackStruct *)call_data;

  create_batwin( list_call_data->item_position - 1 );
}

void robot_list_popup_callback( Widget w, XtPointer user_data,
				XtPointer call_data )
{
  int pos_count;
  int *pos_list;
  int c;
  Robot *tmp_robot;
  XmString tmp_label;
  char str[ 256 ];

  XmListGetSelectedPos( robot_list_widget, &pos_list, &pos_count );

  switch( (int)user_data ) {
  case 0:			/* open robot view */
    for( c = 0 ; c < pos_count ; c++ ) {
      create_batwin( pos_list[ c ] - 1 );
    }
    break;

  case 1:			/* open stats window */
    for( c = 0 ; c < pos_count ; c++ ) {
      create_stats_window( pos_list[ c ] - 1 );
    }
    break;

  case 2:			/* debug */
    for( c = 0 ; c < pos_count ; c++ ) {
      robotlist_get_robot_by_index( robot_list,
				    pos_list[ c ] - 1 )->program->debug = 1;
    }
    break;

  case 3:			/* invulnerable */
    for( c = 0 ; c < pos_count ; c++ ) {
      tmp_robot = robotlist_get_robot_by_index( robot_list,
						pos_list[ c ] - 1 );
      robot_set_invulnerable( tmp_robot, !robot_get_invulnerable( tmp_robot ) );
      sprintf( str, "%s%s", tmp_robot->robot_name,
	       (robot_get_invulnerable( tmp_robot ) ? " *" : "") );
      tmp_label = XmStringCreateLocalized( str );
      XmListReplaceItemsPos( robot_list_widget, &tmp_label, 1, pos_list[ c ] );
      XmStringFree( tmp_label );
    }
    break;

  }
  XtFree( (XtPointer)pos_list );
}

void g_delete_robot( int n )
{
  XmListDeletePos( robot_list_widget, n + 1 );
}

void main_loop( void )
{
  XtAppMainLoop( app_context );
}
