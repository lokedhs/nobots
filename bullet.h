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

#ifndef BULLET_H
#define BULLET_H

#include "robot.h"

typedef struct {
  Robot *robot_from;		/* Robot this bullet was fired from */
  int x_pos;			/* X coordinate */
  int y_pos;			/* Y coordinate */
  int heading;			/* Heading */
  int speed;			/* Speed */
  int power;			/* Power */
  int explode_range;		/* Range of explosion */
  int range;			/* Range of the bullet */
  int distance;			/* Total distance the bullet has travelled */
} Bullet;

Bullet *BulletCreate( Robot *, int );
void BulletDelete( Bullet * );
void BulletUpdate( Bullet * );
void update_bullets( void );
void delete_bullets_from_robot( Robot * );

#define BulletGetXCoordinate(bullet)	((bullet)->x_pos)
#define BulletGetYCoordinate(bullet)	((bullet)->y_pos)
#define BulletGetPower(bullet)		((bullet)->power)
#define BulletGetExplodeRange(bullet)	((bullet)->explode_range)

#endif
