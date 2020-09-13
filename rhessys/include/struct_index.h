#ifndef _STRUCT_INDEX_H_
#define _STRUCT_INDEX_H_

#include <stdlib.h>
#include <assert.h>

#include "dictionary.h"

typedef struct struct_index_s {
	// TODO: Add linked list whose nodes consist of a tuple:
	// (char *, Dictionary_t *) where the first element is a string representing
	// the name of the struct, e.g. "patch_object", and the second element is a
	// pointer to the dictionary associated with the so-named struct. This will
	// allow for dynamic lookup of struct member offsets when the struct name is
	// not known until runtime.
	Dictionary_t *patch_object;
	Dictionary_t *accumulate_patch_object;
	Dictionary_t *patch_hourly_object;
	Dictionary_t *canopy_strata_object;
	// Begin, structs nested within canopy_strata_object
	//...
	Dictionary_t *cstate_struct;
	//...
	// End, structs nested within canopy_strata_object
	Dictionary_t *accumulate_strata_object;
} StructIndex_t;

#endif
