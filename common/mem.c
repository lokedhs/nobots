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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void *mymalloc( size_t size )
{
  void *mem;

  if( (mem = malloc( size )) == NULL ) {
    fprintf( stderr, "Could not allocate %d bytes\n", size );
    exit( 1 );
  }
  return mem;
}

void *myrealloc( void *mem, size_t size )
{
  void *mem2;

  if( (mem2 = realloc( mem, size )) == NULL ) {
    fprintf( stderr, "Could not reallocate %d bytes\n", size );
    exit( 1 );
  }
  return mem2;
}

void myfree( void *mem )
{
  free( mem );
}

char *mystrdup( char *str )
{
  char *str2;

  if( (str2 = strdup( str )) == NULL ) {
    fprintf( stderr, "Could not strdup\n" );
    exit( 1 );
  }
  return str2;
}
