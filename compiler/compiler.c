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
#include <stdarg.h>
#include <string.h>

#include "mem.h"
#include "compiler.h"
#include "list.h"
#include "stack.h"

extern List *dup_strings;
extern int tmp_level;
extern Stack *parsestk;
extern char src_file[];
extern int src_line;
extern char *current_func;

int compile_ok = 1;

int main( void )
{
  yyparse();
  return 0;
}

void cf( char *format, ... )
{
  va_list ap;

  va_start( ap, format );
  vprintf( format, ap );
  va_end( ap );
  printf( "\n" );
}

void addinstr0( char *instruction )
{
  cf( "%s", instruction );
}

void addinstr1( char *instruction, Variable arg1 )
{
  cf( "%s %s", instruction, var_to_string( arg1 ) );
}

void addinstr2( char *instruction, Variable arg1, Variable arg2 )
{
  cf( "%s %s,%s", instruction, var_to_string( arg1 ),
      var_to_string( arg2 ) );
}

Variable mk_int( int n )
{
  Variable var;

  var.type = VAR_TYPE_INT;
  var.value.i = n;
  return var;
}

Variable mk_abs( char *str )
{
  Variable var;

  var.type = VAR_TYPE_ABS;
  var.value.s = lstrdup( str );
  return var;
}

Variable mk_sym( char *str )
{
  Variable var;

  var.type = VAR_TYPE_SYM;
  var.value.s = lstrdup( str );
  return var;
}

Variable mk_ref( char *str )
{
  Variable var;

  var.type = VAR_TYPE_REF;
  var.value.s = lstrdup( str );
  return var;
}

char *mk_label( void )
{
  static int label_ctr = 1;
  char str[ 1024 ];

  sprintf( str, "L%d", label_ctr++ );
  return mystrdup( str );
}

void add_label( char *label )
{
  cf( "%s:", label );
}

void add_array( char *name, int size, List *init_data )
{
  int val;
  int wid_counter;
  char str[ 128 ];
  char str2[ 128 ];
  int first = 1;

  if( init_data != NULL ) {
    if( size > 0 && list_size( init_data ) > size ) {
      compiler_error( "list size (%d) is smaller than init data (%d)",
		      size, list_size( init_data ) );
      return;
    }
    
    if( size > 0 ) {
      cf( "$array %s:%d [", name, size );
    }
    else {
      cf( "$array %s [", name );
    }

    wid_counter = 0;
    str[ 0 ] = 0;
    list_init_walk( init_data );
    while( list_walk_next_int( init_data, &val ) ) {
      if( !first ) {
	strcat( str, ", " );
      }
      first = 0;
      if( ++wid_counter >= 16 ) {
	str[ strlen( str ) - 1 ] = 0;
	cf( "%s", str );
	wid_counter = 0;
	str[ 0 ] = 0;
	first = 1;
      }
      sprintf( str2, "%d", val );
      strcat( str, str2 );
    }
    cf( "%s ]", str );

  }
  else {

    cf( "$array %s:%d", name, size );

  }
}


char *var_to_string( Variable var )
{
  static char str[ 1024 ];

  switch( var.type ) {
  case VAR_TYPE_INT:
    sprintf( str, "%d", var.value.i );
    break;
  case VAR_TYPE_SYM:
  case VAR_TYPE_TMPSYM:
    strcpy( str, var.value.s );
    break;
  case VAR_TYPE_REF:
    sprintf( str, "&%s", var.value.s );
    break;
  case VAR_TYPE_ABS:
    sprintf( str, "%%%s", var.value.s );
    break;
  default:
    fprintf( stderr, "var_to_string: couldn't happen, illegal var type\n" );
  }
  return lstrdup( str );
}

Variable replvar( Variable src )
{
  Variable dest;
  char *str;

  if( src.type == VAR_TYPE_TMPSYM ) {
    dest = src;
  }
  else {
    dest.type = VAR_TYPE_TMPSYM;
    str = get_tmpvar_name( ++tmp_level );
    dest.value.s = lstrdup( str );
    cf( "move %s,%s", var_to_string( src ), dest.value.s );
  }
  return dest;
}   

Variable newvar( void )
{
  Variable dest;
  char *str;

  dest.type = VAR_TYPE_TMPSYM;
  str = get_tmpvar_name( ++tmp_level );
  dest.value.s = lstrdup( str );
  return dest;
}

char *get_tmpvar_name( int level )
{
  static char str[ 1024 ];

  if( current_func == NULL ) {
    sprintf( str, "tmpvar_%d", level );
  }
  else {
    sprintf( str, "tmpvar_%s_%d", current_func, level );
  }
  return str;
}

void save_tmpvars( void )
{
  int c;
  char *str;

  for( c = 1 ; c <= tmp_level ; c++ ) {
    str = get_tmpvar_name( c );
    addinstr1( "push", MK_SYM( str ) );
  }
}

void restore_tmpvars( void )
{
  int c;
  char *str;

  for( c = tmp_level ; c > 0 ; c-- ) {
    str = get_tmpvar_name( c );
    addinstr1( "pop", MK_SYM( str ) );
  }
}

char *lstrdup( char *str )
{
  char *str2;

  if( (str2 = strdup( str )) == NULL ) {
    fprintf( stderr, "Couldn't strdup\n" );
    exit( 1 );
  }
  list_add_to_tail_ptr( dup_strings, str2 );
  return str2;
}

void clean_statement( void )
{
  /* cleanup the lstrdup list */
  while( dup_strings->first != NULL ) {
    myfree( dup_strings->last->val.ptr );
    list_delete_list_entry( dup_strings, dup_strings->last );
  }

  /* set the tmpvar level to 0 */
  tmp_level = 0;
}

void compiler_error( char *format, ... )
{
  va_list ap;

  compile_ok = 0;
  fprintf( stderr, "error in %s, line %d: ", src_file, src_line );
  va_start( ap, format );
  vfprintf( stderr, format, ap );
  va_end( ap );
  fprintf( stderr, "\n" );
}
