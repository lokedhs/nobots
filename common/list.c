/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-2002  Elias Martenson (elias-m@algonet.se)
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

static void list_add_to_head( List *, ListEntry * );
static void list_add_to_tail( List *, ListEntry * );
static void list_add_before( List *, ListEntry *, ListEntry * );
static void list_add_after( List *, ListEntry *, ListEntry * );

List *list_create( void )
{
  List *ret = mymalloc( sizeof( List ) );

  ret->first = NULL;
  ret->last = NULL;
  return ret;
}

void list_delete( List *list )
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

void list_add_to_head_int( List *list, int val )
{
  ListEntry *entry = mymalloc( sizeof( ListEntry ) );

  entry->val.i = val;
  list_add_to_head( list, entry );
}

void list_add_to_head_ptr( List *list, void *val )
{
  ListEntry *entry = mymalloc( sizeof( ListEntry ) );

  entry->val.ptr = val;
  list_add_to_head( list, entry );
}

static void list_add_to_head( List *list, ListEntry *entry )
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

void list_add_to_tail_int( List *list, int val )
{
  ListEntry *entry = mymalloc( sizeof( ListEntry ) );

  entry->val.i = val;
  list_add_to_tail( list, entry );
}

void list_add_to_tail_ptr( List *list, void *val )
{
  ListEntry *entry = mymalloc( sizeof( ListEntry ) );

  entry->val.ptr = val;
  list_add_to_tail( list, entry );
}

void list_add_before_ptr( List *list, ListEntry *entry, void *val )
{
  ListEntry *newentry = mymalloc( sizeof( ListEntry ) );

  entry->val.ptr = val;
  list_add_before( list, entry, newentry );
}

void list_add_before_int( List *list, ListEntry *entry, int val )
{
  ListEntry *newentry = mymalloc( sizeof( ListEntry ) );

  entry->val.i = val;
  list_add_before( list, entry, newentry );
}

void list_add_after_ptr( List *list, ListEntry *entry, void *val )
{
  ListEntry *newentry = mymalloc( sizeof( ListEntry ) );

  entry->val.ptr = val;
  list_add_after( list, entry, newentry );
}

void list_add_after_int( List *list, ListEntry *entry, int val )
{
  ListEntry *newentry = mymalloc( sizeof( ListEntry ) );

  entry->val.i = val;
  list_add_after( list, entry, newentry );
}

static void list_add_to_tail( List *list, ListEntry *entry )
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

static void list_add_before( List *list, ListEntry *entry, ListEntry *newentry )
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

static void list_add_after( List *list, ListEntry *entry, ListEntry *newentry )
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

void list_delete_list_entry( List *list, ListEntry *entry )
{
  if( list_has_one_entry( list ) ) {
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

void list_delete_ptr( List *list, void *ptr )
{
  ListEntry *w = list->first;

  while( w != NULL ) {
    if( w->val.ptr == ptr ) {
      list_delete_list_entry( list, w );
      return;
    }
    w = w->next;
  }
}

void list_init_walk( List *list )
{
  list->walk_ptr = list->first;
}

int list_walk_next_int( List *list, int *ret )
{
  if( list->walk_ptr == NULL ) {
    return 0;
  }
  *ret = list->walk_ptr->val.i;
  list->walk_ptr = list->walk_ptr->next;
  return 1;
}

void *list_walk_next_ptr( List *list )
{
  void *ret;

  if( list->walk_ptr == NULL ) {
    return NULL;
  }
  ret = list->walk_ptr->val.ptr;
  list->walk_ptr = list->walk_ptr->next;
  return ret;
}

int list_size( List *list )
{
  int size = 0;
  ListEntry *w = list->first;

  while( w != NULL ) {
    size++;
    w = w->next;
  }
  return size;
}

int list_get_pos_int( List *list, int index )
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

void *list_get_pos_ptr( List *list, int index )
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
