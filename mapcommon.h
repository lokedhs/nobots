#ifndef MAPCOMMON_H
#define MAPCOMMON_H

#define MAP_MAX_X 100000
#define MAP_MAX_Y 100000

#define GWOOT_RECT 1 /* Graphics_World_ObstacleObject_Type Rectangle */
#define GWOOT_LINE 2 /* Graphics_World_ObstacleObject_Type Line */
#define GWOOT_CIRC 3 /* Graphics_World_ObstacleObject_Type Circle */

typedef struct map_object_struct{
  struct map_object_struct *next; /* List pointer */
  int o_type, valx, valy, valz, valw;
} MapObject;

typedef struct {
  int x;
  int y;
} StartPoint;

int load_map_common( char * );
void clear_map( void );
void add_oobj( int, int, int, int, int );
void add_startpoint( int, int );

#endif
