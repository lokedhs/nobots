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
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include "interface.h"
#include "support.h"
#include "game_field.h"
#include "robot_list_table.h"
#include "robotlist.h"
#include "prototypes.h"

GtkWidget *main_win;

extern RobotList *robot_list;

void init_mainwin( int *argc, char **argv )
{
  gtk_set_locale();
  gtk_init( argc, &argv );

  init_gl();

  main_win = create_main_win();
  init_robot_tree_table();

  gtk_widget_show( main_win );
}

void main_loop( void )
{
  printf( "main_loop called\n" );
  gtk_main();
}

void g_delete_robot( int n )
{
  printf( "g_delete_robot(%d)\n", n );
}

void quit_selected( GtkMenuItem *menuitem, gpointer user_data )
{
  gtk_exit( 0 );
}

void start_clicked( GtkButton *button, gpointer user_data )
{
  char *map_file = "foo.map";

  init_simulation_file_map( map_file );

  init_game_field_window();
}
