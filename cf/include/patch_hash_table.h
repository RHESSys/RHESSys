#ifndef PATCH_HASH_TABLE_H
#define PATCH_HASH_TABLE_H

#include "util.h"

#define PATCH_HASH_TABLE_DEFAULT_SIZE 4099
#define PATCH_HASH_TABLE_LOAD_FACTOR 0.75
#define PATCH_HASH_TABLE_EMPTY -1

// Think before changing KeyType to a much larger struct as KeyType is
// often passed by value
typedef struct key_s {
	int patchID;
	int hillID;
	int zoneID;
} KeyType;

typedef size_t TableKeyType;

typedef int ValueType;

//typedef struct {
//	KeyType originKey;
//	ValueType value;
//} EntryType;

typedef struct table_entry_s {
//	TableKeyType	key;
	KeyType	originKey;
	ValueType value;
	struct table_entry_s *next;
} TableEntry;

typedef struct table_s {
	size_t tableSize;
	size_t numEntries;
	TableEntry *entries;
} Table;


Table *allocatePatchHashTable(size_t tableSize);
void freePatchHashTable(Table *table);

void patchHashTableInsert(Table *table, KeyType key, ValueType value);
ValueType patchHashTableGet(Table *table, KeyType key);

void printPatchHashTable(Table *table);
//bool patchHashTableContainsKey(Table* table, KeyType key);

#endif
