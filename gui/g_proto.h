/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1998  Elias Martenson (lokedhs@gmail.com)
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

#ifndef G_PROTO_H
#define G_PROTO_H

#include "batwin.h"
#include "statswin.h"

/* mainwin.c */
void init_mainwin( int *, char ** );
void add_robot_callback( Widget, XtPointer, XtPointer );
void delete_robot_callback( Widget, XtPointer, XtPointer );
void reload_robot_callback( Widget, XtPointer, XtPointer );
void reload_robot( int );
void quit_program_callback( Widget, XtPointer, XtPointer );
void map_random_checkbox_callback( Widget, XtPointer, XtPointer );
void map_file_checkbox_callback( Widget, XtPointer, XtPointer );
void map_file_selection_callback( Widget, XtPointer, XtPointer );
void robot_list_menu_callback( Widget, XtPointer, XEvent *, Boolean * );
void start_game_callback( Widget, XtPointer, XtPointer );
void pause_game_callback( Widget, XtPointer, XtPointer );
void show_log_window_callback( Widget, XtPointer, XtPointer );
void robot_list_callback( Widget, XtPointer, XtPointer );
void robot_list_popup_callback( Widget, XtPointer, XtPointer );
void g_delete_robot( int );

/* file_select.c */
void init_fileselect( void );
void fileselect_ok_callback( Widget, XtPointer, XtPointer );
void fileselect_cancel_callback( Widget, XtPointer, XtPointer );

/* batwin.c */
void init_batwins( void );
void create_batwin( int );
void close_batwin( BattleWindow * );
void batwin_size_changed_callback( Widget, XtPointer, XEvent *, Boolean * );
void batwin_close_callback( Widget, XtPointer, XtPointer );
void horiz_scrollbar_changed_callback( Widget, XtPointer, XtPointer );
void vert_scrollbar_changed_callback( Widget, XtPointer, XtPointer );
void batwin_redraw_callback( Widget, XtPointer, XtPointer );
void batwin_button_callback( Widget, XtPointer, XtPointer );
void zoom_changed_callback( Widget, XtPointer, XtPointer );
void draw_batwin( BattleWindow * );
void center_batwin( BattleWindow *, int, int );
void batwin_set_scrollbars( BattleWindow * );
void draw_rot_line( BattleWindow *, int, int, int, int, int, int, int );
void rotate_point( BattleWindow *, int, int, int, int, int, int *, int * );

/* statswin.c */
StatsWindow *create_stats_window( int );
Widget statswin_create_scale( Widget, char *, Widget, char *, int );
void statswin_close_callback( Widget, XtPointer, XtPointer );
void close_statswin( StatsWindow * );
void update_stats_window( StatsWindow * );

/* msgwin.c */
void init_msgwin( void );
void show_msgwin( char * );

/* logwin.c */
void init_logwin( void );
void logwin_add( char *, ... );
void logwin_delete_callback( Widget, XtPointer, XtPointer );

/* update_loop.c */
void update_loop( XtPointer, XtIntervalId * );

/* g_bullet.c */
void draw_bullets( BattleWindow * );

/* g_map.c */
void draw_objects( Display *, Drawable, GC, int, int, double, double );

/* map_file_select.c */
void init_map_fileselect( void );
void map_preview_frame_changed( Widget, XtPointer, XEvent *, Boolean * );
void map_fileselect_clicked_callback( Widget, XtPointer, XtPointer );
void map_fileselect_ok_callback( Widget, XtPointer, XtPointer );
void map_fileselect_cancel_callback( Widget, XtPointer, XtPointer );
void map_preview_redraw_callback( Widget, XtPointer, XtPointer );
void map_preview_redraw( void );

#endif
