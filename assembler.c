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
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#include "mem.h"
#include "lang.h"
#include "assembler.h"
#include "hash.h"
#include "list.h"
#include "prototypes.h"


typedef void LogFunc( char *, ... );
extern LogFunc *log_print;

static Instruction *code;
static int code_length;
static int code_pos;
static HashTable *symbol_table;
static HashTable *define_table;

char src_file[ 1024 ] = "unknown";
int src_line;
int asm_ok;

extern FILE *yyin;

typedef struct {
  int is_set;
  int value;
  int is_array;
  int size;
  List *refs;
  List *array_values;
} SymbolEntry;

typedef struct {
  int value;
} DefineEntry;

typedef struct {
  char *instruction_name;
  int num_args;
} SyntaxTableEntry;

static void make_arg( int *, int, ArgType, int *, ptrdiff_t, int );
static SymbolEntry *find_or_create_symbol_entry( char * );

#include "oper_syntax.c"

Program *new_program( char *file )
{
  FILE *fp;

  if( (fp = fopen( file, "r" )) == NULL ) {
    log_print( "Could not open assembly file: %s", file );
    return NULL;
  }

  strcpy( src_file, file );

  return new_program_fd( fp, 1 );
}

Program *new_program_fd( FILE *fp, int line )
{
  code = mymalloc( 10 * sizeof( Instruction ) );
  code_length = 10;
  code_pos = 0;
  symbol_table = hashtable_create( NULL, 0 );
  define_table = hashtable_create( NULL, 0 );
  asm_ok = 1;
  src_line = line;

  yyin = fp;
  yyparse();
  fclose( fp );
  return close_program();
}

int add_instruction( char *instruction_name,
		     int arg1_type, ArgType arg1_val, int arg1_offset,
		     int arg2_type, ArgType arg2_val, int arg2_offset )
{
  int opcode;
  int argflags1, argflags2;

  for( opcode = 0 ; syntax_table[ opcode ].instruction_name ; opcode++ ) {
    if( strcmp( syntax_table[ opcode ].instruction_name, instruction_name ) == 0 ) {
      break;
    }
  }
  if( syntax_table[ opcode ].instruction_name == NULL ) {
    asm_error( "unknown instruction: %s", instruction_name );
    return 0;
  }

  code[ code_pos ].opcode = opcode;
  
  if( syntax_table[ opcode ].num_args == 0 ) {
    if( arg1_type != ASMARG_NONE || arg2_type != ASMARG_NONE ) {
      asm_error( "illegal argument type" );
      return 0;
    }
    code[ code_pos ].argflags = 0;
    code[ code_pos ].arg0 = 0;
    code[ code_pos ].arg1 = 0;
  }
  else if( syntax_table[ opcode ].num_args == 1 ) {
    if( arg1_type == ASMARG_NONE || arg2_type != ASMARG_NONE ) {
      asm_error( "illegal argument type" );
      return 0;
    }
    make_arg( (int *)code, arg1_type, arg1_val, &argflags1,
	      &code[ code_pos ].arg0 - (int *)code, arg1_offset );
    code[ code_pos ].argflags = argflags1;
    code[ code_pos ].arg1 = 0;
  }
  else {
    if( arg1_type == ASMARG_NONE || arg2_type == ASMARG_NONE ) {
      asm_error( "illegal argument type" );
      return 0;
    }
    make_arg( (int *)code, arg1_type, arg1_val, &argflags1,
	      &code[ code_pos ].arg0 - (int *)code, arg1_offset );
    make_arg( (int *)code, arg2_type, arg2_val, &argflags2,
	      &code[ code_pos ].arg1 - (int *)code, arg2_offset );
    code[ code_pos ].argflags = argflags1 | (argflags2 << ARG_SHIFT);
  }
  if( ++code_pos >= code_length ) {
    code_length += 10;
    code = myrealloc( code, code_length * sizeof( Instruction ) );
  }
  return 1;
}

void make_arg( int *code_ptr, int type, ArgType val, int *retflags, ptrdiff_t retarg_offset, int mem_offset )
{
  SymbolEntry *symbol_entry;

  switch( type ) {
  case ASMARG_IMMEDIATE: *retflags = ARG_IMMEDIATE; break;
  case ASMARG_NAMEIMM:   *retflags = ARG_IMMEDIATE; break;
  case ASMARG_ABSOLUTE:  *retflags = ARG_ABSOLUTE;  break;
  case ASMARG_INDIRECT:  *retflags = ARG_INDIRECT;  break;
  }

  switch( type ) {
  case ASMARG_IMMEDIATE:
    code_ptr[ retarg_offset ] = val.i;
    return;

  case ASMARG_NAMEIMM:
  case ASMARG_ABSOLUTE:
  case ASMARG_INDIRECT:
    symbol_entry = find_or_create_symbol_entry( val.s );
    list_add_to_tail_int( symbol_entry->refs, retarg_offset );
    code_ptr[ retarg_offset ] = mem_offset;
    break;

  default:
    fprintf( stderr, "Couldn't happen! Illegal argument type!\n" );
  }
}

int add_label( char *name )
{
  SymbolEntry *symbol_entry;

  symbol_entry = find_or_create_symbol_entry( name );
  if( symbol_entry->is_set ) {
    asm_error( "label already defined: %s", name );
    return 0;
  }
  if( symbol_entry->is_array ) {
    asm_error( "symbol already declared as an array: %s", name );
    return 0;
  }

  symbol_entry->is_set = 1;
  symbol_entry->value = code_pos;

  return 1;
}

