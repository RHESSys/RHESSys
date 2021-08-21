#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "output_filter.h"

OutputFilter *parse(const char* input, bool verbose);

void test_output_filter3() {
	OutputFilter *filter = parse("fixtures/filter3.yml", true);

	print_output_filter(filter);
	g_assert(filter->next == NULL);
	// Verify timestep
	g_assert(filter->timestep == TIMESTEP_DAILY);
	// Verify output filter type
	g_assert(filter->type == OUTPUT_FILTER_PATCH);
	// Verify output section of filter
	g_assert(filter->output != NULL);
	g_assert(filter->output->format == OUTPUT_TYPE_CSV);
	int cmp = strcmp(filter->output->path, "./output/fire-project-1");
	g_assert(cmp == 0);
	cmp = strcmp(filter->output->filename, "scenario-abc1");
	g_assert(cmp == 0);
	// Verify patch section of filter
	// IDs
	OutputFilterPatch *p1 = filter->patches;
	g_assert(p1->output_patch_type == PATCH_TYPE_BASIN);
	g_assert(p1->basinID == 1);
	g_assert(p1->next == NULL);
	// Variables
	OutputFilterVariable *v1 = filter->variables;
	g_assert(v1->hierarchy_level == OF_HIERARCHY_LEVEL_PATCH);
	g_assert(v1->variable_type == NAMED);
	cmp = strcmp(v1->name, "water_balance");
	g_assert(cmp == 0);
	OutputFilterVariable *v2 = v1->next;
	g_assert(v2->hierarchy_level == OF_HIERARCHY_LEVEL_PATCH);
	g_assert(v2->variable_type == NAMED);
	cmp = strcmp(v2->name, "Qout");
	OutputFilterVariable *v3 = v2->next;
	g_assert(v3->hierarchy_level == OF_HIERARCHY_LEVEL_PATCH);
	g_assert(v3->variable_type == NAMED);
	cmp = strcmp(v3->name, "Qin");
	g_assert(v3->next == NULL);

	free(filter);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test output_filter3", test_output_filter3);
	return g_test_run();
}
