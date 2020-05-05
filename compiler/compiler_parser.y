%{
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

#include <string.h>
#include <stdlib.h>

#include "mem.h"
#include "list.h"
#include "stack.h"
#include "hash.h"
#include "compiler.h"


List *dup_strings;
Stack *parsestk;
int tmp_level = 0;
HashTable *funcs;
char *current_func = NULL;
char *current_func_end = NULL;

char src_file[ 1024 ] = "unknown";
int src_line = 1;

void yyerror( char * );

%}

%union {
  int i;
  char s[ 1024 ];
  Variable v;
  List *l;
}

%token ARRAY
%token FUNC
%token CFUNC
%token SAFE
%token CALLBACK
%token GETCALLBACK
%token PRINT
%token WHILE
%token IF
%token ELSE
%token RANDOM
%token GETCB
%token RETURN
%token <s> SYMBOL
%token <i> INTEGER
%token <s> ROBOTCOM_RETZERO
%token <s> ROBOTCOM_RETONE
%type  <v> expression
%type  <v> lvalue
%type  <i> optional_size
%type  <l> init_list
%type  <l> arglist
%type  <l> call_arglist
%type  <i> func_or_cfunc

%right '='
%right SETADD SETSUB SETMUL SETDIV
%left AND OR
%left '<' '>' LE GE EQ NE
%left '+' '-'
%left '*' '/' '%'
%left NEG
%left '!'
%left INC DEC
%left '&'
%left NAMEIM


%%

start:		{
		  dup_strings = list_create();
		  parsestk = stack_create();
		  funcs = hashtable_create( NULL, 0 );
                } input {
                  clean_statement();
		  stack_delete( parsestk );
		  list_delete( dup_strings );
		  hashtable_delete( funcs );
		}
		;

input:		  statement_list
		;

statement:	  newstatement
			{
			  clean_statement();
			}
		;

newstatement:	  PRINT '(' expression ')' ';'
			{
			  addinstr1( "print", $3 );
			}

		| ARRAY SYMBOL optional_size init_list ';'
			{
			  add_array( $2, $3, $4 );
			  if( $4 != NULL ) {
			    list_delete( $4 );
			  }
			}

		| func_or_cfunc SYMBOL '(' arglist ')'
			{
			  char *arg;
			  Variable *var;
			  List *save_vars = list_create();
			  List *tmp_vars = list_create();

			  if( current_func != NULL ) {
			    compiler_error( "cannot declare function "
					    "inside another function" );
			    YYERROR;
			  }

			  current_func = mystrdup( $2 );
			  stack_push_ptr( parsestk, mk_label() );
			  current_func_end = mystrdup( mk_label() );

			  /* jump over the function */
			  addinstr1( "jmp",
				     MK_ABS( stack_look_ptr( parsestk, 0 ) ) );
			  add_label( $2 );

			  /* pop the arguments off the stack */
			  list_init_walk( $4 );
			  while( (arg = list_walk_next_ptr( $4 )) != NULL ) {
			    var = mymalloc( sizeof( Variable ) );
			    *var = newvar();
			    addinstr1( "pop", *var );
			    list_add_to_tail_ptr( tmp_vars, var );
			    list_add_to_tail_ptr( save_vars, arg );
			  }

			  /* add the list of local variables here */

			  /* push the arguments to be saved */
			  list_init_walk( save_vars );
			  while( (arg = list_walk_next_ptr( save_vars )) != NULL ) {
			    addinstr1( "push", MK_SYM( arg ) );
			  }			  

			  /* move the temporary to the real vairables */
			  list_init_walk( $4 );
			  list_init_walk( tmp_vars );
			  while( (arg = list_walk_next_ptr( $4 )) != NULL ) {
			    var = list_walk_next_ptr( tmp_vars );
			    addinstr2( "move", *var, MK_SYM( arg ) );
			    myfree( var );
			  }
			  list_delete( tmp_vars );
			  list_delete( $4 );

			  /* push the list of save variables */
			  stack_push_ptr( parsestk, save_vars );

			}
		  '{' statement_list '}'
			{
			  List *tmp_vars;
			  ListEntry *w;

			  add_label( current_func_end );
			  myfree( current_func_end );

			  /* restore the saved variables */
			  tmp_vars = stack_pop_ptr( parsestk );
			  w = tmp_vars->last;
			  while( w != NULL ) {
			    addinstr1( "pop", MK_SYM( w->val.ptr ) );
			    myfree( w->val.ptr );
			    w = w->prev;
			  }
			  list_delete( tmp_vars );

			  if( $1 == 0 ) {
			    addinstr0( "rts" );
			  }
			  else {
			    addinstr0( "rtc" );
			  }
			  add_label( stack_look_ptr( parsestk, 0 ) );
			  myfree( stack_pop_ptr( parsestk ) );

			  myfree( current_func );
			  current_func = NULL;
			}

		| SAFE SYMBOL ';'
			{
			  hashtable_add_entry( funcs, $2, (void *)1 );
			}

		| RETURN expression ';'
			{
			  if( current_func == NULL ) {
			    compiler_error( "return outside function" );
			    YYERROR;
			  }
			  addinstr2( "move", $2, MK_SYM( "return_val" ) );
			  addinstr1( "jmp", MK_ABS( current_func_end ) );
			}

		| expression ';'
			{
			  /* no default action */
			}

		| '{' statement_list '}'

		| WHILE '('
			{
			  stack_push_ptr( parsestk, mk_label() );
			  stack_push_ptr( parsestk, mk_label() );
			  add_label( stack_look_ptr( parsestk, 1 ) );
			}
		  expression ')'
			{
			  addinstr2( "jeq", $4,
				     MK_ABS( stack_look_ptr( parsestk, 0 ) ) );
			}
		  statement
			{
			  addinstr1( "jmp",
				     MK_ABS( stack_look_ptr( parsestk, 1 ) ) );
			  add_label( stack_look_ptr( parsestk, 0 ) );
			  myfree( stack_pop_ptr( parsestk ) );
			  myfree( stack_pop_ptr( parsestk ) );
			}

		| IF '(' expression ')'
			{
			  stack_push_ptr( parsestk, mk_label() );
			  addinstr2( "jeq", $3,
				     MK_ABS( stack_look_ptr( parsestk, 0 ) ) );
			}
		  if_clauses

		| ROBOTCOM_RETZERO '(' robotcom_arglist ')' ';'
			{
			  addinstr0( $1 );
			}

		| CALLBACK '(' expression ',' expression ')' ';'
			{
			  addinstr2( "callback", $3, $5 );
			}

		| ';'
			{
			  /* no default */
			}

		| error

		;

