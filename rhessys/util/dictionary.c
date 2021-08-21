/** @file dictionary.c
 *  @brief Implements functions for a simple hash table to map
 *  strings to size_t values.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "output_filter.h"
#include "dictionary.h"
#include "string_list.h"

static DictionaryTableKey_t _hash(char *key, size_t tableSize) {
	DictionaryTableKey_t hash = 0;
	while (*key != '\0') {
		hash = 31 * hash + (*key++);
	}
	return hash % tableSize;
}

static bool keysAreEqual(char *key1, char *key2) {
	return strcmp(key1, key2) == 0;
}

static bool keyIsEmpty(char *key) {
	return key == DICTIONARY_KEY_EMPTY;
}

static StringList_t *addToKeyList(Dictionary_t *table, char *key) {
	StringList_t *elem = stringListAppend(table->keys, key);
	if (table->keys == NULL) {
		// Table was empty, this is the first key.
		table->keys = elem;
	}
	return elem;
}

static DictionaryValue_t *copyValue(DictionaryValue_t v) {
	DictionaryValue_t *new_val = (DictionaryValue_t *) malloc(sizeof(DictionaryValue_t));
	new_val->data_type = v.data_type;
	new_val->offset = v.offset;
	new_val->sub_struct_index = v.sub_struct_index;
	return new_val;
}

Dictionary_t *newDictionary(size_t tableSize) {
	Dictionary_t *t = (Dictionary_t *) malloc(sizeof(Dictionary_t));
	assert(t);
	t->size = tableSize;
	t->numEntries = 0;
	t->keys = NULL;
	t->entries = (DictionaryEntry_t *) malloc(tableSize * sizeof(DictionaryEntry_t));
	assert(t->entries);
	// Initialize hash table entries
	DictionaryEntry_t *e;
	for (size_t i = 0; i < tableSize; i++) {
		e = t->entries + i;
		e->key = DICTIONARY_KEY_EMPTY;
		e->value = DICTIONARY_VALUE_EMPTY;
		e->next = NULL;
	}
	return t;
}

void freeDictionary(Dictionary_t *table) {
	if (table == NULL) return;
	// Free keys
	freeStringList(table->keys);
	// Free buckets
	DictionaryEntry_t *entry;
	DictionaryEntry_t *tmpEntry;
	for (size_t i = 0; i < table->size; i++) {
		entry = table->entries + i;
		tmpEntry = entry->next;
		while (tmpEntry != NULL) {
			entry = tmpEntry;
			tmpEntry = entry->next;
			if (entry->value) free(entry->value);
			free(entry);
		}
	}
	free(table->entries);
	free(table);
}

bool dictionaryValueEquals(DictionaryValue_t first, DictionaryValue_t second) {
	if (first.data_type != second.data_type) return false;
	if (first.offset != second.offset) return false;
	return true;
}

void dictionaryInsert(Dictionary_t *table, char *key, DictionaryValue_t value) {
	assert(table != NULL);
	DictionaryTableKey_t hash = _hash(key, table->size);
	DictionaryEntry_t *e = table->entries + hash;
	if (keyIsEmpty(e->key)) {
		// Insert first value for this table entry
		// Add to our list of keys, copying the string in the process
		StringList_t *keyCopy = addToKeyList(table, key);
		// Assign pointer to our copy of the key as the entry key
		e->key = keyCopy->str;
		e->value = copyValue(value);
		e->next = NULL;
		table->numEntries++;
	} else {
		// Iterate over entries in this bucket
		while (e) {
			if (keysAreEqual(e->key, key)) {
				// Key is already present, overwrite
				free(e->value);
				e->value = copyValue(value);
				break;
			} else if (NULL == e->next) {
				// We've reached the end of the list, add a new value for this table entry
				e->next = (DictionaryEntry_t *) malloc(sizeof(DictionaryEntry_t));
				assert(e->next);
				// Add to our list of keys, copying the string in the process
				StringList_t *keyCopy = addToKeyList(table, key);
				// Assign a pointer to our copy of the key as the entry key
				e->next->key = keyCopy->str;
				e->next->value = copyValue(value);
				e->next->next = NULL;
				table->numEntries++;
				break;
			}
			e = e->next;
		}
	}
}

DictionaryValue_t *dictionaryGet(Dictionary_t *table, char *key) {
	assert(table != NULL);
	DictionaryTableKey_t hash = _hash(key, table->size);
	DictionaryEntry_t *e = table->entries + hash;
	// Search for key in bucket
	while (e) {
		if (e->key != NULL && keysAreEqual(e->key, key)) {
			return e->value;
		}
		e = e->next;
	}
	return DICTIONARY_VALUE_EMPTY;
}

void printDictionary(Dictionary_t *table) {
	DictionaryEntry_t *entry;
	DictionaryEntry_t *tmpEntry;
	fprintf(stderr, "Dictionary@%p {\n", (void *)table);
	for (size_t i = 0; i < table->size; i++) {
		entry = table->entries + i;
		if (entry->key != DICTIONARY_KEY_EMPTY) {
			fprintf(stderr, "\t'%s': {data_type: %d, offset: %zu},\n",
					entry->key, entry->value->data_type, entry->value->offset);
			tmpEntry = entry->next;
			while (tmpEntry != NULL) {
				entry = tmpEntry;
				fprintf(stderr, "\t'%s': {data_type: %d, offset: %zu},\n",
						entry->key, entry->value->data_type, entry->value->offset);
				tmpEntry = entry->next;
			}
		}
	}
	fprintf(stderr, "}\n");
}

