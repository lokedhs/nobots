/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-2002  Magnus Ahlden (mga@algonet.se)
 *                           Elias Martenson (elias-m@algonet.se)
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

#include "config.h"

#include "map.h"
#include "octree.h"
#include "mem.h"
#include "prototypes.h"

MapObject *gwo_start = NULL;
List *start_point_list;
Branch *world_walls;

void init_map( int num_objects )
{
  int i;
  int valx, valy;

  clear_map();

  add_oobj( GWOOT_LINE, 25, 25, MAP_MAX_X-25, 25 );
  add_oobj( GWOOT_LINE, 25, MAP_MAX_Y-25, MAP_MAX_X-25, MAP_MAX_Y-25 );
  add_oobj( GWOOT_LINE, 25, 25, 25, MAP_MAX_Y-25 );
  add_oobj( GWOOT_LINE, MAP_MAX_X-25, 25, MAP_MAX_X-25, MAP_MAX_Y-25 );

  for(i = 0; i < num_objects; i++){
    valx = 100+(random()%(MAP_MAX_X-3700));
    valy = 100+(random()%(MAP_MAX_Y-3700));
    add_oobj( GWOOT_RECT, valx, valy, valx + 900+(random()%2700), 
	      valy + 900+(random()%2700));
  }

  world_walls = make_octree( OCTREE_DEPTH, 0, 0, MAP_MAX_X, MAP_MAX_Y );
}

int load_map( char *filename )
{
  int ret;

  ret = load_map_common( filename );

  add_oobj( GWOOT_LINE, 25, 25, MAP_MAX_X-25, 25 );
  add_oobj( GWOOT_LINE, 25, MAP_MAX_Y-25, MAP_MAX_X-25, MAP_MAX_Y-25 );
  add_oobj( GWOOT_LINE, 25, 25, 25, MAP_MAX_Y-25 );
  add_oobj( GWOOT_LINE, MAP_MAX_X-25, 25, MAP_MAX_X-25, MAP_MAX_Y-25 );

  world_walls = make_octree( OCTREE_DEPTH, 0, 0, MAP_MAX_X, MAP_MAX_Y );
  return ret;
}
