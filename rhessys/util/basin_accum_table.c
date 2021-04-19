/** @file dictionary.c
 *  @brief Implements functions for a simple hash table to map
 *  output filter variables to accumulated values.
 *
 *  @note Once allocated, the table size is fixed.
 *
 *  @note Table size should be a prime number to minimize the number of
 *  collisions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "types.h"
#include "output_filter.h"
#include "basin_accum_table.h"

static size_t _hash(BasinAccumTableKey_t *key, size_t tableSize) {
    size_t hash = 0;
    char *str_ptr = key->name;
    while (*str_ptr != '\0') {
        hash = 31 * hash + (*str_ptr++);
    }
    str_ptr = key->sub_struct_varname;
    while (*str_ptr != '\0') {
        hash = 31 * hash + (*str_ptr++);
    }
    return hash % tableSize;
}

static bool keysAreEqual(BasinAccumTableKey_t *key1, BasinAccumTableKey_t *key2) {
    int cmp = strcmp(key1->name, key2->name);
    if (cmp != 0) {
        return false;
    }
    return strcmp(key1->sub_struct_varname, key2->sub_struct_varname) == 0;
}

static bool keyIsEmpty(BasinAccumTableKey_t *key) {
    return key == BASIN_ACCUM_TABLE_KEY_EMPTY;
}

