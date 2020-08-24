/** @file dictionary.h
 *  @brief Defines data and functions for a simple hash table to map
 *  strings to size_t values.
 *
 *  @note Once allocated, the table size is fixed.
 *
 *  @note Table size should be a prime number to minimize the number of
 *  collisions.
 */
#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <stdio.h>

#include "string_list.h"

#define DICTIONARY_DEFAULT_SIZE 521
#define DICTIONARY_SIZE_MEDIUM 101
#define DICTIONARY_SIZE_SMALL 23
#define DICTIONARY_KEY_EMPTY NULL
#define DICTIONARY_VALUE_EMPTY SIZE_MAX

typedef size_t DictionaryTableKey_t;
typedef size_t DictionaryValue_t;

typedef struct dict_entry_s {
	char *key;
	DictionaryValue_t value;
	struct dict_entry_s *next;
} DictionaryEntry_t;

typedef struct dict_s {
	size_t size;
	size_t numEntries;
	StringList_t *keys;
	DictionaryEntry_t *entries;
} Dictionary_t;

Dictionary_t *newDictionary(size_t tableSize);
void freeDictionary(Dictionary_t *table);

void dictionaryInsert(Dictionary_t *table, char *key, DictionaryValue_t value);
DictionaryValue_t dictionaryGet(Dictionary_t *table, char *key);

void printDictionary(Dictionary_t *table);

#endif