if_clauses:	  statement
			{
			  add_label( stack_look_ptr( parsestk, 0 ) );
			  myfree( stack_pop_ptr( parsestk ) );
			}

		| statement ELSE
			{
			  stack_push_ptr( parsestk, mk_label() );
			  addinstr1( "jmp",
				     MK_ABS( stack_look_ptr( parsestk, 0 ) ) );
			  add_label( stack_look_ptr( parsestk, 1 ) );
			}
		  statement
			{
			  add_label( stack_look_ptr( parsestk, 0 ) );
			  myfree( stack_pop_ptr( parsestk ) );
			  myfree( stack_pop_ptr( parsestk ) );
			}

		;


arglist:		{
			  stack_push_ptr( parsestk, list_create() );
			}
		  arglist_values_or_none
			{
			  $$ = stack_pop_ptr( parsestk );
			}

		;

arglist_values_or_none:	  /* nothing */
			| arglist_values
			;

arglist_values:	  arglist_val
		| arglist_values ',' arglist_val
		;

arglist_val:	  SYMBOL
			{
			  list_add_to_tail_ptr( stack_look_ptr( parsestk, 0 ),
					    mystrdup( $1 ) );
			}
		;

statement_list:	  /* empty */
		| statement_list statement
		;

expression:	  INTEGER
			{
			  $$ = MK_INT( $1 );
			}

		| lvalue
			{
			  $$ = $1;
			}

		| '%' SYMBOL %prec NAMEIM
			{
			  $$ = MK_ABS( $2 );
			}

		| '(' expression ')'
			{
			  $$ = $2;
			}

		| expression '+' expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "add", $3, $$ );
			}

		| expression '-' expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "sub", $3, $$ );
			}

		| expression '*' expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "mul", $3, $$ );
			}

		| expression '/' expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "div", $3, $$ );
			}


		| '-' expression %prec NEG
			{
			  $$ = replvar( $2 );
			  addinstr1( "neg", $$ );
			}

		| expression '%' expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "mod", $3, $$ );
			}

		| lvalue '=' expression
			{
			  addinstr2( "move", $3, $1 );
			  $$ = $1;
			}

		| INC lvalue
			{
			  addinstr2( "add", MK_INT( 1 ), $2 );
			  $$ = $2;
			}

		| lvalue INC
			{
			  $$ = replvar( $1 );
			  addinstr2( "add", MK_INT( 1 ), $1 );
			}

		| DEC lvalue
			{
			  addinstr2( "sub", MK_INT( 1 ), $2 );
			  $$ = $2;
			}

		| lvalue DEC
			{
			  $$ = replvar( $1 );
			  addinstr2( "sub", MK_INT( 1 ), $1 );
			}

		| expression EQ expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "tsteq", $3, $$ );
			}

		| expression NE expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "tstne", $3, $$ );
			}

		| expression '>' expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "tstgt", $3, $$ );
			}

		| expression '<' expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "tstlt", $3, $$ );
			}

		| expression GE expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "tstge", $3, $$ );
			}

		| expression LE expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "tstle", $3, $$ );
			}

		| '!' expression
			{
			  $$ = replvar( $2 );
			  addinstr1( "not", $$ );
			}

		| expression AND expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "tstand", $3, $$ );
			}

		| expression OR expression
			{
			  $$ = replvar( $1 );
			  addinstr2( "tstor", $3, $$ );
			}

		| lvalue SETADD expression
			{
			  addinstr2( "add", $3, $1 );
			  $$ = $1;
			}

		| lvalue SETSUB expression
			{
			  addinstr2( "sub", $3, $1 );
			  $$ = $1;
			}

		| lvalue SETMUL expression
			{
			  addinstr2( "mul", $3, $1 );
			  $$ = $1;
			}

		| lvalue SETDIV expression
			{
			  addinstr2( "div", $3, $1 );
			  $$ = $1;
			}

		| RANDOM
			{
			  $$ = newvar();
			  addinstr1( "random", $$ );
			}

		| SYMBOL '(' call_arglist ')'
			{
			  int to_save = (hashtable_get_entry( funcs, $1 ) != NULL);
			  Variable *var;
			  if( to_save ) {
			    save_tmpvars();
			  }
			  list_init_walk( $3 );
			  while( (var = list_walk_next_ptr( $3 )) != NULL ) {
			    addinstr1( "push", *var );
			    myfree( var );
			  }
			  list_delete( $3 );
			  addinstr1( "jsr", MK_ABS( $1 ) );
			  if( to_save ) {
			    restore_tmpvars();
			  }
			  $$ = MK_SYM( lstrdup( "return_val" ) );
			}

		| ROBOTCOM_RETONE '(' robotcom_arglist ')'
			{
			  addinstr0( $1 );
			  $$ = newvar();
			  addinstr1( "pop", $$ );
			}

		| GETCALLBACK '(' expression ')'
			{
			  $$ = newvar();
			  addinstr2( "getcallback", $3, $$ );
			}

		| GETCB '(' expression ')'
			{
			  $$ = newvar();
			  addinstr2( "getcb", $3, $$ );
			}
		;


