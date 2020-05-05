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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"
#include "robot.h"
#include "robotlist.h"
#include "robot_list_table.h"
#include "callbacks.h"

extern RobotList *robot_list;

static GtkWidget *file_selector = NULL;

static void robotfile_selected( void );

void load_robot( GtkMenuItem *menuitem, gpointer user_data )
{
  if( file_selector == NULL ) {
    file_selector = gtk_file_selection_new( "Select robot descriptor file" );
    gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION(file_selector)->ok_button ),
			"clicked", GTK_SIGNAL_FUNC( robotfile_selected ), NULL );   gtk_signal_connect_object( GTK_OBJECT( GTK_FILE_SELECTION(file_selector)->ok_button ),
													       "clicked", GTK_SIGNAL_FUNC( gtk_widget_hide ),
													       (gpointer)file_selector);
    
    gtk_signal_connect_object( GTK_OBJECT( GTK_FILE_SELECTION(file_selector)->cancel_button ),
			       "clicked", GTK_SIGNAL_FUNC( gtk_widget_hide ),
			       (gpointer)file_selector );
  }

  gtk_widget_show( file_selector );
}

static void robotfile_selected( void )
{
  char *dirname = gtk_file_selection_get_filename( GTK_FILE_SELECTION(file_selector) );
  char *filename;
  Robot *robot;

  for( filename = dirname + strlen( dirname ) - 1 ;
       filename > dirname && *filename != '/' ;
       filename-- );

  if( filename > dirname ) {
    *filename = 0;
    filename++;
    chdir( dirname );
  }

  printf( "file: %s\ndir: %s\n", filename, dirname );
  if( (robot = robot_create( filename )) != NULL ) {
    robotlist_add_robot( robot_list, robot );
    puts("loaded and added");

    add_robot_list_entry( robot );
  }

}
