#include <stdlib.h>
#include <math.h>
#include <openmp.h>
#include "rhessys.h"
#include "functions.h"

struct routing_list_object *construct_topmodel_patchlist(struct hillslope_object * const hillslope) {

	struct routing_list_object	*patch_list = NULL;
	int num_patches;
	int z, p, current_patch_idx;
	
	// How many patches are there in the basin?
	num_patches = 0;
    #pragma omp parallel for reduction(+ : num_patches)                       
	for (int z = 0; z < hillslope->num_zones; z++) {
		struct zone_object const * zone = hillslope->zones[z];
		for ( p = 0; p < zone->num_patches; p++) {
			num_patches++;
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
	for (int z = 0; z < hillslope->num_zones; z++) {
		struct zone_object const * zone = hillslope->zones[z];
		for (int p = 0; p < zone->num_patches; p++) {
			patch_list->list[curr_patch_idx] = zone->patches[p];
			curr_patch_idx++;
		}
	}

	return patch_list;
}
