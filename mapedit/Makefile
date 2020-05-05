#    robot assembler - robot assembly language
#    Copyright (C) 1996-2020  Elias Martenson (lokedhs@gmail.com)
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

EXEC = mapedit

CFLAGS += -I.. -I../common $(XCFLAGS)
LDFLAGS += -L../common $(XLDFLAGS)
EXTRALIBS = -lcommon -lXm -lXpm -lXt -lXext -lSM -lICE -lX11 -lm

OBJS = editwin.o grid_options.o mode_options.o file_select.o edit_ops.o \
	mapcommon.o

all:		$(EXEC)

$(EXEC):	$(OBJS)
		$(CC) $(LDFLAGS) -o $(EXEC) $(OBJS) $(EXTRALIBS) $(LIBS)

mapcommon.o:
		$(CC) $(CFLAGS) -c ../mapcommon.c -o mapcommon.o

clean:
		rm -f $(EXEC) *.o
