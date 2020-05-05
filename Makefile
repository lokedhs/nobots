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

include Makefile.global

VERSION = 0.20

MOTIFEXEC = nobotsmotif
GTKEXEC = nobotsgtk

OBJS = execute.o evaluate.o assembler.o parser.o robot.o direction.o deffile.o \
	scan.o octree.o bullet.o robot_move.o weapon.o map.o mapcommon.o \
	comms.o robotlist.o simulation.o

MOTIFGUILDFLAGS = -Lgui
MOTIFGUILIB = -lgui

GTKGUILDFLAGS = -Lgtk
GTKGUILIB = -lgtkgui

EXTRALIBS = -lcommon
CFLAGS += -Icommon -I.
LDFLAGS += -Lcommon
SUBDIRS = common gui gtk compiler mapedit

LIBS = -lm

all:		$(GTKEXEC)

$(MOTIFEXEC):	$(OBJS) makegui makecommon
		$(CC) $(LDFLAGS) $(MOTIFGUILDFLAGS) -o $(MOTIFEXEC) $(OBJS) $(MOTIFGUILIB) $(EXTRALIBS) $(XLDFLAGS) $(LIBS)

$(GTKEXEC):	$(OBJS) makegtk makecommon
		$(CC) $(LDFLAGS) $(GTKCFLAGS) $(GTKGUILDFLAGS) -o $(GTKEXEC) $(OBJS) $(GTKGUILIB) $(EXTRALIBS) $(GTKLDFLAGS) $(LIBS)

parser.c:	parser.y lexer.c
		$(YACC) parser.y
		mv y.tab.c $@

lexer.c:	lexer.l
		$(LEX) lexer.l
		mv lex.yy.c $@

evaluate.o:	oper_functions.c

assembler.o:	oper_syntax.c

oper_functions.c oper_syntax.c:	make_oper.pl operations.def movement.def \
			comms.def misc.def
		$(PERL) make_oper.pl

gui/libgui.a:	makegui

makegui:
		(cd gui ; $(MAKE))

makegtk:
		(cd gtk ; $(MAKE))

common/libcommon.a: makecommon

makecommon:
		(cd common ; $(MAKE))

clean:
		rm -f $(MOTIFEXEC) $(GTKEXEC) *.o parser.c lexer.c oper_functions.c
		rm -f oper_syntax.c
		(for dir in $(SUBDIRS) ; do ( cd $$dir ; $(MAKE) clean ) ; done)

dist:
		(robot_dir=`pwd` ; cd .. ; tar cf - `basename $$robot_dir` | (cd /tmp ; tar xvf -) ; mv /tmp/`basename $$robot_dir` $$robot_dir/nobots-$(VERSION))
		(cd nobots-$(VERSION) ; $(MAKE) clean ; find . \( -name core -o -name "*~" \) -print | xargs rm -f)
		tar cvf nobots-$(VERSION).tar nobots-$(VERSION)
		gzip -9 nobots-$(VERSION).tar
		rm -fr nobots-$(VERSION)
