#    robot assembler - robot assembly language
#    Copyright (C) 1998  Elias Martenson (elias.martenson@sun.se)
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

CFLAGS += -I.. -I../common $(GTKCFLAGS)
LDFLAGS += -L../common $(GTKLDFLAGS)

LIB = libgtkgui.a

OBJS = nobotsgtk.o interface.o support.o file_select.o robot_list_table.o

all:		$(LIB)

$(LIB):		$(OBJS)
		$(AR) $(LIB) $(OBJS)

clean:
		rm -f $(LIB) *.o
