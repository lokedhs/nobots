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

#ifndef CIRCBUFFER_H
#define CIRCBUFFER_H

typedef union {
  int i;
  void *ptr;
} QueueValue;

typedef struct {
  QueueValue *values;
  int size;
  int head;
  int tail;
} Queue;

#define queue_pop_int(queue)		(queue_pop( (queue) ).i)
#define queue_pop_ptr(queue)		(queue_pop( (queue) ).ptr)

Queue *queue_create( void );
void queue_delete( Queue * );
QueueValue queue_pop( Queue * );
void queue_push_int( Queue *, int );
void queue_push_ptr( Queue *, void * );
void queue_push( Queue *, QueueValue );
int queue_num_values( Queue * );

#endif
