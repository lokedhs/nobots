/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1998  Elias Martenson (elias-m@algonet.se)
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
#include <Xm/FileSB.h>
#include <Xm/DialogS.h>
#include <Xm/TextF.h>
#include <Xm/List.h>

#include <stdio.h>
#include <unistd.h>

#include "robot.h"
#include "robotlist.h"
#include "map.h"
#include "g_proto.h"
#include "mem.h"


void fileselect_ok_callback( Widget, XtPointer, XtPointer );
void fileselect_cancel_callback( Widget, XtPointer, XtPointer );

extern Widget toplevel;
extern Widget robot_list_widget;
extern RobotList *robot_list;
extern int in_progress;

Widget fileselect;

void init_fileselect( void )
{
  Widget fileselect_shell;
  Widget filter_text;
  Arg args[ 20 ];
  int ac;
  char *str, *str2;
  char filter_str[ 1024 ];
  XmString filter_mstring;

  /* dialog shell */
  ac = 0;
  XtSetArg( args[ ac ], XmNtitle, "Add robot" );		ac++;
  fileselect_shell = XmCreateDialogShell( toplevel, "fileSelectShell",
					  args, ac );
  /*  XtManageChild( fileselect_shell );*/

  fileselect = XmCreateFileSelectionBox( fileselect_shell, "fileSelect",
					 NULL, 0 );
  /*  XtManageChild( fileselect );*/

  filter_text = XmFileSelectionBoxGetChild( fileselect, XmDIALOG_FILTER_TEXT );
  str = XmTextFieldGetString( filter_text );
  for( str2 = str + strlen( str ) - 1 ;
       str2 > str && *str2 != '/' ;
       str2 -- );
  *str2 = 0;
  sprintf( filter_str, "%s/*.robot", str );
  filter_mstring = XmStringCreateLocalized( filter_str );
  XmFileSelectionDoSearch( fileselect, filter_mstring );
  XmStringFree( filter_mstring );
  /*  XmTextFieldSetString( filter_text, filter_str );*/

  XtFree( str );


  XtAddCallback( fileselect, XmNokCallback, fileselect_ok_callback, NULL );
  XtAddCallback( fileselect, XmNcancelCallback, fileselect_cancel_callback,
		 NULL );



#if 0
  /*
   *  TEST CODE
   */
  {
    XmString name;
    Robot *robot1, *robot2;

    chdir( "/export/home/elias/robot/bots" );
    robot1 = robot_create( "search2.robot" );
    robot2 = robot_create( "turning.robot" );
    RobotListAddRobot( robot_list, robot1 );
    RobotListAddRobot( robot_list, robot2 );
    name = XmStringCreateLocalized( robot1->robot_name );
    XmListAddItem( robot_list_widget, name, 0 );
    XmStringFree( name );
    name = XmStringCreateLocalized( robot2->robot_name );
    XmListAddItem( robot_list_widget, name, 0 );
    XmStringFree( name );
  }
#endif


}

void fileselect_ok_callback( Widget w, XtPointer call_data, XtPointer user_data )
{
  Widget file_name_field;
  char *file_name;
  char *dir_name;
  Robot *robot;
  XmString name;

  XtUnmanageChild( fileselect );

  file_name_field = XmFileSelectionBoxGetChild( fileselect, XmDIALOG_TEXT );
  dir_name = XmTextFieldGetString( file_name_field );

  for( file_name = dir_name + strlen( dir_name ) - 1 ;
       file_name > dir_name && *file_name != '/' ;
       file_name-- );

  if( file_name > dir_name ) {
    *file_name = 0;
    file_name++;
    chdir( dir_name );
  }

  if( (robot = robot_create( file_name )) != NULL ) {
    name = XmStringCreateLocalized( robot->robot_name );
    XmListAddItem( robot_list_widget, name, 0 );
    XmStringFree( name );
    if( in_progress ) {
      robot_place_on_map( robot );
    }
    robotlist_add_robot( robot_list, robot );
  }

  XtFree( dir_name );
}

void fileselect_cancel_callback( Widget w, XtPointer call_data,
				 XtPointer user_data )
{
  XtUnmanageChild( fileselect );
}
