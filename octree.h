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

#ifndef OCTREE_H
#define OCTREE_H

#include "map.h"

typedef struct branch_struct {
  int x1, y1, x2, y2;
  struct branch_struct *ul;
  struct branch_struct *ur;
  struct branch_struct *ll;
  struct branch_struct *lr;
  MapObject **objs;
  int level;
} Branch;

#define OCTREE_DEPTH		5

Branch *make_octree( int, int, int, int, int );
MapObject ***octree_find_world_objs( int, int, int, int );
void octree_find_objs( Branch *, int, int, int, int );
void start_linesearch( int, int, int, int );
int get_next_line( void );
int intersect_obj( int, int, int, int );

#endif
