/** @file basin_accum_table.h
 *  @brief Defines data and functions for a simple hash table to map
 *  output filter variables to accumulated values.
 *
 *  @note Once allocated, the table size is fixed.
 *
 *  @note Table size should be a prime number to minimize the number of
 *  collisions.
 */
#ifndef _RHESSYS_BASIN_ACCUM_TABLE_H_
#define _RHESSYS_BASIN_ACCUM_TABLE_H_

#include "output_filter.h"

#define BASIN_ACCUM_TABLE_DEFAULT_SIZE 521
#define BASIN_ACCUM_TABLE_SIZE_LARGE 251
#define BASIN_ACCUM_TABLE_SIZE_MEDIUM 101
#define BASIN_ACCUM_TABLE_SIZE_SMALL 53
#define BASIN_ACCUM_TABLE_KEY_EMPTY NULL
#define BASIN_ACCUM_TABLE_VALUE_EMPTY NULL

typedef struct bas_acc_tab_k {
    char *name;
    char *sub_struct_varname;
} BasinAccumTableKey_t;

typedef MaterializedVariable BasinAccumTableValue_t;

typedef struct bas_acc_tab_entry_s {
    BasinAccumTableKey_t *key;
    BasinAccumTableValue_t *value;
    struct bas_acc_tab_entry_s *next;
} BasinAccumTableEntry_t;

typedef struct dict_s {
    size_t size;
    size_t numEntries;
    BasinAccumTableKey_t *keys;
    BasinAccumTableEntry_t *entries;
} BasinAccumTable_t;

BasinAccumTable_t *newBasinAccumTable(size_t tableSize);
void freeBasinAccumTable(BasinAccumTable_t *table);

bool basinAccumTableValueEquals(BasinAccumTableValue_t first, BasinAccumTableValue_t second);
void basinAccumTableInsert(BasinAccumTable_t *table, BasinAccumTableKey_t *key, BasinAccumTableValue_t value);
BasinAccumTableValue_t *basinAccumTableGet(BasinAccumTable_t *table, BasinAccumTableKey_t *key);

void printBasinAccumTable(BasinAccumTable_t *table);


#endif //_RHESSYS_BASIN_ACCUM_TABLE_H_
