#include <gnome.h>
#include <stdio.h>

#include "interface.h"
#include "support.h"

void init_mainwin( int *argc, char **argv )
{
  GtkWidget *main_win;

  gnome_init( "nobots", "1.0", *argc, argv );
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

void open_clicked( GtkButton *button, gpointer user_data )
{
  printf( "open\n" );
}

void remove_clicked( GtkButton *button, gpointer user_data )
{
  printf( "remove\n" );
}
