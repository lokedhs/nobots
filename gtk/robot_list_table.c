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

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"
#include "robot.h"
#include "robotlist.h"
#include "robot_list_table.h"
#include "callbacks.h"

enum {
  ID_COLUMN,
  NAME_COLUMN,
  SHIELDS_COLUMN,
  ROBOT_COLUMN,
  ROBOT_LIST_COLUMNS
};

extern RobotList *robot_list;
extern GtkWidget *main_win;

static GtkTreeStore *robot_list_store;

void init_robot_tree_table( void )
{
  GtkTreeView *robot_list_table;
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;

  robot_list_table =
    GTK_TREE_VIEW(lookup_widget( GTK_WIDGET(main_win), "robot_list_table" ));

  robot_list_store = gtk_tree_store_new( ROBOT_LIST_COLUMNS,
					 G_TYPE_INT,	/* ID */
					 G_TYPE_STRING, /* Robot name */
					 G_TYPE_INT, /* Shields */
					 G_TYPE_POINTER ); /* Robot */
  gtk_tree_view_set_model( robot_list_table, GTK_TREE_MODEL(robot_list_store) );
  g_object_unref( G_OBJECT(robot_list_store) );

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes( "ID", renderer,
						     "text", ID_COLUMN,
						     NULL );
  gtk_tree_view_append_column( robot_list_table, column );
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes( "Name", renderer,
						     "text", NAME_COLUMN,
						     NULL );
  gtk_tree_view_append_column( robot_list_table, column );

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes( "Shields", renderer,
						     "text", SHIELDS_COLUMN,
						     NULL );
  gtk_tree_view_append_column( robot_list_table, column );
}

void add_robot_list_entry( Robot *robot )
{
  GtkTreeIter iter;

  gtk_tree_store_append( robot_list_store, &iter, NULL );
  gtk_tree_store_set( robot_list_store, &iter,
		      ID_COLUMN, robot_get_id( robot ),
		      NAME_COLUMN, robot_get_name( robot ),
		      SHIELDS_COLUMN, robot_get_shields( robot ),
		      ROBOT_COLUMN, robot,
		      -1 );
}

void remove_robot( GtkMenuItem *button, gpointer user_data )
{
  GtkTreeView *robot_list_table =
    GTK_TREE_VIEW(lookup_widget( GTK_WIDGET(main_win), "robot_list_table" ));
  GtkTreeSelection *select = gtk_tree_view_get_selection( robot_list_table );
  GtkTreeIter iter;

  if( gtk_tree_selection_get_selected( select, NULL, &iter ) ) {
    GValue value = { 0 };
    Robot *robot;

    //    g_value_init( &value, G_TYPE_POINTER );
    gtk_tree_model_get_value( GTK_TREE_MODEL(robot_list_store), &iter,
			      ROBOT_COLUMN, &value );
    robot = (Robot *)g_value_get_pointer( &value );
    robot_delete( robot );
    g_value_unset( &value );
    gtk_tree_store_remove( robot_list_store, &iter );
  }
}
