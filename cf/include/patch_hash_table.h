#ifndef PATCH_HASH_TABLE_H
#define PATCH_HASH_TABLE_H

#include "util.h"

#define PATCH_HASH_TABLE_DEFAULT_SIZE 25013
//#define PATCH_HASH_TABLE_DEFAULT_SIZE 94007
#define PATCH_HASH_TABLE_LOAD_FACTOR 0.75
#define PATCH_HASH_TABLE_EMPTY -1

// Think before changing KeyType to a much larger struct as KeyType is
// often passed by value
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
