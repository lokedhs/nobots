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

HashTable *HashTableCreate( HashFunc *, int );
void HashTableDelete( HashTable * );
void HashTableAddEntry( HashTable *, char *, void * );
void HashTableDeleteEntry( HashTable *, char * );
void *HashTableGetEntry( HashTable *, char * );
void HashTableInitWalk( HashTable * );
HashEntry *HashTableWalkNext( HashTable * );

HashBucket *HashBucketCreate( void );
void HashBucketDelete( HashBucket * );
void HashBucketAddEntry( HashBucket *, char *, void * );
void HashBucketDeleteEntry( HashBucket *, char * );
void *HashBucketGetEntry( HashBucket *, char * );
void HashBucketInitWalk( HashBucket * );
HashEntry *HashBucketWalkNext( HashBucket * );

HashEntry *HashEntryCreate( char *, void * );
void HashEntryDelete( HashEntry * );

int hash_std_func( char * );

#endif
