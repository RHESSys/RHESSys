#include <stdio.h>
#include <glib.h>

#include "output_filter.h"

void test_output_filter_patch_list() {
	// Setup and destroy list
	OutputFilterPatch *head = create_new_output_filter_patch();
	head->output_patch_type = BASIN;
	OutputFilterPatch *next = create_new_output_filter_patch();
	next->output_patch_type = HILLSLOPE;
	add_to_output_filter_patch_list(head, next);
	OutputFilterPatch *last = create_new_output_filter_patch();
	last->output_patch_type = ZONE;
	add_to_output_filter_patch_list(head, last);


	g_assert(head->output_patch_type == BASIN);
	g_assert(head->next->output_patch_type == HILLSLOPE);
	g_assert(head->next->next->output_patch_type == ZONE);

	free_output_filter_patch_list(head);
}

int main(int argc, char **argv) {
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/set1/test output_filter_patch_list", test_output_filter_patch_list);
  return g_test_run();
}
