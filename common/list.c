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

#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "list.h"

static void ListAddToHead( List *, ListEntry * );
static void ListAddToTail( List *, ListEntry * );
static void ListAddBefore( List *, ListEntry *, ListEntry * );
static void ListAddAfter( List *, ListEntry *, ListEntry * );

List *ListCreate( void )
{
  List *ret = mymalloc( sizeof( List ) );

  ret->first = NULL;
  ret->last = NULL;
  return ret;
}

void ListDelete( List *list )
{
  ListEntry *w = list->first;
  ListEntry *wnext;

  while( w != NULL ) {
    wnext = w->next;
    myfree( w );
    w = wnext;
  }
  myfree( list );
}

void ListAddToHeadInt( List *list, int val )
{
  ListEntry *entry = mymalloc( sizeof( ListEntry ) );

  entry->val.i = val;
  ListAddToHead( list, entry );
}

void ListAddToHeadPtr( List *list, void *val )
{
  ListEntry *entry = mymalloc( sizeof( ListEntry ) );

  entry->val.ptr = val;
  ListAddToHead( list, entry );
}

static void ListAddToHead( List *list, ListEntry *entry )
{
  entry->prev = NULL;
  if( list->first == NULL ) {
    list->first = entry;
    entry->next = NULL;
    list->last = entry;
  }
  else {
    entry->next = list->first;
    list->first->prev = entry;
    list->first = entry;
  }
}

void ListAddToTailInt( List *list, int val )
{
  ListEntry *entry = mymalloc( sizeof( ListEntry ) );

  entry->val.i = val;
  ListAddToTail( list, entry );
}

void ListAddToTailPtr( List *list, void *val )
{
  ListEntry *entry = mymalloc( sizeof( ListEntry ) );

  entry->val.ptr = val;
  ListAddToTail( list, entry );
}

void ListAddBeforePtr( List *list, ListEntry *entry, void *val )
{
  ListEntry *newentry = mymalloc( sizeof( ListEntry ) );

  entry->val.ptr = val;
  ListAddBefore( list, entry, newentry );
}

void ListAddBeforeInt( List *list, ListEntry *entry, int val )
{
  ListEntry *newentry = mymalloc( sizeof( ListEntry ) );

  entry->val.i = val;
  ListAddBefore( list, entry, newentry );
}

void ListAddAfterPtr( List *list, ListEntry *entry, void *val )
{
  ListEntry *newentry = mymalloc( sizeof( ListEntry ) );

  entry->val.ptr = val;
  ListAddAfter( list, entry, newentry );
}

void ListAddAfterInt( List *list, ListEntry *entry, int val )
{
  ListEntry *newentry = mymalloc( sizeof( ListEntry ) );

  entry->val.i = val;
  ListAddAfter( list, entry, newentry );
}

static void ListAddToTail( List *list, ListEntry *entry )
{
  entry->next = NULL;
  if( list->first == NULL ) {
    list->first = entry;
    entry->prev = NULL;
    list->last = entry;
  }
  else {
    entry->prev = list->last;
    list->last->next = entry;
    list->last = entry;
  }
}

static void ListAddBefore( List *list, ListEntry *entry, ListEntry *newentry )
{
  if( entry->prev == NULL ) {
    list->first = newentry;
  }
  else {
    entry->prev->next = newentry;
  }
  newentry->prev = entry->prev;
  newentry->next = entry;
  entry->prev = newentry;
}

static void ListAddAfter( List *list, ListEntry *entry, ListEntry *newentry )
{
  if( entry->next == NULL ) {
    list->last = newentry;
  }
  else {
    entry->next->prev = newentry;
  }
  newentry->next = entry->next;
  newentry->prev = entry;
  entry->next = newentry;
}

void ListDeleteListEntry( List *list, ListEntry *entry )
{
  if( ListHasOneEntry( list ) ) {
    if( list->first == entry ) {
      myfree( entry );
      list->first = NULL;
      list->last = NULL;
    }
    else {
      printf( "Attempting to delete nonexistent list entry\n" );
    }
  }
  else if( list->first == NULL ) {
    printf( "Attempting to delete entry in list that has no entries\n" );
  }
  else {
    if( list->first == entry ) {
      list->first = list->first->next;
      list->first->prev = NULL;
    }
    else if( list->last == entry ) {
      list->last = list->last->prev;
      list->last->next = NULL;
    }
    else {
      entry->prev->next = entry->next;
      entry->next->prev = entry->prev;
    }
    myfree( entry );
  }
}

void ListDeletePtr( List *list, void *ptr )
{
  ListEntry *w = list->first;

  while( w != NULL ) {
    if( w->val.ptr == ptr ) {
      ListDeleteListEntry( list, w );
      return;
    }
    w = w->next;
  }
}

void ListInitWalk( List *list )
{
  list->walk_ptr = list->first;
}

int ListWalkNextInt( List *list, int *ret )
{
  if( list->walk_ptr == NULL ) {
    return 0;
  }
  *ret = list->walk_ptr->val.i;
  list->walk_ptr = list->walk_ptr->next;
  return 1;
}

void *ListWalkNextPtr( List *list )
{
  void *ret;

  if( list->walk_ptr == NULL ) {
    return NULL;
  }
  ret = list->walk_ptr->val.ptr;
  list->walk_ptr = list->walk_ptr->next;
  return ret;
}

int ListSize( List *list )
{
  int size = 0;
  ListEntry *w = list->first;

  while( w != NULL ) {
    size++;
    w = w->next;
  }
  return size;
}

int ListGetPosInt( List *list, int index )
{
  ListEntry *e = list->first;
  int c = 0;

  while( e != NULL && c < index ) {
    e = e->next;
    c++;
  }

  if( c != index ) {
    return 0;
  }
  else {
    return e->val.i;
  }
}

void *ListGetPosPtr( List *list, int index )
{
  ListEntry *e = list->first;
  int c = 0;

  while( e != NULL && c < index ) {
    e = e->next;
    c++;
  }

  if( c != index ) {
    return NULL;
  }
  else {
    return e->val.ptr;
  }
}
