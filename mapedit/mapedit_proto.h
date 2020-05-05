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

#ifndef MAPEDIT_PROTO_H
#define MAPEDIT_PROTO_H

/* editwin.c */
void create_gcs( void );
void quit_pressed_callback( Widget, XtPointer, XtPointer );
void load_map_pressed_callback( Widget, XtPointer, XtPointer );
void load_map_callback( Widget, XtPointer, XtPointer );
void save_map_pressed_callback( Widget, XtPointer, XtPointer );
void save_as_map_pressed_callback( Widget, XtPointer, XtPointer );
void grid_options_pressed_callback( Widget, XtPointer, XtPointer );
void draw_mode_pressed_callback( Widget, XtPointer, XtPointer );
void save_map_callback( Widget, XtPointer, XtPointer );
void redraw_map_callback( Widget, XtPointer, XtPointer );
void redraw_map( void );
void save_map( char *filename );

/* file_select.c */
Widget create_file_select( char *, char *, XtCallbackProc );
void fileselect_cancel_callback( Widget, XtPointer, XtPointer );

/* edit_ops.c */
void edit_ops( Widget, XtPointer, XtPointer );
void map_edit_motion_callback( Widget, XtPointer, XEvent *, Boolean * );
void align_to_grid( int *, int * );
void draw_obj( int, int, int, int );
void delete_object( int, int );
double line_distance( int, int, int, int, int, int );

/* grid_options.c */
void init_grid_optionswin( void );
void grid_options_ok_callback( Widget, XtPointer, XtPointer );

/* mode_options.c */
void init_mode_options( void );
void mode_close_callback( Widget, XtPointer, XtPointer );
void mode_delete_callback( Widget, XtPointer, XtPointer );
void mode_line_callback( Widget, XtPointer, XtPointer );
void mode_box_callback( Widget, XtPointer, XtPointer );
void mode_startpoint_callback( Widget, XtPointer, XtPointer );

#endif
