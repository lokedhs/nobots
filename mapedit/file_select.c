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

#include <Xm/FileSB.h>
#include <Xm/DialogS.h>
#include <Xm/TextF.h>
#include <Xm/List.h>

#include <stdio.h>

#include "mapedit_proto.h"

extern Widget toplevel;

Widget create_file_select( char *name, char *title, XtCallbackProc callback )
{
  Arg args[ 20 ];
  int ac;
  Widget fileselect_shell;
  Widget fileselect;
  Widget ok_button;
  Widget cancel_button;
  Widget filter_text;
  char *str, *str2;
  char filter_str[ 1024 ];
  XmString filter_mstring;

  /* shell */
  ac = 0;
  XtSetArg( args[ ac ], XmNtitle, title );			ac++;
  fileselect_shell = XmCreateDialogShell( toplevel, name, args, ac );

  /* file selector */
  fileselect = XmCreateFileSelectionBox( fileselect_shell, "fileSelect",
					 NULL, 0 );

  filter_text = XmFileSelectionBoxGetChild( fileselect, XmDIALOG_FILTER_TEXT );
  str = XmTextFieldGetString( filter_text );
    for( str2 = str + strlen( str ) - 1 ;
       str2 > str && *str2 != '/' ;
       str2 -- );
  *str2 = 0;
  sprintf( filter_str, "%s/*.map", str );
  filter_mstring = XmStringCreateLocalized( filter_str );
  XmFileSelectionDoSearch( fileselect, filter_mstring );
  XmStringFree( filter_mstring );
  XtFree( str );


  ok_button = XmFileSelectionBoxGetChild( fileselect, XmDIALOG_OK_BUTTON );
  XtAddCallback( ok_button, XmNactivateCallback, callback, NULL );

  cancel_button = XmFileSelectionBoxGetChild( fileselect,
					       XmDIALOG_CANCEL_BUTTON );
  XtAddCallback( cancel_button, XmNactivateCallback, fileselect_cancel_callback,
		 fileselect );

  return fileselect;
}


void fileselect_cancel_callback( Widget w, XtPointer user_data,
				 XtPointer call_data )
{
  XtUnmanageChild( (Widget)user_data );
}
