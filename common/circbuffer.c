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

#include <stdio.h>
#include <string.h>

#include "mem.h"
#include "circbuffer.h"

Circbuffer *CircbufferCreate( void )
{
  Circbuffer *ret = mymalloc( sizeof( Circbuffer ) );

  ret->size = 10;
  ret->values = mymalloc( 10 * sizeof( CircbufferValue ) );
  ret->head = 0;
  ret->tail = 0;

  return ret;
}

void CircbufferDelete( Circbuffer *circbuffer )
{
  myfree( circbuffer->values );
  myfree( circbuffer );
}

CircbufferValue CircbufferPop( Circbuffer *circbuffer )
{
  CircbufferValue ret;

  if( circbuffer->head == circbuffer->tail ) {
    fprintf( stderr, "circbuffer error: no values in buffer\n" );
    exit( 1 );
  }
  ret = circbuffer->values[ circbuffer->tail++ ];
  if( circbuffer->tail >= circbuffer->size ) {
    circbuffer->tail = 0;
  }

  return ret;
}

void CircbufferPush( Circbuffer *circbuffer, CircbufferValue val )
{
  circbuffer->values[ circbuffer->head++ ] = val;
  if( circbuffer->head >= circbuffer->size ) {
    circbuffer->head = 0;
  }
  if( circbuffer->head == circbuffer->tail ) {
    circbuffer->size += 10;
    circbuffer->values = myrealloc( circbuffer->values,
				    circbuffer->size *
				    sizeof( CircbufferValue ) );
    memmove( &circbuffer->values[ circbuffer->tail ] + 10,
	     &circbuffer->values[ circbuffer->tail ],
	     ((circbuffer->size - 10) - circbuffer->tail) *
	     sizeof( CircbufferValue ) );
    circbuffer->tail += 10;
  }
}

void CircbufferPushInt( Circbuffer *circbuffer, int val )
{
  CircbufferValue tmp;

  tmp.i = val;
  CircbufferPush( circbuffer, tmp );
}

void CircbufferPushPtr( Circbuffer *circbuffer, void *val )
{
  CircbufferValue tmp;

  tmp.ptr = val;
  CircbufferPush( circbuffer, tmp );
}

int CircbufferNumValues( Circbuffer *circbuffer )
{
  if( circbuffer->head >= circbuffer->tail ) {
    return circbuffer->head - circbuffer->tail;
  }
  else {
    return circbuffer->size - circbuffer->tail + circbuffer->head;
  }
}
