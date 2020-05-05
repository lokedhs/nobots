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
#include <Xm/FileSB.h>
#include <Xm/DialogS.h>
#include <Xm/TextF.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/DrawingA.h>
#include <Xm/List.h>
#include <Xm/Form.h>

#include <stdio.h>

#include "map.h"
#include "g_proto.h"

extern Display *display;
extern Screen *screen;

extern Widget toplevel;
extern Widget map_file_filename_field;
extern MapObject *gwo_start;

Widget map_fileselect;
Widget map_preview;
MapObject *preview_gwo_start = NULL;

void init_map_fileselect( void )
{
  Widget fileselect_shell;
  Widget map_preview_form, map_preview_frame, map_preview_label;
  Widget filter_text;
  Widget file_list;
  Arg args[ 20 ];
  int ac;
  char *str, *str2;
  char filter_str[ 1024 ];
  XmString filter_mstring;
  XmString label_string;

  /* dialog shell */
  ac = 0;
  XtSetArg( args[ ac ], XmNtitle, "Select map" );		ac++;
  fileselect_shell = XmCreateDialogShell( toplevel, "mapFileSelectShell",
					  args, ac );

  /* file selection box */
  ac = 0;
  XtSetArg( args[ ac ], XmNresizePolicy, XmRESIZE_NONE );	ac++;
  map_fileselect = XmCreateFileSelectionBox( fileselect_shell, "fileSelect",
					     args, ac );

  XtAddCallback( map_fileselect, XmNokCallback, map_fileselect_ok_callback,
		 NULL );
  XtAddCallback( map_fileselect, XmNcancelCallback,
		 map_fileselect_cancel_callback, NULL );

  file_list = XmFileSelectionBoxGetChild( map_fileselect, XmDIALOG_LIST );
  XtAddCallback( file_list, XmNbrowseSelectionCallback,
		 map_fileselect_clicked_callback, NULL );

  /* preview form */
  ac = 0;
  map_preview_form = XmCreateForm( map_fileselect, "mapPreviewForm", args, ac );
  XtManageChild( map_preview_form );

  /* preview frame */
  ac = 0;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNmarginWidth, 5 );			ac++;
  XtSetArg( args[ ac ], XmNmarginHeight, 5 );			ac++;
  map_preview_frame = XmCreateFrame( map_preview_form, "mapPreviewFrame",
				     args, ac );
  XtManageChild( map_preview_frame );
  XtAddEventHandler( map_preview_frame, StructureNotifyMask, True,
		     map_preview_frame_changed, NULL );

  /* preview label */
  ac = 0;
  label_string = XmStringCreateLocalized( "Preview" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNchildType, XmFRAME_TITLE_CHILD );	ac++;
  map_preview_label = XmCreateLabel( map_preview_frame, "mapPreviewFrameLabel",
				     args, ac );
  XtManageChild( map_preview_label );

  /* preview area */
  ac = 0;
  XtSetArg( args[ ac ], XmNwidth, 100 );			ac++;
  XtSetArg( args[ ac ], XmNheight, 100 );			ac++;
  XtSetArg( args[ ac ], XmNbackground, BlackPixelOfScreen( screen ) ); ac++;
  map_preview = XmCreateDrawingArea( map_preview_frame, "mapPreview", args, ac );
  XtManageChild( map_preview );
  XtAddCallback( map_preview, XmNexposeCallback, map_preview_redraw_callback,
		 NULL );




  filter_text = XmFileSelectionBoxGetChild( map_fileselect,
					    XmDIALOG_FILTER_TEXT );
  str = XmTextFieldGetString( filter_text );
  for( str2 = str + strlen( str ) - 1 ;
       str2 > str && *str2 != '/' ;
       str2 -- );
  *str2 = 0;
  sprintf( filter_str, "%s/*.map", str );
  filter_mstring = XmStringCreateLocalized( filter_str );
  XmFileSelectionDoSearch( map_fileselect, filter_mstring );
  XmStringFree( filter_mstring );
  XtFree( str );
}

void map_preview_frame_changed( Widget w, XtPointer user_data, XEvent *e,
				Boolean *cont )
{
  XtVaSetValues( map_preview,
		 XmNwidth, 100,
		 XmNheight, 100,
		 NULL );
}

void map_fileselect_clicked_callback( Widget w, XtPointer call_data,
				      XtPointer user_data )
{
  Widget file_name_field;
  char *file_name;
  MapObject *old_gwo_start;

  file_name_field = XmFileSelectionBoxGetChild( map_fileselect, XmDIALOG_TEXT );
  file_name = XmTextFieldGetString( file_name_field );

  old_gwo_start = gwo_start;
  gwo_start = preview_gwo_start;
  if( load_map_common( file_name ) ) {
    XClearWindow( display, XtWindow( map_preview ) );
  }
  else {
    clear_map();
  }
  preview_gwo_start = gwo_start;
  gwo_start = old_gwo_start;

  map_preview_redraw();

  XtFree( file_name );
}

void map_fileselect_ok_callback( Widget w, XtPointer call_data,
				 XtPointer user_data )
{
  Widget file_name_field;
  char *file_name;

  XtUnmanageChild( map_fileselect );

  file_name_field = XmFileSelectionBoxGetChild( map_fileselect, XmDIALOG_TEXT );
  file_name = XmTextFieldGetString( file_name_field );
  XmTextFieldSetString( map_file_filename_field, file_name );
  XtFree( file_name );
}

void map_fileselect_cancel_callback( Widget w, XtPointer call_data,
				     XtPointer user_data )
{
  XtUnmanageChild( map_fileselect );
}

void map_preview_redraw_callback( Widget w, XtPointer call_data,
				  XtPointer user_data )
{
  map_preview_redraw();
}

void map_preview_redraw( void )
{
  MapObject *old_gwo_start;
  GC draw_gc;
  XGCValues values;

  draw_gc = DefaultGCOfScreen( screen );
  values.foreground = WhitePixelOfScreen( screen );
  XChangeGC( display, draw_gc, GCForeground, &values );

  old_gwo_start = gwo_start;
  gwo_start = preview_gwo_start;
  draw_objects( display, XtWindow( map_preview ), draw_gc, 0, 0,
		100.0 / MAP_MAX_X, 100.0 / MAP_MAX_Y );
  gwo_start = old_gwo_start;
}
