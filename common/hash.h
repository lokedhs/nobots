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

#ifndef HASH_H
#define HASH_H

#include "list.h"

typedef int HashFunc( char * );

typedef struct {
  char *key;
  void *data;
} HashEntry;

typedef struct {
  List *entries;		/* list of entries */
  ListEntry *walk_pointer;	/* current position of the walk pointer */
} HashBucket;

typedef struct {
  HashBucket **buckets;		/* list of buckets */
  HashFunc *hash_func;		/* hashing function */
  int nbuckets;			/* number of buckets */
  int walk_pointer;		/* current position of the walk pointer */
} HashTable;

HashTable *hashtable_create( HashFunc *, int );
void hashtable_delete( HashTable * );
void hashtable_add_entry( HashTable *, char *, void * );
void hashtable_deleteEntry( HashTable *, char * );
void *hashtable_get_entry( HashTable *, char * );
void hashtable_init_walk( HashTable * );
HashEntry *hashtable_walk_next( HashTable * );

HashBucket *hashbucket_create( void );
void hashbucket_delete( HashBucket * );
void hashbucket_add_entry( HashBucket *, char *, void * );
void hashbucket_deleteEntry( HashBucket *, char * );
void *hashbucket_get_entry( HashBucket *, char * );
void hashbucket_init_walk( HashBucket * );
HashEntry *hashbucket_walk_next( HashBucket * );

HashEntry *hashentry_create( char *, void * );
void hashentry_delete( HashEntry * );

int hash_std_func( char * );

#endif
