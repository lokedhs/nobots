/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-98  Elias Martenson (elias.martenson@sun.se)
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

#include <math.h>

#include "mem.h"
#include "robot.h"
#include "bullet.h"
#include "list.h"
#include "octree.h"
#include "robotlist.h"
#include "prototypes.h"

#define DEG_TO_RAD(X)	(((double)(X) * (M_PI * 2)) / 360)

List *bullet_list = NULL;

extern RobotList *robot_list;

/*
 *  Create a new bullet
 *
 *  In:
 *      robot     = robot that fired this bullet
 *      direction = direction of the bullet
 */
Bullet *BulletCreate( Robot *robot, int direction )
{
  Bullet *ret = mymalloc( sizeof( Bullet ) );

  ret->robot_from = robot;
  ret->x_pos = RobotGetXCoordinate( robot );
  ret->y_pos = RobotGetYCoordinate( robot );
  ret->heading = direction;
  ret->speed = RobotGetWeaponSpeed( robot );
  ret->power = RobotGetWeaponPower( robot );
  ret->explode_range = 1000;
  ret->range = RobotGetWeaponRange( robot );
  ret->distance = 0;

  if( bullet_list == NULL ) {
    bullet_list = ListCreate();
  }
  ListAddToTailPtr( bullet_list, ret );

  return ret;
}

/*
 *  Delete a bullet
 */
void BulletDelete( Bullet *bullet )
{
  ListDeletePtr( bullet_list, bullet );
  myfree( bullet );
}

/*
 *  Update the position of a bullet, and check if it
 *  has hit something.
 */
void BulletUpdate( Bullet *bullet )
{
  double curr_dir;
  double new_x, new_y;
  int bot_dx, bot_dy;
  double dist;
  Robot *w;

  curr_dir = DEG_TO_RAD( (double)bullet->heading / 10 );
  new_x = bullet->x_pos + cos( curr_dir ) * bullet->speed;
  new_y = bullet->y_pos + sin( curr_dir ) * bullet->speed;
  if( intersect_obj( bullet->x_pos, bullet->y_pos, new_x, new_y ) ) {
    BulletDelete( bullet );
    return;
  }
  bullet->x_pos = new_x;
  bullet->y_pos = new_y;

  bullet->distance += bullet->speed;

  /*
   *  If the total distance travelled is longer than the bullets range,
   *  delete the bullet
   */
  if( bullet->distance > bullet->range ) {
    BulletDelete( bullet );
    return;
  }


  /*
   *  If the new position of the bullet is in the vicinity of a
   *  robot, inflict some damage, and remove it.
   */
  RobotListInitWalk( robot_list );
  while( (w = RobotListWalkNext( robot_list )) != NULL ) {
    if( bullet->robot_from != w ) {
      bot_dx = abs( RobotGetXCoordinate( w ) - bullet->x_pos );
      bot_dy = abs( RobotGetYCoordinate( w ) - bullet->y_pos );
      dist = sqrt( (bot_dx * bot_dx) + (bot_dy * bot_dy) );
      if( dist <= bullet->explode_range ) {
	/*	printf( "robot %s took %d hit points\n", w->robot_name, bullet->power );*/
	RobotTakeDamage( w, bullet->power );
	BulletDelete( bullet );
      }
    }
  }
}

/*
 *  Update all bullets
 */
void update_bullets( void )
{
  Bullet *bullet;

  ListInitWalk( bullet_list );
  while( (bullet = ListWalkNextPtr( bullet_list )) != NULL ) {
    BulletUpdate( bullet );
  }
}

/*
 *  Delete all bullets belonging to a robot
 */
void delete_bullets_from_robot( Robot *robot )
{
  Bullet *bullet;
  List *bullet_remove_list;

  if( bullet_list == NULL ) {
    return;
  }

  bullet_remove_list = ListCreate();

  ListInitWalk( bullet_list );
  while( (bullet = ListWalkNextPtr( bullet_list )) != NULL ) {
    if( bullet->robot_from == robot ) {
      ListAddToTailPtr( bullet_remove_list, bullet );
    }
  }

  ListInitWalk( bullet_remove_list );
  while( (bullet = ListWalkNextPtr( bullet_remove_list )) != NULL ) {
    BulletDelete( bullet );
  }

  ListDelete( bullet_remove_list );
}