int add_array( char *name, int size, List *init_list )
{
  SymbolEntry *symbol_entry;

  symbol_entry = find_or_create_symbol_entry( name );
  if( symbol_entry->is_set ) {
    asm_error( "symbol already declared as a label: %s", name );
    return 0;
  }
  if( symbol_entry->is_array ) {
    asm_error( "array already defined: %s", name );
    return 0;
  }

  symbol_entry->is_array = 1;
  symbol_entry->size = size;
  symbol_entry->array_values = init_list;

  return 1;
}


void add_define( char *name, int value )
{
  DefineEntry *tmp = mymalloc( sizeof( DefineEntry ) );

  tmp->value = value;
  hashtable_add_entry( define_table, name, tmp );
}


int get_define( char *name, int *error_ret )
{
  DefineEntry *tmp;

  if( (tmp = hashtable_get_entry( define_table, name )) == NULL ) {
    asm_error( "symbol not defined: %s", name );
    *error_ret = 1;
    return 0;
  }
  *error_ret = 0;
  return tmp->value;
}


static SymbolEntry *find_or_create_symbol_entry( char *name )
{
  SymbolEntry *ret;

  if( (ret = hashtable_get_entry( symbol_table, name )) == NULL ) {
    ret = mymalloc( sizeof( SymbolEntry ) );
    ret->refs = list_create();
    ret->is_set = 0;
    ret->value = 0;
    ret->is_array = 0;
    ret->size = 1;
    ret->array_values = NULL;
    hashtable_add_entry( symbol_table, name, ret );
  }
  return ret;
}


Program *close_program( void )
{
  SymbolEntry *symbol_entry;
  HashEntry *e;
  int ref;
  int label_ctr = 0;
  int val;
  Program *program;
  List *memory_init;
  int offset;
  int pos;
  List *init_data;
  int c;
  ArgType empty_type;

  empty_type.i = 0;
  add_instruction( "exit", ASMARG_NONE, empty_type, 0,
		   ASMARG_NONE, empty_type, 0 );

  if( !asm_ok ) {
    /* delete the symbol table */
    hashtable_init_walk( symbol_table );
    while( (e = hashtable_walk_next( symbol_table )) != NULL ) {
      myfree( e->data );
      hashtable_deleteEntry( symbol_table, e->key );
    }
    hashtable_delete( symbol_table );

    /* delete the define table */
    hashtable_init_walk( define_table );
    while( (e = hashtable_walk_next( define_table )) != NULL ) {
      myfree( e->data );
      hashtable_deleteEntry( define_table, e->key );
    }
    hashtable_delete( define_table );

    /* delete the memory allocated for code */
    myfree( code );

    return NULL;
  }

  program = mymalloc( sizeof( Program ) );
  memory_init = list_create();

  hashtable_init_walk( symbol_table );
  while( (e = hashtable_walk_next( symbol_table )) != NULL ) {
    symbol_entry = e->data;
    if( symbol_entry->is_set ) {
      val = symbol_entry->value;
    }
    else {
      val = label_ctr;
      label_ctr += symbol_entry->size;
      if( symbol_entry->array_values != NULL ) {
	/*
	 *  Here we know that the memory contents has to be
	 *  initialized with the contents of the list
	 *  in symbol_entry->array_values, but the memory hasn't
	 *  been allocated yet. The problem is that before the
	 *  memory is allocated the symbol table has been deleted.
	 *  In order to cope with this we put the initalization
	 *  array in a special list that is then used to initialize
	 *  the memory as soon as it has been created.
	 *
	 *  The format for the initialization list is pretty simple,
	 *  the first value in the list contains an offset to where
	 *  the data begins, so that value is first tacked on to
	 *  the head of the list.
	 */
	list_add_to_head_int( symbol_entry->array_values, val );
	list_add_to_tail_ptr( memory_init, symbol_entry->array_values );
      }
    }
    list_init_walk( symbol_entry->refs );
    while( list_walk_next_int( symbol_entry->refs, &ref ) ) {
      ((int *)code)[ ref ] += val;
    }
    list_delete( symbol_entry->refs );
    myfree( symbol_entry );
  }
  hashtable_delete( symbol_table );

  code = myrealloc( code, code_pos * sizeof( Instruction ) );
  program->code = code;
  program->code_size = code_pos;
  program->mem = mymalloc( label_ctr * sizeof( int ) );
  memset( program->mem, 0, label_ctr * sizeof( int ) );
  program->mem_size = label_ctr;
  program->pc = 0;

  /*
   *  Initialize the memory
   */
  list_init_walk( memory_init );
  while( (init_data = list_walk_next_ptr( memory_init )) != NULL ) {
    list_init_walk( init_data );
    list_walk_next_int( init_data, &offset );
    while( list_walk_next_int( init_data, &pos ) ) {
      program->mem[ offset++ ] = pos;
    }
    list_delete( init_data );
  }
  list_delete( memory_init );

  /*
   *  Set up the stacks
   */
  program->usrstack = stack_create();
  program->sysstack = stack_create();

  /*
   *  Clear the callback list
   */
  for( c = 0 ; c < NUM_CALLBACKS ; c++ ) {
    program->callbacks[ c ] = -1;
  }

  program->waiting_callbacks = queue_create();
  program->current_callback_values = NULL;

  /*
    *  Set up the debugging flag
    */
  program->debug = 0;

  return program;
}

void asm_error( char *format, ... )
{
  va_list ap;
  char str[ 1024 ];

  asm_ok = 0;
  va_start( ap, format );
  vsprintf( str, format, ap );
  log_print( "error in %s, line %d: %s", src_file, src_line, str );
  va_end( ap );
}