robotcom_arglist:	{
			  stack_push_ptr( parsestk, list_create() );
			}
		      robotcom_arglist_none
			{
			  Variable *var;
			  list_init_walk( stack_look_ptr( parsestk, 0 ) );
			  while( (var = list_walk_next_ptr( stack_look_ptr( parsestk, 0 ) ) ) != NULL ) {
			    addinstr1( "push", *var );
			    myfree( var );
			  }
			  list_delete( stack_pop_ptr( parsestk ) );
			}
			;
			  
robotcom_arglist_none:	  /* nothing */
			| robotcom_arglist_one
			;

robotcom_arglist_one:	  robotcom_arglist_value
			| robotcom_arglist_one ',' robotcom_arglist_value
			;

robotcom_arglist_value:	  expression
				{
				  Variable *var = mymalloc( sizeof( Variable ) );
				  *var = $1;
				  list_add_to_head_ptr( stack_look_ptr( parsestk, 0 ),
						    var );
				}
			;


call_arglist:		{
			  stack_push_ptr( parsestk, list_create() );
			}
		  call_arglist_none
			{
			  $$ = stack_pop_ptr( parsestk );
			}
		;

call_arglist_none:	  /* nothing */
			| call_arglist_one
			;

call_arglist_one:	  call_arglist_value
			| call_arglist_one ',' call_arglist_value
			;

call_arglist_value:	  expression
				{
				  Variable *var = mymalloc( sizeof( Variable ) );
				  *var = $1;
				  list_add_to_head_ptr( stack_look_ptr( parsestk, 0 ),
						    var );
				}
			;

lvalue:		  SYMBOL
			{
			  $$ = MK_SYM( lstrdup( $1 ) );
			}

		| '&' expression
			{
			  $$ = MK_REF( lstrdup( var_to_string( $2 ) ) );
			}

		;

optional_size:	  /* nothing */
			{
			  $$ = 0;
			}

		| ':' INTEGER
			{
			  $$ = $2;
			}

		;

init_list:	  /* nothing */
			{
			  $$ = NULL;
			}

		| '['
			{
			  stack_push_ptr( parsestk, list_create() );
			}
		  init_list_vallist ']'
			{
			  $$ = stack_pop_ptr( parsestk );
			}
		;

init_list_vallist:	  init_list_int
			| init_list_vallist ',' init_list_int
			;

init_list_int:	  INTEGER
			{
			  list_add_to_tail_int( stack_look_ptr( parsestk, 0 ), $1 );
			}
		;

func_or_cfunc:	  FUNC
			{
			  $$ = 0;
			}

		| CFUNC
			{
			  $$ = 1;
			}

		;



%%

void yyerror( char *str )
{
  compiler_error( "%s", str );
}

int yywrap( void )
{
  return 1;
}


#include "compiler_lexer.c"
