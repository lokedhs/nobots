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
#include <stdlib.h>

#include "mem.h"
#include "stack.h"

Stack *StackCreate( void )
{
  Stack *stack = mymalloc( sizeof( Stack ) );

  stack->size = 10;
  stack->pos = 0;
  stack->entries = mymalloc( 10 * sizeof( StackEntry ) );

  return stack;
}

void StackDelete( Stack *stack )
{
  myfree( stack->entries );
  myfree( stack );
}

void StackPushInt( Stack *stack, int val )
{
  if( stack->pos >= stack->size ) {
    stack->size += 10;
    stack->entries = myrealloc( stack->entries, stack->size *
				sizeof( StackEntry ) );
  }
  stack->entries[ stack->pos++ ].i = val;
}

void StackPushPtr( Stack *stack, void *val )
{
  if( stack->pos >= stack->size ) {
    stack->size += 10;
    stack->entries = myrealloc( stack->entries, stack->size *
				sizeof( StackEntry ) );
  }
  stack->entries[ stack->pos++ ].ptr = val;
}

int StackPopInt( Stack *stack )
{
  if( stack->pos <= 0 ) {
    fprintf( stderr, "trying to pop values off stack with nothing on it\n" );
    exit( 1 );
  }
  return stack->entries[ --stack->pos ].i;
}

void *StackPopPtr( Stack *stack )
{
  if( stack->pos <= 0 ) {
    fprintf( stderr, "trying to pop values off stack with nothing on it\n" );
    exit( 1 );
  }
  return stack->entries[ --stack->pos ].ptr;
}

int StackLookInt( Stack *stack, int pos )
{
  if( stack->pos <= pos ) {
    fprintf( stderr, "trying to look at nonexistant stack entry\n" );
    exit( 1 );
  }
  return stack->entries[ stack->pos - pos - 1 ].i;
}

void *StackLookPtr( Stack *stack, int pos )
{
  if( stack->pos <= pos ) {
    fprintf( stderr, "trying to look at nonexistant stack entry\n" );
    exit( 1 );
  }
  return stack->entries[ stack->pos - pos - 1 ].ptr;
}
