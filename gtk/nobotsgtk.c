#include <stdio.h>

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"
#include "robot_list_table.h"

GtkWidget *main_win;

void init_mainwin( int *argc, char **argv )
{
  gtk_set_locale ();
  gtk_init (argc, &argv);

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
