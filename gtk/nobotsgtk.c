#include <stdio.h>

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"

void init_mainwin( int *argc, char **argv )
{
  GtkWidget *main_win;

  gtk_set_locale ();
  gtk_init (argc, &argv);

  main_win = create_main_win();
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
