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

#include "config.h"

#include <stdio.h>
#include <math.h>

#include "prototypes.h"

int direction_diff( int src, int dest )
{
  int right;
  int left;

  right = fix_direction( dest - src );
  left = fix_direction( src - dest );
  if( right <= left ) {
    return right;
  }
  return -left;
}

int fix_direction( int direction )
{
  return (direction < 0 ? (direction % 3600) + 3600 : direction) % 3600;
}


#define SWAP(X,Y)		do {			\
				  int tmpx = (X);	\
				  (X) = (Y);		\
				  (Y) = tmpx;		\
				} while( 0 )

#define MAX(X,Y)		((X) > (Y) ? (X) : (Y))
#define MIN(X,Y)		((X) < (Y) ? (X) : (Y))

/*
 *  Line intersection routine
 *
 *  In:
 *    First line:  l1x1,l1y1 - l1x2,l1y2
 *    Second line: l2x1,l2y1 - l2x2,l2y2
 *    Pointer to the intersection point: intersect_x,intersect_y
 *
 *  Out:
 *    Return value: 1 if the lines intersect, 0 if they don't
 *    If return value is 1, writes the intersection point
 *    to intersect_x,intersect_y
 */

int line_intersect( int l1x1, int l1y1, int l1x2, int l1y2,
		    int l2x1, int l2y1, int l2x2, int l2y2,
		    int *intersect_x, int *intersect_y )
{
  int cx1, cx2;			/* common x1 -> x2 */
  double k1, k2;		/* k value for the lines */
  int nl1y1, nl1y2;		/* new l1 y coordinates */
  int nl2y1, nl2y2;		/* new l2 y coordinates */
  double intersect_wid;		/* x intersection point */

  /*
   *  both lines has to be drawn from left to right
   */
  if( l1x1 > l1x2 ) {
    SWAP( l1x1, l1x2 );
    SWAP( l1y1, l1y2 );
  }
  if( l2x1 > l2x2 ) {
    SWAP( l2x1, l2x2 );
    SWAP( l2y1, l2y2 );
  }

  /*
   *  find the common segment
   */
  cx1 = MAX( l1x1, l2x1 );
  cx2 = MIN( l1x2, l2x2 );

  /*
   *  if there is no common segment, abort
   */
  if( cx1 > cx2 ) {
    return 0;
  }

  /*
   *  find the y-coordinates for the lines at points cx1 and cx2
   */
#ifdef STUPID_MATH
  if( l1x2 - l1x1 == 0 )	k1 = (l1y2 > 0) ? MAXFLOAT : -MAXFLOAT;
  else				k1 = (double)(l1y2 - l1y1) / (l1x2 - l1x1);
#else
  k1 = (double)(l1y2 - l1y1) / (l1x2 - l1x1);
#endif
  if( l1x2 - l1x1 == 0 ) {
    nl1y1 = l1y1;
    nl1y2 = l1y2;
  }
  else {
    nl1y1 = (cx1 - l1x1) * k1 + l1y1;
    nl1y2 = (cx2 - l1x1) * k1 + l1y1;
  }

#ifdef STUPID_MATH
  if( l2x2 - l2x1 == 0 )	k2 = (l1y2 > 0) ? MAXFLOAT : -MAXFLOAT;
  else				k2 = (double)(l2y2 - l2y1) / (l2x2 - l2x1);
#else
  k2 = (double)(l2y2 - l2y1) / (l2x2 - l2x1);
#endif
  if( l2x2 - l2x1 == 0 ) {
    nl2y1 = l2y1;
    nl2y2 = l2y2;
  }
  else {
    nl2y1 = (cx1 - l2x1) * k2 + l2y1;
    nl2y2 = (cx2 - l2x1) * k2 + l2y1;
  }

  /*
   *  if the y coordinates of one line is above the y coordinates
   *  of the other, then the lines doesn't intersect
   */
  if( (nl1y1 < nl2y1 && nl1y2 < nl2y2) ||
      (nl1y1 > nl2y1 && nl1y2 > nl2y2) ) {
    return 0;
  }

  /*
   *  the lines intersect, store the intersection point
   */
  if( intersect_x != NULL && intersect_y != NULL ) {
    intersect_wid = -((nl1y1 - nl2y1) / (k1 - k2));
    *intersect_x = intersect_wid + cx1;
    *intersect_y = intersect_wid * k1 + nl1y1;
  }

  return 1;
}

void c_to_ar( int x, int y, int *ret_angle, int *ret_radius )
{
  double angle;
  double radius;

  radius = sqrt( (double)x*x + (double)y*y );
  angle = acos( x / radius );
  if( y < 0 )  angle = M_PI * 2 - angle;

  *ret_angle = (angle * 3600) / (M_PI * 2);
  *ret_radius = radius;

  if( *ret_angle < 0 || *ret_angle >= 3600 ) {
    printf( "PANIC! ret_angle:%d angle:%f, x:%d,y:%d,radius:%f\n", *ret_angle, angle, x, y, radius );
  }

  /*  if( y < 0 ) {
    *ret_angle = (*ret_angle + 1800) % 3600;
  }*/

}

int c_to_a( int x, int y )
{
  int angle;
  int radius;

  c_to_ar( x, y, &angle, &radius );
  return angle;
}

int c_to_r( int x, int y )
{
  int angle;
  int radius;

  c_to_ar( x, y, &angle, &radius );
  return radius;
}

void ar_to_c( int angle, int radius, int *x, int *y )
{
  double na = (angle * M_PI * 2) / 3600;

  *x = cos(na) * radius;
  *y = sin(na) * radius;
}

int ar_to_x( int angle, int radius )
{
  int x;
  int y;

  ar_to_c( angle, radius, &x, &y );
  return x;
}

int ar_to_y( int angle, int radius )
{
  int x;
  int y;

  ar_to_c( angle, radius, &x, &y );
  return y;
}
