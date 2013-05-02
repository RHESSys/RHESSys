/** @file patch_hash_table.c
 *  @brief Implements functions for a simple hash table to map
 *  fully qualified patch IDs (a combination of patch, zone, and hill
 *  IDs) to flow table indices.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "util.h"
#include "patch_hash_table.h"

PatchTableKey_t _hash(PatchKey_t originKey, size_t tableSize) {
	int mult = (originKey.patchID * originKey.hillID * originKey.zoneID);
	PatchTableKey_t hash = (PatchTableKey_t) (mult % tableSize);
#ifdef DEBUG
	fprintf(stderr, "hash: tableSize: %d\n", tableSize);
	fprintf(stderr, "hash: mult: %d\n", mult);
	fprintf(stderr, "hash: %d\n", hash);
#endif
	return hash;
}

bool _keysAreEqual(PatchKey_t key1, PatchKey_t key2) {
	if (key1.patchID != key2.patchID) return false;
	if (key1.hillID != key2.hillID) return false;
	if (key1.zoneID != key2.zoneID) return false;

	return true;
}

bool _keyIsEmpty(PatchKey_t key) {
	if ((PATCH_HASH_TABLE_EMPTY == key.patchID)
			&& (PATCH_HASH_TABLE_EMPTY == key.hillID)
			&& (PATCH_HASH_TABLE_EMPTY == key.zoneID)) {
		return true;
	}

	return false;
}

PatchTable_t *allocatePatchHashTable(size_t tableSize) {
	PatchTable_t* table = (PatchTable_t *) malloc( sizeof(PatchTable_t) );
	table->tableSize = tableSize;
	table->numEntries = 0;
	size_t allocSize = tableSize * sizeof(PatchTableEntry_t);
	table->entries = (PatchTableEntry_t *) malloc( allocSize );
	if (NULL == table->entries) {
		free(table);
		table = NULL;
	} else {
		// Initialize keys to empty values
		PatchTableEntry_t *entry;
		size_t size = sizeof(PatchTableEntry_t);
		for (size_t i = 0; i < tableSize; i++) {
			size_t increment = i * size;
			entry = table->entries + i;
			entry->originKey.patchID = PATCH_HASH_TABLE_EMPTY;
			entry->originKey.hillID = PATCH_HASH_TABLE_EMPTY;
			entry->originKey.zoneID = PATCH_HASH_TABLE_EMPTY;
			entry->value = PATCH_HASH_TABLE_EMPTY;
			entry->next = NULL;
		}
	}
	return table;
}

void freePatchHashTable(PatchTable_t *table) {
	assert(table != NULL);
	// Free buckets
	PatchTableEntry_t *entry;
	PatchTableEntry_t *tmpEntry;
	size_t size = sizeof(PatchTableEntry_t);
	for (size_t i = 0; i < table->tableSize; i++) {
		entry = table->entries + i;
		tmpEntry = entry->next;
		while (tmpEntry != NULL ) {
			entry = tmpEntry;
			tmpEntry = entry->next;
			free(entry);
		}
	}
	free(table->entries);
	free(table);
}

void patchHashTableInsert(PatchTable_t *table, PatchKey_t key, PatchTableValue_t value) {
	assert(table != NULL);
	PatchTableKey_t hash = _hash(key, table->tableSize);
#ifdef DEBUG
	fprintf(stderr, "Inserting key: %d, %d, %d; value: %d; hash: %d\n",
				key.patchID, key.hillID, key.zoneID, value, hash);
#endif
	PatchTableEntry_t *entry = table->entries + hash;
	if (_keyIsEmpty(entry->originKey)) {
		// Insert first key
#ifdef DEBUG
		fprintf(stderr, "key is empty\n");
#endif
		entry->originKey = key;
		entry->value = value;
		entry->next = NULL;
		table->numEntries++;
	} else {
		while (entry) {
			if (_keysAreEqual(entry->originKey, key)) {
				// Key is already present, overwrite value and return
#ifdef DEBUG
				fprintf(stderr, "keys are equal\n");
#endif
				entry->value = value;
				return;
			}
			if (NULL == entry->next) {
				// We've reached the end of the list, add a new entry and return
#ifdef DEBUG
				fprintf(stderr, "end of list; next: %p\n", entry->next);
#endif
				entry->next = (PatchTableEntry_t *)malloc( sizeof(PatchTableEntry_t) );
#ifdef DEBUG
				fprintf(stderr, "next: %p\n", entry->next);
#endif
				assert( entry->next );
				entry->next->originKey = key;
				entry->next->value = value;
				entry->next->next = NULL;
				table->numEntries++;
				return;
			}
			entry = entry->next;
		}
	}
}

PatchTableValue_t patchHashTableGet(PatchTable_t *table, PatchKey_t key) {
	assert(table != NULL);
	PatchTableKey_t hash = _hash(key, table->tableSize);
	PatchTableEntry_t *entry = table->entries + hash;
	// Search for key
	while (entry) {
		if (_keysAreEqual(entry->originKey, key)) {
#ifdef DEBUG
			fprintf(stderr, "found key: %d, %d, %d, returning value: %d\n",
					key.patchID, key.hillID, key.zoneID, entry->value);
#endif
			return entry->value;
		}
		entry = entry->next;
	}
	return PATCH_HASH_TABLE_EMPTY;
}

void printPatchHashTable(PatchTable_t *table) {
	PatchTableEntry_t *entry;
	PatchTableEntry_t *tmpEntry;
	size_t size = sizeof(PatchTableEntry_t);
	for (size_t i = 0; i < table->tableSize; i++) {
		entry = table->entries + i;
		fprintf(stderr, "Entry %d: originKey: %d, %d, %d; value: %d; next: %p\n", i,
				entry->originKey.patchID, entry->originKey.hillID,
				entry->originKey.zoneID, entry->value, entry->next);
		tmpEntry = entry->next;
		while (tmpEntry != NULL ) {
			entry = tmpEntry;

			fprintf(stderr, "\tEntry %d: originKey: %d, %d, %d; value: %d; next: %p\n", i,
					entry->originKey.patchID, entry->originKey.hillID,
					entry->originKey.zoneID, entry->value, entry->next);

			tmpEntry = entry->next;

		}
	}
}
