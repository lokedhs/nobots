#    robot assembler - robot assembly language
#    Copyright (C) 1996-2002  Elias Martenson (elias-m@algonet.se)
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

include ../Makefile.global

EXEC = compiler

CFLAGS += -I../common
LDFLAGS += -L../common
EXTRALIBS = -lcommon

OBJS = compiler.o compiler_parser.o

all:		$(EXEC)

$(EXEC):	$(OBJS)
		$(CC) $(LDFLAGS) -o $(EXEC) $(OBJS) $(EXTRALIBS) $(LIBS)

compiler_parser.c:	compiler_parser.y compiler_lexer.c
		$(YACC) compiler_parser.y
		mv y.tab.c $@

compiler_lexer.c:	compiler_lexer.l	
		$(LEX) compiler_lexer.l
		mv lex.yy.c $@

clean:
		rm -f $(EXEC) *.o compiler_parser.c compiler_lexer.c
