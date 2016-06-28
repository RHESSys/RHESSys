#include <stdlib.h>
#include <math.h>

#include "rhessys.h"
#include "functions.h"

struct routing_list_object *construct_topmodel_patchlist(struct basin_object * const basin) {

	struct routing_list_object	*patch_list = NULL;
	int num_patches;

	// How many patches are there in the basin?
	num_patches = 0;
    #pragma omp parallel for reduction(+ : num_patches)                          //160628LML
	for (int h = 0; h < basin->num_hillslopes; h++) {
		struct hillslope_object const * hill = basin->hillslopes[h];
		for (int z = 0; z < hill->num_zones; z++) {
			struct zone_object const * zone = hill->zones[z];
			for (int p = 0; p < zone->num_patches; p++) {
				num_patches++;
			}
		}
	}

	if (num_patches == 0) {
		return patch_list;
	}

	// Build the patch list
	patch_list = (struct routing_list_object *)alloc( sizeof(struct routing_list_object), "patch_list", "construct_topmodel_patchlist");
	patch_list->num_patches = num_patches;
	patch_list->list = (struct patch_object **)alloc(
			num_patches * sizeof(struct patch_object *), "patch_list",
			"construct_topmodel_patchlist");
	int curr_patch_idx = 0;
	for (int h = 0; h < basin->num_hillslopes; h++) {
		struct hillslope_object const * hill = basin->hillslopes[h];
		for (int z = 0; z < hill->num_zones; z++) {
			struct zone_object const * zone = hill->zones[z];
			for (int p = 0; p < zone->num_patches; p++) {
				patch_list->list[curr_patch_idx] = zone->patches[p];
				curr_patch_idx++;
			}
		}
	}

	return patch_list;
}
