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

#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>

#include "mapedit_proto.h"
#include "modes.h"

extern Widget toplevel;

extern int draw_mode;

Widget mode_options_form;

void init_mode_options( void )
{
  Widget mode_options_shell;
  Widget draw_mode_frame, draw_mode_frame_label;
  Widget draw_mode_rowcol, draw_mode_delete, draw_mode_line, draw_mode_box;
  Widget draw_mode_startpoint;
  Widget buttons_rowcol, close_button;
  Arg args[ 20 ];
  int ac;
  XmString label_string;

  /* mode options shell */
  ac = 0;
  XtSetArg( args[ ac ], XmNtitle, "Drawing options" );		ac++;
  /*  XtSetArg( args[ ac ], XmNallowShellResize, True );		ac++;*/
  mode_options_shell = XmCreateDialogShell( toplevel, "modeOptions", args, ac );

  /* mode options form */
  ac = 0;
  XtSetArg( args[ ac ], XmNhorizontalSpacing, 5 );		ac++;
  mode_options_form = XmCreateForm( mode_options_shell, "modeOptionsForm",
				    args, ac );
  /*  XtManageChild( mode_options_form );*/

  /* buttons rowcol */
  ac = 0;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNbottomAttachment, XmATTACH_FORM );	ac++;
  buttons_rowcol = XmCreateRowColumn( mode_options_form, "buttonsRowcol",
				      args, ac );
  XtManageChild( buttons_rowcol );

  /* close button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Close" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  close_button = XmCreatePushButton( buttons_rowcol, "close", args, ac );
  XmStringFree( label_string );
  XtManageChild( close_button );
  XtAddCallback( close_button, XmNactivateCallback, mode_close_callback, NULL );

  /* draw mode frame */
  ac = 0;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNrightAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNbottomAttachment, XmATTACH_WIDGET );	ac++;
  XtSetArg( args[ ac ], XmNbottomWidget, buttons_rowcol );	ac++;
  draw_mode_frame = XmCreateFrame( mode_options_form, "drawModeFrame",
				   args, ac );
  XtManageChild( draw_mode_frame );

  /* draw mode frame label */
  ac = 0;
  label_string = XmStringCreateLocalized( "Draw mode" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNchildType, XmFRAME_TITLE_CHILD );	ac++;
  draw_mode_frame_label = XmCreateLabel( draw_mode_frame, "drawMorameLabel",
					 args, ac );
  XmStringFree( label_string );
  XtManageChild( draw_mode_frame_label );

  /* draw mode rowcolumn */
  ac = 0;
  XtSetArg( args[ ac ], XmNorientation, XmVERTICAL );		ac++;
  XtSetArg( args[ ac ], XmNradioBehavior, True );		ac++;
  draw_mode_rowcol = XmCreateRowColumn( draw_mode_frame, "drawModeRowcol",
					args, ac );
  XtManageChild( draw_mode_rowcol );

  /* draw mode delete */
  ac = 0;
  label_string = XmStringCreateLocalized( "Delete" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNindicatorType, XmONE_OF_MANY );	ac++;
  draw_mode_delete = XmCreateToggleButton( draw_mode_rowcol, "drawModeDelete",
					   args, ac );
  XmStringFree( label_string );
  XtManageChild( draw_mode_delete );
  XtAddCallback( draw_mode_delete, XmNvalueChangedCallback, mode_delete_callback,
		 NULL );

  /* draw mode line */
  ac = 0;
  label_string = XmStringCreateLocalized( "Line" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNindicatorType, XmONE_OF_MANY );	ac++;
  XtSetArg( args[ ac ], XmNset, True );				ac++;
  draw_mode_line = XmCreateToggleButton( draw_mode_rowcol, "drawModeLine",
					 args, ac );
  XmStringFree( label_string );
  XtManageChild( draw_mode_line );
  XtAddCallback( draw_mode_line, XmNvalueChangedCallback, mode_line_callback,
		 NULL );

  /* draw mode box */
  ac = 0;
  label_string = XmStringCreateLocalized( "Box" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNindicatorType, XmONE_OF_MANY );	ac++;
  draw_mode_box = XmCreateToggleButton( draw_mode_rowcol, "drawModeBox",
					args, ac );
  XmStringFree( label_string );
  XtManageChild( draw_mode_box );
  XtAddCallback( draw_mode_box, XmNvalueChangedCallback, mode_box_callback,
		 NULL );

  /* draw mode add startpoint */
  ac = 0;
  label_string = XmStringCreateLocalized( "Start point" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNindicatorType, XmONE_OF_MANY );	ac++;
  draw_mode_startpoint = XmCreateToggleButton( draw_mode_rowcol,
					       "drawStartpoint", args, ac );
  XmStringFree( label_string );
  XtManageChild( draw_mode_startpoint );
  XtAddCallback( draw_mode_startpoint, XmNvalueChangedCallback,
		 mode_startpoint_callback, NULL );

}


void mode_close_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  XtUnmanageChild( mode_options_form );
}


void mode_delete_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  XmToggleButtonCallbackStruct *toggle_button_data =
    (XmToggleButtonCallbackStruct *)call_data;

  if( toggle_button_data->set ) {
    draw_mode = MODE_DELETE;
  }
}


void mode_line_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  XmToggleButtonCallbackStruct *toggle_button_data =
    (XmToggleButtonCallbackStruct *)call_data;

  if( toggle_button_data->set ) {
    draw_mode = MODE_LINE;
  }
}


void mode_box_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  XmToggleButtonCallbackStruct *toggle_button_data =
    (XmToggleButtonCallbackStruct *)call_data;

  if( toggle_button_data->set ) {
    draw_mode = MODE_BOX;
  }
}


void mode_startpoint_callback( Widget w, XtPointer user_data,
			       XtPointer call_data )
{
  XmToggleButtonCallbackStruct *toggle_button_data =
    (XmToggleButtonCallbackStruct *)call_data;

  if( toggle_button_data->set ) {
    draw_mode = MODE_STARTPOINT;
  }
}
