#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "output_filter.h"

OutputFilter *parse(const char* input);

void test_output_filter1() {
	OutputFilter *filter = parse("test/fixtures/filter1.yml");

	print_output_filter(filter);

	g_assert(filter->next == NULL);
	// Verify timestep
	g_assert(filter->timestep == TIMESTEP_DAILY);
	// Verify output filter type
	g_assert(filter->type == OUTPUT_FILTER_PATCH);
	// Verify output section of filter
	g_assert(filter->output != NULL);
	g_assert(filter->output->format == OUTPUT_TYPE_CSV);
	int cmp = strcmp(filter->output->path, "../output/fire-project-1");
	g_assert(cmp == 0);
	cmp = strcmp(filter->output->filename, "scenario-abc1");
	g_assert(cmp == 0);
	// Verify patch section of filter
	// IDs
	OutputFilterPatch *p1 = filter->patches;
	g_assert(p1->output_patch_type == PATCH_TYPE_PATCH);
	g_assert(p1->basinID == 1);
	g_assert(p1->hillslopeID == 2);
	g_assert(p1->zoneID == 3);
	g_assert(p1->patchID == 4);
	OutputFilterPatch *p2 = p1->next;
	g_assert(p2->output_patch_type == PATCH_TYPE_BASIN);
	g_assert(p2->basinID == 2);
	g_assert(p2->next == NULL);
	// Variables
	OutputFilterVariable *v1 = filter->variables;
	g_assert(v1->variable_type == NAMED);
	cmp = strcmp(v1->name, "Qout");
	g_assert(cmp == 0);
	OutputFilterVariable *v2 = v1->next;
	g_assert(v2->variable_type == NAMED);
	cmp = strcmp(v2->name, "Qin");
	OutputFilterVariable *v3 = v2->next;
	g_assert(v3->variable_type == NAMED);
	cmp = strcmp(v3->name, "evaporation");
	g_assert(v3->next == NULL);

	free(filter);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test output_filter1", test_output_filter1);
	return g_test_run();
}
