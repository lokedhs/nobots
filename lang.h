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

#ifndef LANG_H
#define LANG_H

#include "stack.h"
#include "circbuffer.h"

#define NUM_CALLBACKS		256
#define MAX_CALLBACK_DATA	64

typedef struct {
  int callback_number;
  int data[ MAX_CALLBACK_DATA ];
} CallbackValues;



typedef struct {
  int opcode;			/* opcode */
  int argflags;			/* argument types and flags */
  int arg0;			/* first argument */
  int arg1;			/* second argument */
} Instruction;

typedef struct {
  Instruction *code;		/* program code */
  int code_size;		/* size of code */
  int pc;			/* program counter */
  int *mem;			/* memory area */
  int mem_size;			/* size of memory area */
  Stack *usrstack;		/* user stack */
  Stack *sysstack;		/* system stack (for jsr) */
  int callbacks[ NUM_CALLBACKS ]; /* list of callbacks */
  Circbuffer *waiting_callbacks; /* list of callbacks waiting to be executed */
  CallbackValues *current_callback_values; /* current callback values */
  int debug;			/* debugger active */
} Program;

#define ARG_MASK		0x03
#define ARG_IMMEDIATE		0x00
#define ARG_ABSOLUTE		0x01
#define ARG_INDIRECT		0x02
#define ARG_SHIFT		0x02

#define ARG1_IMMEDIATE		ARG_IMMEDIATE
#define ARG1_ABSOLUTE		ARG_ABSOLUTE
#define ARG1_INDIRECT		ARG_INDIRECT
#define ARG2_IMMEDIATE		(ARG_IMMEDIATE << ARG_SHIFT)
#define ARG2_ABSOLUTE		(ARG_ABSOLUTE << ARG_SHIFT)
#define ARG2_INDIRECT		(ARG_INDIRECT << ARG_SHIFT)

#endif
