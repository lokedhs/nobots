/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1998  Elias Martenson (elias.martenson@sun.se)
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

#ifndef BATWIN_H
#define BATWIN_H

typedef struct {
  Widget batwin_shell;
  Widget drawarea;
  Widget scrolled_win;
  Widget horiz_scrollbar, vert_scrollbar;
  Widget clip_window;
  Widget work_window;
  int x, y;
  int width, height;
  int visible_width, visible_height;
  double xdiv, ydiv;
  int follow;
  GC draw_gc;
  GC scanner_gc;
#ifdef USE_DOUBLE_BUFFER_EXTENSION
  XdbeBackBuffer dbe_back_buffer;
#endif
#ifdef USE_MULTI_BUFFER_EXTENSION
  Multibuffer buffers[ 2 ];
  int current_buffer;
#endif
  Drawable out_buffer;
  int battle_window_id;
} BattleWindow;

#define MAX_BATWINS			10

#define MULTIBUFFER_METHOD_NONE		0
#define MULTIBUFFER_METHOD_DBLBUF	1
#define MULTIBUFFER_METHOD_MBUF		2

#endif
