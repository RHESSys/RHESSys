/** @file patch_hash_table.h
 *  @brief Defines data and functions for a simple hash table to map
 *  fully qualified patch IDs (a combination of patch, zone, and hill
 *  IDs) to flow table indices.
 *
 *  @note Once allocated, the table size is fixed.  For large flow tables
 *  (e.g. >100,000 entries) a table size larger than the default may be
 *  needed to maintain performance.
 *
 *  @note Table size should be a prime number to minimize the number of
 *  collisions.
 */
#ifndef PATCH_HASH_TABLE_H
#define PATCH_HASH_TABLE_H

#include "util.h"

#define PATCH_HASH_TABLE_DEFAULT_SIZE 25013
//#define PATCH_HASH_TABLE_DEFAULT_SIZE 94007
#define PATCH_HASH_TABLE_EMPTY -1

// Note: think before changing this to a much larger
// struct as it is often passed by value
typedef struct key_s {
	int patchID;
	int zoneID;
	int hillID;
} PatchKey_t;

typedef size_t PatchTableKey_t;

typedef int PatchTableValue_t;

typedef struct table_entry_s {
	PatchKey_t	originKey;
	PatchTableValue_t value;
	struct table_entry_s *next;
} PatchTableEntry_t;

typedef struct table_s {
	size_t tableSize;
	size_t numEntries;
	PatchTableEntry_t *entries;
} PatchTable_t;


PatchTable_t *allocatePatchHashTable(size_t tableSize);
void freePatchHashTable(PatchTable_t *table);

void patchHashTableInsert(PatchTable_t *table, PatchKey_t key, PatchTableValue_t value);
PatchTableValue_t patchHashTableGet(PatchTable_t *table, PatchKey_t key);

void printPatchHashTable(PatchTable_t *table);

#endif
