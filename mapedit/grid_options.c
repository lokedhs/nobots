/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-2020  Elias Martenson (lokedhs@gmail.com)
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

#include <Xm/MessageB.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>

#include "mapedit_proto.h"

extern Widget toplevel;
extern Window draw_window;
extern Display *display;
extern Screen *screen;

extern int grid_active, grid_visible, grid_scale;

Widget grid_options_dialog;

static Widget active_toggle_button, visible_toggle_button;

void init_grid_optionswin( void )
{
  Widget options_form;
  Arg args[ 20 ];
  int ac;
  XmString label_string;
  XmString ok_string, cancel_string;

  /* shell */
  ac = 0;
  ok_string = XmStringCreateLocalized( "OK" );
  cancel_string = XmStringCreateLocalized( "Cancel" );
  XtSetArg( args[ ac ], XmNtitle, "Grid options" );		ac++;
  XtSetArg( args[ ac ], XmNokLabelString, ok_string );		ac++;
  XtSetArg( args[ ac ], XmNcancelLabelString, cancel_string );	ac++;
  grid_options_dialog = XmCreateTemplateDialog( toplevel, "gridOptionsDialog",
						args, ac );
  XmStringFree( ok_string );
  XmStringFree( cancel_string );
  XtAddCallback( grid_options_dialog, XmNokCallback,
		 grid_options_ok_callback, NULL );

  /* options form */
  ac = 0;
  options_form = XmCreateForm( grid_options_dialog, "optionsForm", args, ac );
  XtManageChild( options_form );

  /* active toggle button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Active" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_FORM );	ac++;
  active_toggle_button = XmCreateToggleButton( options_form,
					       "activeToggleButton", args, ac );
  XmStringFree( label_string );
  XtManageChild( active_toggle_button );
					       
  /* visible toggle button */
  ac = 0;
  label_string = XmStringCreateLocalized( "Visible" );
  XtSetArg( args[ ac ], XmNlabelString, label_string );		ac++;
  XtSetArg( args[ ac ], XmNleftAttachment, XmATTACH_FORM );	ac++;
  XtSetArg( args[ ac ], XmNtopAttachment, XmATTACH_WIDGET );	ac++;
  XtSetArg( args[ ac ], XmNtopWidget, active_toggle_button );	ac++;
  visible_toggle_button = XmCreateToggleButton( options_form,
						"visibleToggleButton",
						args, ac );

  XmStringFree( label_string );
  XtManageChild( visible_toggle_button );

}


void grid_options_ok_callback( Widget w, XtPointer user_data,
			       XtPointer call_data )
{
  Boolean tmp;

  XtVaGetValues( active_toggle_button,
		 XmNset, &tmp,
		 NULL );
  grid_active = tmp;

  XtVaGetValues( visible_toggle_button,
		 XmNset, &tmp,
		 NULL );
  grid_visible = tmp;

  XClearWindow( display, draw_window );
  redraw_map();
}
