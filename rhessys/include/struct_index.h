#ifndef _STRUCT_INDEX_H_
#define _STRUCT_INDEX_H_

#include <stdlib.h>
#include <assert.h>

#include "dictionary.h"

typedef struct struct_index_s {
	Dictionary_t *patch_object;
	Dictionary_t *accumulate_patch_object;
	Dictionary_t *patch_hourly_object;
	// Begin, structs nested within patch_object...
	Dictionary_t *patch_fire_water_object;
	Dictionary_t *rooting_zone_object;
	Dictionary_t *snowpack_object;
	Dictionary_t *soil_c_object;
	Dictionary_t *soil_n_object;
	Dictionary_t *litter_object;
	Dictionary_t *litter_c_object;
	Dictionary_t *litter_n_object;
	Dictionary_t *cdayflux_patch_struct;
	Dictionary_t *ndayflux_patch_struct;
	// End, structs nested within patch_object.

	Dictionary_t *canopy_strata_object;
	// Begin, structs nested within canopy_strata_object...
	Dictionary_t *accumulate_strata_object;
	Dictionary_t *cdayflux_struct;
	Dictionary_t *cstate_struct;
	Dictionary_t *target_object;
	Dictionary_t *epvar_struct;
	Dictionary_t *nstate_struct;
	Dictionary_t *ndayflux_struct;
	Dictionary_t *phenology_struct;
	Dictionary_t *fire_effects_object;
	Dictionary_t *mult_conduct_struct;
	// End, structs nested within canopy_strata_object.

	Dictionary_t *hillslope_object;
	// Begin, structs nested within hillslope_object...
	Dictionary_t *gw_object;
	// End, structs nested within hillslope_object.

	Dictionary_t *basin_object;

} StructIndex_t;

#endif
