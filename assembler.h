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

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>

#include "list.h"
#include "lang.h"

typedef union {
  int i;
  char *s;
} ArgType;

#define ASMARG_IMMEDIATE	0
#define ASMARG_ABSOLUTE		1
#define ASMARG_INDIRECT		2
#define ASMARG_NAMEIMM		3
#define ASMARG_NONE		4

Program *new_program( char * );
Program *new_program_fd( FILE *, int );
int add_instruction( char *,
		     int, ArgType, int,
		     int, ArgType, int );
int add_label( char * );
int add_array( char *, int, List * );
void add_define( char *, int );
int get_define( char *, int * );
Program *close_program( void );
void asm_error( char *, ... );

#endif
