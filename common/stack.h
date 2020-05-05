/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-2020  Elias Martenson (lokedhs@gmail.com)
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

#ifndef STACK_H
#define STACK_H

typedef union {
  int i;
  void *ptr;
} StackEntry;

typedef struct {
  int size;
  int pos;
  StackEntry *entries;
} Stack;

#define StackGetSize(X)		((X)->pos)

Stack *stack_create( void );
void stack_delete( Stack * );
void stack_push_int( Stack *, int );
void stack_push_ptr( Stack *, void * );
int stack_pop_int( Stack * );
void *stack_pop_ptr( Stack * );
int stack_look_int( Stack *, int );
void *stack_look_ptr( Stack *, int );

#endif
