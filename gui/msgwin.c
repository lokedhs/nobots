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
#ifdef USE_DOUBLE_BUFFER_EXTENSION
#include <X11/extensions/Xdbe.h>
#endif
#ifdef USE_MULTI_BUFFER_EXTENSION
#include <X11/extensions/multibuf.h>
#endif
#include <Xm/DialogS.h>
#include <Xm/MessageB.h>

#include "g_proto.h"

extern Widget toplevel;

Widget msgwin;
Widget message_box;

void init_msgwin( void )
{
  Arg args[ 20 ];
  int ac;
  Widget cancel_button;

  ac = 0;
  XtSetArg( args[ ac ], XmNtitle, "Error" );			ac++;
  msgwin = XmCreateDialogShell( toplevel, "msgwinShell", args, ac );

  ac = 0;
  XtSetArg( args[ ac ], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL ); ac++;
  XtSetArg( args[ ac ], XmNdialogType, XmDIALOG_ERROR );	ac++;
  message_box = XmCreateMessageBox( msgwin, "messageBox", args, ac );

  cancel_button = XmMessageBoxGetChild( message_box, XmDIALOG_CANCEL_BUTTON );
  XtUnmanageChild( cancel_button );
}


void show_msgwin( char *message )
{
  XmString message_string;

  message_string = XmStringCreateLocalized( message );
  XtVaSetValues( message_box,
		 XmNmessageString, message_string,
		 NULL );

  XtManageChild( message_box );
}
