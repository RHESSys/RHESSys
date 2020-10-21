#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "output_filter.h"

OutputFilter *parse(const char* input);

void test_output_filter5() {
	OutputFilter *filter = parse("test/fixtures/filter5.yml");

	print_output_filter(filter);

	g_assert(filter->next == NULL);
	// Verify timestep
	g_assert(filter->timestep == TIMESTEP_DAILY);
	// Verify output filter type
	g_assert(filter->type == OUTPUT_FILTER_BASIN);
	// Verify output section of filter
	g_assert(filter->output != NULL);
	g_assert(filter->output->format == OUTPUT_TYPE_CSV);
	int cmp = strcmp(filter->output->path, "../output/fire-project-1");
	g_assert(cmp == 0);
	cmp = strcmp(filter->output->filename, "scenario-abc4.foo");
	g_assert(cmp == 0);
	// Verify patch section of filter
	// IDs
	OutputFilterBasin *b1 = filter->basins;
	g_assert(b1->basinID == 1);
	g_assert(b1->next == NULL);
	// Variables
	OutputFilterVariable *v1 = filter->variables;
	g_assert(v1->hierarchy_level == OF_HIERARCHY_LEVEL_PATCH);
	g_assert(v1->variable_type == NAMED);
	cmp = strcmp(v1->name, "evaporation");
	g_assert(cmp == 0);
	OutputFilterVariable *v2 = v1->next;
	g_assert(v2->hierarchy_level == OF_HIERARCHY_LEVEL_STRATUM);
	g_assert(v2->variable_type == NAMED);
	cmp = strcmp(v2->name, "PET");
	g_assert(cmp == 0);
	OutputFilterVariable *v3 = v2->next;
	g_assert(v3->hierarchy_level == OF_HIERARCHY_LEVEL_STRATUM);
	g_assert(v3->variable_type == NAMED);
	cmp = strcmp(v3->name, "cs");
	g_assert(cmp == 0);
	cmp = strcmp(v3->sub_struct_varname, "leafc");
	g_assert(cmp == 0);
	g_assert(v3->next == NULL);

	free(filter);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test output_filter5", test_output_filter5);
	return g_test_run();
}
