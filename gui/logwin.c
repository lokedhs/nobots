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
#include <Xm/DialogS.h>
#include <Xm/Text.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include <stdio.h>
#include <stdarg.h>

#include "mem.h"
#include "g_proto.h"

typedef void LogFunc( char *, ... );
extern LogFunc *log_print;

extern Display *display;
extern Widget toplevel;

Widget logwin_shell;
Widget log_text;

void init_logwin( void )
{
  Arg args[ 20 ];
  int ac;
  Atom wm_delete_atom;

  ac = 0;
  XtSetArg( args[ ac ], XmNtitle, "Messages" );			ac++;
  XtSetArg( args[ ac ], XmNallowShellResize, True );		ac++;
  XtSetArg( args[ ac ], XmNdeleteResponse, XmUNMAP );	ac++;
  logwin_shell = XtCreatePopupShell( "logWindow", topLevelShellWidgetClass,
				     toplevel, args, ac );

  ac = 0;
  XtSetArg( args[ ac ], XmNcolumns, 80 );			ac++;
  XtSetArg( args[ ac ], XmNrows, 5 );				ac++;
  XtSetArg( args[ ac ], XmNeditable, False );			ac++;
  XtSetArg( args[ ac ], XmNeditMode, XmMULTI_LINE_EDIT );	ac++;
  log_text = XmCreateScrolledText( logwin_shell, "logText", args, ac );
  XtManageChild( log_text );

  wm_delete_atom = XmInternAtom( display, "WM_DELETE_WINDOW", False );
  XmAddWMProtocolCallback( logwin_shell, wm_delete_atom,
			   logwin_delete_callback, NULL );

  XtManageChild( logwin_shell );

  log_print = logwin_add;
}


void logwin_add( char *format, ... )
{
  va_list ap;
  char str[ 1024 ];
  XmTextPosition last_pos;

  va_start( ap, format );
  vsprintf( str, format, ap );
  strcat( str, "\n" );
  last_pos = XmTextGetLastPosition( log_text );
  XmTextInsert( log_text, last_pos, str );
  XmTextShowPosition( log_text, XmTextGetLastPosition( log_text ) );
  va_end( ap );
}


void logwin_delete_callback( Widget w, XtPointer user_data, XtPointer call_data )
{
  XtUnmanageChild( w );
}
