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
#include <stdarg.h>

#include "mem.h"
#include "lang.h"
#include "bullet.h"
#include "comms.h"
#include "prototypes.h"

typedef void LogFunc( char *, ... );
extern LogFunc *log_print;

int debug_info = 1;

extern Robot *current_robot;

#define POPVAR(X)	int X = (StackGetSize(program->usrstack) ?     \
				 stack_pop_int(program->usrstack) : 0)

#define CHECK_PC(PC)				\
	do {					\
	  if( !check_pc( program, (PC) ) ) {	\
	    return 0;				\
	  }					\
	} while( 0 )

static int check_pc( Program *program, int pc )
{
  if( pc < 0 || pc >= program->code_size ) {
    runtime_warning( current_robot, "jump outside program: %d\n", pc );
    return 0;
  }
  return 1;
}

#include "oper_functions.c"


#define GET_MEM(PROG,POS,ARG)						\
	do {								\
	  if( (POS) < 0 || (POS) >= (PROG)->mem_size ) {		\
	    fprintf( stderr, "access outside memory: %d\n", (POS) );	\
	    return 0;							\
	  }								\
	  *(ARG) = &(PROG)->mem[ (POS) ];				\
	} while( 0 )

#define GET_ARGX(NUM,PROG,INSTR,ARG)					\
	do {								\
	  int type = ((INSTR)->argflags & (ARG_MASK <<			\
					   (ARG_SHIFT * (NUM)))) >>	\
	    (ARG_SHIFT * (NUM));					\
	  switch( type ) {						\
	  case ARG_IMMEDIATE:						\
	    *(ARG) = &(INSTR)->arg ## NUM;				\
	    break;							\
	  case ARG_ABSOLUTE:						\
	    GET_MEM( (PROG),  (INSTR)->arg ## NUM, (ARG) );		\
	    break;							\
	  case ARG_INDIRECT:						\
	    {								\
	      int *tmp;							\
	      GET_MEM( (PROG), (INSTR)->arg ## NUM, &tmp );		\
	      GET_MEM( (PROG), *tmp, (ARG) );				\
	    }								\
	    break;							\
	  }								\
	} while( 0 )


int evaluate_instruction( Program *program )
{
  int *arg0, *arg1;
  Instruction *instruction = &program->code[ program->pc ];

  GET_ARGX( 0, program, instruction, &arg0 );
  GET_ARGX( 1, program, instruction, &arg1 );
  if( instruction->opcode < 0 ||
      instruction->opcode >= sizeof(code_tab) / sizeof(CodeOperation *) ) {
    fprintf( stderr, "Illegal opcode: %d\n", instruction->opcode );
    exit( 1 );
  }
  program->pc++;
  return code_tab[ instruction->opcode ]( program, arg0, arg1 );
}


CallbackValues *make_cbvalues( Program *program, int callback_number )
{
  CallbackValues *cbvalues;

  if( program->callbacks[ callback_number ] == -1 ) {
    return NULL;
  }
  cbvalues = mymalloc( sizeof( CallbackValues ) );
  cbvalues->callback_number = callback_number;
  
  return cbvalues;
}

void call_callback( Program *program, CallbackValues *cbvalues )
{
  if( program->callbacks[ cbvalues->callback_number ] != -1 ) {
    if( cbvalues->callback_number < 0 ||
	cbvalues->callback_number >= NUM_CALLBACKS ) {
      fprintf( stderr, "illegal callback\n" );
      exit( 1 );
    }
    if( program->current_callback_values == NULL ) {
      stack_push_int( program->sysstack, program->pc );
      program->pc = program->callbacks[ cbvalues->callback_number ];
      program->current_callback_values = cbvalues;
    }
    else {
      queue_push_ptr( program->waiting_callbacks, cbvalues );
    }
  }
  else {
    myfree( cbvalues );
  }
}


void make_cbvalues_call_callback( Program *program, int callback_number, ... )
{
  va_list ap;
  int c;
  CallbackValues *cbvalues;
  static int callback_num_args[] = { 1, /* CB_TAKE_DAMAGE */
				     7, /* CB_ENEMY_FOUND */
				     0, /* CB_ENEMY_NOT_FOUND */
				     3, /* CB_WALL_FOUND */
				     0, /* CB_WALL_NOT_FOUND */
				     0, /* CB_COLLIDE_WALL */
				     3, /* CB_MESSAGE_RECEIVED */
				     1, /* CB_TIMEOUT */
  };


  if( (cbvalues = make_cbvalues( program, callback_number )) != NULL ) {
    va_start( ap, callback_number );
    for( c = 0 ; c < callback_num_args[ callback_number ] ; c++ ) {
      cbvalues->data[ c ] = va_arg( ap, int );
    }
    call_callback( program, cbvalues );
    va_end( ap );
  }
}


void runtime_warning( Robot *robot, char *format, ... )
{
  va_list ap;
  char str[ 1024 ];

  if( debug_info ) {
    va_start( ap, format );
    vsprintf( str, format, ap );
    log_print( "runtime error: robot: %d: %s", robot->id, str );
    va_end( ap );
  }
}
