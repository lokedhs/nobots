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

#define QueuePopInt(queue)		(QueuePop( (queue) ).i)
#define QueuePopPtr(queue)		(QueuePop( (queue) ).ptr)

Queue *QueueCreate( void );
void QueueDelete( Queue * );
QueueValue QueuePop( Queue * );
void QueuePushInt( Queue *, int );
void QueuePushPtr( Queue *, void * );
void QueuePush( Queue *, QueueValue );
int QueueNumValues( Queue * );

#endif