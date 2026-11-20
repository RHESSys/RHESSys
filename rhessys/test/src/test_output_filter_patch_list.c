#include <stdio.h>
#include <glib.h>

#include "output_filter.h"

/* create_new_output_filter_patch() does not initialize the ID fields, so set
 * every field explicitly before adding to a list. */
static OutputFilterPatch *make_patch(OutputPatchType type, int basinID,
		int hillslopeID, int zoneID, int patchID) {
	OutputFilterPatch *p = create_new_output_filter_patch();
	p->output_patch_type = type;
	p->basinID = basinID;
	p->hillslopeID = hillslopeID;
	p->zoneID = zoneID;
	p->patchID = patchID;
	return p;
}

void test_output_filter_patch_list() {
	// Distinct basins: nothing is a parent/duplicate of anything else, so all three are appended
	OutputFilterPatch *head = make_patch(PATCH_TYPE_BASIN, 1, 0, 0, 0);
	OutputFilterPatch *next = make_patch(PATCH_TYPE_HILLSLOPE, 2, 1, 0, 0);
	g_assert(add_to_output_filter_patch_list(head, next) == next);
	OutputFilterPatch *last = make_patch(PATCH_TYPE_ZONE, 3, 1, 1, 0);
	g_assert(add_to_output_filter_patch_list(head, last) == last);

	g_assert(head->output_patch_type == PATCH_TYPE_BASIN);
	g_assert(head->next->output_patch_type == PATCH_TYPE_HILLSLOPE);
	g_assert(head->next->next->output_patch_type == PATCH_TYPE_ZONE);
	g_assert(head->next->next->next == NULL);

	// A zone inside basin 1 is covered by the basin 1 entry: not added, list unchanged
	OutputFilterPatch *child = make_patch(PATCH_TYPE_ZONE, 1, 5, 5, 0);
	g_assert(add_to_output_filter_patch_list(head, child) == NULL);
	g_assert(head->next->next->next == NULL);
	free(child);

	free_output_filter_patch_list(head);
}

int main(int argc, char **argv) {
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/set1/test output_filter_patch_list", test_output_filter_patch_list);
  return g_test_run();
}
