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

#ifndef COMPILER_H
#define COMPILER_H

#include "list.h"

#define VAR_TYPE_INT		0
#define VAR_TYPE_SYM		1
#define VAR_TYPE_TMPSYM		2
#define VAR_TYPE_REF		3
#define VAR_TYPE_ABS		4

#define MK_INT(X)	mk_int(X)
#define MK_ABS(X)	mk_abs(X)
#define MK_SYM(X)	mk_sym(X)
#define MK_REF(X)	mk_ref(X)


typedef struct {
  int type;
  union {
    int i;
    char *s;
  } value;
} Variable;

void cf( char *, ... );
void addinstr0( char * );
void addinstr1( char *, Variable );
void addinstr2( char *, Variable, Variable );
Variable mk_int( int );
Variable mk_abs( char * );
Variable mk_sym( char * );
Variable mk_ref( char * );
char *mk_label( void );
void add_label( char * );
void add_array( char *, int, List * );
char *var_to_string( Variable );
Variable replvar( Variable );
Variable newvar( void );
char *get_tmpvar_name( int );
void save_tmpvars( void );
void restore_tmpvars( void );
char *lstrdup( char * );
void clean_statement( void );
void compiler_error( char *, ... );

#endif
