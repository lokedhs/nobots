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

#include <stdio.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include "interface.h"
#include "support.h"
#include "game_field.h"

GtkWidget *game_field_window = NULL;

static GdkGLConfig *glconfig;

void init_gl( void )
{
  const gint config_attributes[] = {
    GDK_GL_DOUBLEBUFFER,
    GDK_GL_RGBA,
    GDK_GL_RED_SIZE,        1,
    GDK_GL_GREEN_SIZE,      1,
    GDK_GL_BLUE_SIZE,       1,
    GDK_GL_DEPTH_SIZE,      12,
    GDK_GL_ATTRIB_LIST_NONE
  };

  if( !gdk_gl_query_extension() ) {
    fprintf( stderr, "OpenGL extension is not supported\n" );
    gtk_exit( 1 );
  }

  glconfig = gdk_gl_config_new( &config_attributes[ 0 ] );
  if( glconfig == NULL ) {
    printf( "Can't find double-buffered visual. Trying single-buffered.\n" );
    glconfig = gdk_gl_config_new( &config_attributes[ 1 ] );
    if( glconfig == NULL ) {
      printf( "Can't find OpenGL-capable visual.\n" );
      gtk_exit( 1 );
    }
  }
}

void init_game_field_window( void )
{
  GtkWidget *game_field_area;

  game_field_window = create_game_field_window();

  game_field_area = lookup_widget( GTK_WIDGET(game_field_window),
				   "game_field_area" );
  gtk_widget_set_gl_capability( game_field_area,
				glconfig,
				GDK_GL_RGBA_TYPE,
				NULL,
				TRUE);

  gtk_widget_show( game_field_window );
}
