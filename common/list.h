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

#ifndef LIST_H
#define LIST_H

typedef struct list_entry_struct {
  union {
    int i;
    void *ptr;
  } val;
  struct list_entry_struct *next;
  struct list_entry_struct *prev;
} ListEntry;

typedef struct list_struct {
  ListEntry *first;
  ListEntry *last;
  ListEntry *walk_ptr;
} List;


#define ListIsEmpty(list)		((list)->first == NULL)
#define ListHasOneEntry(list)		((list)->first != NULL && \
					 ((list)->first == (list)->last))
#define ListGetFirstPtr(list)		((list)->first->val.ptr)
#define ListGetFirstInt(list)		((list)->first->val.i)
#define ListGetLastPtr(list)		((list)->last->val.ptr)
#define ListGetLastInt(list)		((list)->last->val.i)
#define list_deleteFirst(list)		(list_deleteListEntry((list), \
							     (list)->first))
#define list_deleteLast(list)		(list_deleteListEntry((list),\
							     (list)->last))

List *list_create( void );
void list_delete( List * );
void list_deleteListEntry( List *, ListEntry * );
void list_deletePtr( List *, void * );
void list_add_to_head_int( List *, int );
void list_add_to_head_ptr( List *, void * );
void list_add_to_tail_int( List *, int );
void list_add_to_tail_ptr( List *, void * );
void list_add_before_ptr( List *, ListEntry *, void * );
void list_add_before_int( List *, ListEntry *, int );
void list_add_after_ptr( List *, ListEntry *, void * );
void list_add_after_int( List *, ListEntry *, int );
void list_init_walk( List * );
int list_walk_next_int( List *, int * );
void *list_walk_next_ptr( List * );
int list_size( List * );
int list_get_pos_int( List *, int );
void *list_get_pos_ptr( List *, int );

#endif
