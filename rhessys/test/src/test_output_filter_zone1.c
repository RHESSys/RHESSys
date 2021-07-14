#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "output_filter.h"

OutputFilter *parse(const char* input, bool verbose);

void test_output_filter1() {
	OutputFilter *filter = parse("fixtures/filter_zone1.yml", true);

	print_output_filter(filter);

	g_assert(filter->next == NULL);
	// Verify timestep
	g_assert(filter->timestep == TIMESTEP_DAILY);
	// Verify output filter type
	g_assert(filter->type == OUTPUT_FILTER_ZONE);
	// Verify output section of filter
	g_assert(filter->output != NULL);
	g_assert(filter->output->format == OUTPUT_TYPE_CSV);
	int cmp = strcmp(filter->output->path, "../output/fire-project-1");
	g_assert(cmp == 0);
	cmp = strcmp(filter->output->filename, "scenario-abc1");
	g_assert(cmp == 0);
	// Verify zone section of filter
	// IDs
	OutputFilterZone *z1 = filter->zones;
	g_assert(z1->output_zone_type == ZONE_TYPE_ZONE);
	g_assert(z1->basinID == 1);
	g_assert(z1->hillslopeID == 2);
	g_assert(z1->zoneID == 3);
	OutputFilterZone *z2 = z1->next;
	g_assert(z2->output_zone_type == ZONE_TYPE_BASIN);
	g_assert(z2->basinID == 2);
	g_assert(z2->next == NULL);
	// Variables
	OutputFilterVariable *v1 = filter->variables;
	g_assert(v1->hierarchy_level == OF_HIERARCHY_LEVEL_ZONE);
	g_assert(v1->variable_type == NAMED);
	cmp = strcmp(v1->name, "PAR_direct");
	g_assert(cmp == 0);

	OutputFilterVariable *v2 = v1->next;
	g_assert(v2->hierarchy_level == OF_HIERARCHY_LEVEL_ZONE);
	g_assert(v2->variable_type == NAMED);
	cmp = strcmp(v2->name, "PAR_diffuse");

	OutputFilterVariable *v3 = v2->next;
	g_assert(v3->hierarchy_level == OF_HIERARCHY_LEVEL_ZONE);
	g_assert(v3->variable_type == VAR_TYPE_EXPR);
	cmp = strcmp(v3->name, "par");

	OutputFilterVariable *v4 = v3->next;
	g_assert(v4->hierarchy_level == OF_HIERARCHY_LEVEL_ZONE);
	g_assert(v4->variable_type == NAMED);
	cmp = strcmp(v4->name, "metv");
	g_assert(cmp == 0);
	cmp = strcmp(v4->sub_struct_varname, "tmax");
	g_assert(cmp == 0);

	OutputFilterVariable *v5 = v4->next;
	g_assert(v5->hierarchy_level == OF_HIERARCHY_LEVEL_ZONE);
	g_assert(v5->variable_type == NAMED);
	cmp = strcmp(v5->name, "metv");
	g_assert(cmp == 0);
	cmp = strcmp(v5->sub_struct_varname, "vpd_night");
	g_assert(cmp == 0);
	g_assert(v5->next == NULL);

	free(filter);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test output_filter1", test_output_filter1);
	return g_test_run();
}
