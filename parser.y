%{
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
#include <stdlib.h>

#include "mem.h"
#include "lang.h"
#include "assembler.h"
#include "list.h"

typedef struct {
	int argtype;
	ArgType val;
	int offset;
} ParsArgType;

List *dup_strings;
List *array_init_list;
int array_init_size;

void yyerror( char * );
char *lstrdup( char * );
void clean_strings( void );
%}

%union {
	int i;
	char s[ 1024 ];
	ParsArgType a;
}

%token <s> SYMBOL
%token <s> IMMSYMBOL
%token <s> INDSYMBOL
%token <i> INTEGER
%token ARRAY
%token DEFINE
%token STMT_SEP
%type  <a> argument
%type  <i> integer
%type  <i> intexpr
%type  <i> optional_int
%type  <i> optional_offset

%left '+' '-'
%left '*' '/'
%left NEG


%%

start:		{ dup_strings = list_create(); } input {
					clean_strings();
					list_delete( dup_strings );
				}
		;

input:		  /* empty */
		| input statement
		;

statement:	  labeldef statement
		| instruction statement_sep
		| declaration statement_sep
		| error statement_sep
		;

labeldef:	  SYMBOL ':'	{
					add_label( $1 );
				}
		;

instruction:	  /* empty */
		| SYMBOL argument ',' argument
				{
					add_instruction( $1,
							 $2.argtype, $2.val,
								$2.offset,
							 $4.argtype, $4.val,
								$4.offset );
				}

		| SYMBOL argument
				{
					ArgType empty_type;
					empty_type.i = 0;
					add_instruction( $1, $2.argtype, $2.val,
								$2.offset,
							 ASMARG_NONE, empty_type, 0 );
				}

		| SYMBOL
				{
					ArgType empty_type;
					empty_type.i = 0;
					add_instruction( $1, ASMARG_NONE, empty_type, 0,
							 ASMARG_NONE, empty_type, 0 );
				}
		;

argument:	  integer	{
					$$.argtype = ASMARG_IMMEDIATE;
					$$.val.i = $1;
				}

		| SYMBOL optional_offset
				{
					$$.argtype = ASMARG_ABSOLUTE;
					$$.val.s = lstrdup($1);
					$$.offset = $2;
				}

		| IMMSYMBOL optional_offset
				{
					$$.argtype = ASMARG_NAMEIMM;
					$$.val.s = lstrdup($1);
					$$.offset = $2;
				}

		| INDSYMBOL optional_offset
				{
					$$.argtype = ASMARG_INDIRECT;
					$$.val.s = lstrdup($1);
					$$.offset = $2;
				}
		;


optional_offset:  /* nothing */	{
					$$ = 0;
				}

		| '/' integer	{
					$$ = $2;
				}
		;


integer:	  INTEGER	{
					$$ = $1;
				}

		| '(' intexpr ')'
				{
					$$ = $2;
				}
		;

intexpr:	  INTEGER	{
					$$ = $1;
				}

		| '(' intexpr ')'
				{
					$$ = $2;
				}

		| intexpr '+' intexpr
				{
					$$ = $1 + $3;
				}

		| intexpr '-' intexpr
				{
					$$ = $1 - $3;
				}

		| intexpr '*' intexpr
				{
					$$ = $1 * $3;
				}

		| intexpr '/' intexpr
				{
					$$ = $1 / $3;
				}

		| '-' intexpr %prec NEG
				{
					$$ = -$2;
				}

		| SYMBOL	{
					int error_ret;
					$$ = get_define( $1, &error_ret );
					if( error_ret ) {
					  YYERROR;
					}
				}
		;

declaration:	  ARRAY SYMBOL optional_int '['
				{
					array_init_list = list_create();
					array_init_size = 0;
				}
			initlist ']'
				{
					if( $3 != 0 && array_init_size > $3 ) {
					  asm_error( "array size specifier (%d) "
						     "is smaller than list "
						     "length (%d)", $3,
						     array_init_size );
					}
					else {
					  if( $3 > array_init_size ) {
					    array_init_size = $3;
					  }
					  add_array( $2, array_init_size,
						     array_init_list );
					}
				}

		| ARRAY SYMBOL ':' integer
				{
					add_array( $2, $4, NULL );
				}

		| DEFINE SYMBOL '=' integer
				{
					add_define( $2, $4 );
				}
		;


initlist:	  initlist_int
		| initlist ',' initlist_int
		;

initlist_int:	  integer	{
					list_add_to_tail_int( array_init_list, $1 );
					array_init_size++;
				}
		;


optional_int:	  /* nothing */	{
					$$ = 0;
				}

		| ':' integer	{
					$$ = $2;
				}
		;
		

statement_sep:	  STMT_SEP	{
					clean_strings();
				}
		;

%%

void yyerror( char *str )
{
	asm_error( "%s", str );
}

int yywrap( void )
{
	return 1;
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

void clean_strings( void )
{
	while( dup_strings->first != NULL ) {
		myfree( dup_strings->last->val.ptr );
		list_delete_list_entry( dup_strings, dup_strings->last );
	}
}


#include "lexer.c"
