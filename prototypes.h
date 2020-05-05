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

#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#include "lang.h"
#include "robot.h"
#include "map.h"

/* evaluate.c */
int evaluate_instruction( Program * );
CallbackValues *make_cbvalues( Program *, int );
void call_callback( Program *, CallbackValues * );
void make_cbvalues_call_callback( Program *, int, ... );
void runtime_warning( Robot *, char *, ... );

/* direction.c */
int direction_diff( int, int );
int fix_direction( int );
int line_intersect( int, int, int, int, int, int, int, int, int *, int * );
void c_to_ar( int, int, int *, int * );
int c_to_a( int, int );
int c_to_r( int, int );
void ar_to_c( int, int, int *, int * );
int ar_to_x( int, int );
int ar_to_y( int, int );

/* deffile.c */
int parse_def_file( Robot *, char * );
void deffile_error( char *, ... );

/* scan.c */
void scan_for_robots( Robot *, int, int );
void scan_for_walls( Robot *, int, int );

/* robot_move.c */
void move_robot( Robot * );

/* weapon.c */
int fire_weapon( Robot *, int );

/* simulation.c */
int init_simulation_file_map( char * );
void init_simulation_random_map( int );

#endif
