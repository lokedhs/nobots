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

#include <string.h>
#include <stdlib.h>

#include "mem.h"
#include "list.h"
#include "hash.h"

HashTable *hashtable_create( HashFunc *hash_func, int nbuckets )
{
  HashTable *table = mymalloc( sizeof( HashTable ) );
  int c;

  if( hash_func == NULL ) {
    table->hash_func = hash_std_func;
    table->nbuckets = 256;
  }
  else {
    table->hash_func = hash_func;
    table->nbuckets = nbuckets;
  }

  table->buckets = mymalloc( sizeof( HashBucket * ) * table->nbuckets );
  for( c = 0 ; c < table->nbuckets ; c++ ) {
    table->buckets[ c ] = hashbucket_create();
  }

  return table;
}

void hashtable_delete( HashTable *table )
{
  int c;

  for( c = 0 ; c < table->nbuckets ; c++ ) {
    hashbucket_delete( table->buckets[ c ] );
  }
  myfree( table->buckets );
  myfree( table );
}

void hashtable_add_entry( HashTable *table, char *key, void *data )
{
  int bucket;

  bucket = table->hash_func( key );
  hashbucket_add_entry( table->buckets[ bucket ], key, data );
}

void hashtable_deleteEntry( HashTable *table, char *key )
{
  int bucket;

  bucket = table->hash_func( key );
  hashbucket_deleteEntry( table->buckets[ bucket ], key );
}

void *hashtable_get_entry( HashTable *table, char *key )
{
  int bucket;

  bucket = table->hash_func( key );
  return hashbucket_get_entry( table->buckets[ bucket ], key );
}

void hashtable_init_walk( HashTable *table )
{
  table->walk_pointer = 0;
  hashbucket_init_walk( table->buckets[ 0 ] );
}

HashEntry *hashtable_walk_next( HashTable *table )
{
  HashEntry *entry;

  while( (entry = hashbucket_walk_next( table->buckets[ table->walk_pointer ] ))
	 == NULL ){
    table->walk_pointer++;
    if( table->walk_pointer >= table->nbuckets ) {
      return NULL;
    }
    hashbucket_init_walk( table->buckets[ table->walk_pointer ] );
  }
  return entry;
}


HashBucket *hashbucket_create( void )
{
  HashBucket *ret = mymalloc( sizeof( HashBucket ) );

  ret->entries = list_create();
  return ret;
}

void hashbucket_delete( HashBucket *bucket )
{
  ListEntry *w;

  for( w = bucket->entries->first ; w != NULL ; w = w->next ) {
    myfree( ((HashEntry *)(w->val.ptr))->key );
    myfree( w->val.ptr );
  }
  list_delete( bucket->entries );
}

void hashbucket_add_entry( HashBucket *bucket, char *key, void *data )
{
  list_add_to_tail_ptr( bucket->entries, hashentry_create( key, data ) );
}

void hashbucket_deleteEntry( HashBucket *bucket, char *key )
{
  ListEntry *w;

  for( w = bucket->entries->first ; w != NULL ; w = w->next ) {
    if( strcmp( ((HashEntry *)(w->val.ptr))->key, key ) == 0 ) {
      hashentry_delete( (HashEntry *)w->val.ptr );
      list_delete_list_entry( bucket->entries, w );
      break;
    }
  }
}

void *hashbucket_get_entry( HashBucket *bucket, char *key )
{
  ListEntry *w;

  for( w = bucket->entries->first ; w != NULL ; w = w->next ) {
    if( strcmp( ((HashEntry *)(w->val.ptr))->key, key ) == 0 ) {
      return ((HashEntry *)w->val.ptr)->data;
    }
  }
  return NULL;
}

void hashbucket_init_walk( HashBucket *bucket )
{
  bucket->walk_pointer = bucket->entries->first;
}

HashEntry *hashbucket_walk_next( HashBucket *bucket )
{
  HashEntry *entry;

  if( bucket->walk_pointer == NULL ) {
    return NULL;
  }
  entry = bucket->walk_pointer->val.ptr;
  if( entry != NULL ) {
    bucket->walk_pointer = bucket->walk_pointer->next;
  }
  return entry;
}


HashEntry *hashentry_create( char *key, void *data )
{
  HashEntry *entry = mymalloc( sizeof( HashEntry ) );

  entry->key = mystrdup( key );
  entry->data = data;
  return entry;
}

void hashentry_delete( HashEntry *entry )
{
  myfree( entry->key );
  myfree( entry );
}

int hash_std_func( char *key )
{
  return *key;
}
