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
#define ListDeleteFirst(list)		(ListDeleteListEntry((list), \
							     (list)->first))
#define ListDeleteLast(list)		(ListDeleteListEntry((list),\
							     (list)->last))

List *ListCreate( void );
void ListDelete( List * );
void ListDeleteListEntry( List *, ListEntry * );
void ListDeletePtr( List *, void * );
void ListAddToHeadInt( List *, int );
void ListAddToHeadPtr( List *, void * );
void ListAddToTailInt( List *, int );
void ListAddToTailPtr( List *, void * );
void ListAddBeforePtr( List *, ListEntry *, void * );
void ListAddBeforeInt( List *, ListEntry *, int );
void ListAddAfterPtr( List *, ListEntry *, void * );
void ListAddAfterInt( List *, ListEntry *, int );
void ListInitWalk( List * );
int ListWalkNextInt( List *, int * );
void *ListWalkNextPtr( List * );
int ListSize( List * );
int ListGetPosInt( List *, int );
void *ListGetPosPtr( List *, int );

#endif
