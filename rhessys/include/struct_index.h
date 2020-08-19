#ifndef _STRUCT_INDEX_H_
#define _STRUCT_INDEX_H_

#include <stdlib.h>
#include <assert.h>

#include "dictionary.h"

typedef struct struct_index_s {
	Dictionary_t *patch_object;
	Dictionary_t *accumulate_patch_object;
} StructIndex_t;

#endif
