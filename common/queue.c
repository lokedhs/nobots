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
#include "queue.h"

Queue *queue_create( void )
{
  Queue *ret = mymalloc( sizeof( Queue ) );

  ret->size = 10;
  ret->values = mymalloc( 10 * sizeof( QueueValue ) );
  ret->head = 0;
  ret->tail = 0;

  return ret;
}

void queue_delete( Queue *queue )
{
  myfree( queue->values );
  myfree( queue );
}

QueueValue queue_pop( Queue *queue )
{
  QueueValue ret;

  if( queue->head == queue->tail ) {
    fprintf( stderr, "queue error: no values in buffer\n" );
    exit( 1 );
  }
  ret = queue->values[ queue->tail++ ];
  if( queue->tail >= queue->size ) {
    queue->tail = 0;
  }

  return ret;
}

void queue_push( Queue *queue, QueueValue val )
{
  queue->values[ queue->head++ ] = val;
  if( queue->head >= queue->size ) {
    queue->head = 0;
  }
  if( queue->head == queue->tail ) {
    queue->size += 10;
    queue->values = myrealloc( queue->values,
				    queue->size *
				    sizeof( QueueValue ) );
    memmove( &queue->values[ queue->tail ] + 10,
	     &queue->values[ queue->tail ],
	     ((queue->size - 10) - queue->tail) *
	     sizeof( QueueValue ) );
    queue->tail += 10;
  }
}

void queue_push_int( Queue *queue, int val )
{
  QueueValue tmp;

  tmp.i = val;
  queue_push( queue, tmp );
}

void queue_push_ptr( Queue *queue, void *val )
{
  QueueValue tmp;

  tmp.ptr = val;
  queue_push( queue, tmp );
}

int queue_num_values( Queue *queue )
{
  if( queue->head >= queue->tail ) {
    return queue->head - queue->tail;
  }
  else {
    return queue->size - queue->tail + queue->head;
  }
}
