#include <stdio.h>

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"

static GtkWidget *file_selector = NULL;

static void robotfile_selected( void );

void load_robot( GtkButton *button, gpointer user_data )
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

void remove_robot( GtkButton *button, gpointer user_data )
{
  printf( "remove\n" );
}

static void robotfile_selected( void )
{
  const gchar *filename = gtk_file_selection_get_filename( GTK_FILE_SELECTION(file_selector) );

  printf( "file: %s\n", filename );
}
