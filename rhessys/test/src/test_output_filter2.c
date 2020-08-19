#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "output_filter.h"

OutputFilter *parse(const char* input);


void test_output_filter2() {
	OutputFilter *filter = parse("test/fixtures/filter2.yml");

	// First filter
	g_assert(filter->next != NULL);
	// Verify output section of filter
	g_assert(filter->output != NULL);
	g_assert(filter->output->timestep == TIMESTEP_DAILY);
	g_assert(filter->output->format == CSV);
	int cmp = strcmp(filter->output->path, "output/fire-project-1");
	g_assert(cmp == 0);
	cmp = strcmp(filter->output->filename, "scenario-abc1");
	g_assert(cmp == 0);
	// Verify patch section of filter
	// IDs
	OutputFilterPatch *p1 = filter->patches;
	g_assert(p1->output_patch_type == PATCH);
	g_assert(p1->basinID == 1);
	g_assert(p1->hillslopeID == 2);
	g_assert(p1->zoneID == 3);
	g_assert(p1->patchID == 4);
	g_assert(p1->next != NULL);
	OutputFilterPatch *p2 = p1->next;
	g_assert(p2->basinID == 2);
	g_assert(p2->next == NULL);
	// Variables
	OutputFilterVariable *v1 = filter->variables;
	g_assert(v1->variable_type == NAMED);
	cmp = strcmp(v1->name, "water_balance");
	g_assert(cmp == 0);
	OutputFilterVariable *v2 = v1->next;
	g_assert(v1->variable_type == NAMED);
	cmp = strcmp(v1->name, "Qout");
	OutputFilterVariable *v3 = v2->next;
	g_assert(v3->variable_type == NAMED);
	cmp = strcmp(v3->name, "Qin");
	g_assert(v3->next == NULL);

	// Second filter
	OutputFilter *filter2 = filter->next;
	g_assert(filter2->next != NULL);
	// Verify output section of filter
	g_assert(filter2->output != NULL);
	g_assert(filter2->output->timestep == TIMESTEP_MONTHLY);
	g_assert(filter2->output->format == NETCDF);
	cmp = strcmp(filter2->output->path, "output/fire-project-1");
	g_assert(cmp == 0);
	cmp = strcmp(filter2->output->filename, "scenario-abc2");
	g_assert(cmp == 0);
	// Verify patch section of filter
	// IDs
	p1 = filter2->patches;
	g_assert(p1->output_patch_type == ALL_PATCHES);
	g_assert(p1->next == NULL);
	// Variables
	v1 = filter2->variables;
	g_assert(v1->variable_type == NAMED);
	cmp = strcmp(v1->name, "foo");
	g_assert(cmp == 0);
	v2 = v1->next;
	g_assert(v1->variable_type == NAMED);
	cmp = strcmp(v1->name, "bar");

	// Third filter
	OutputFilter *filter3 = filter2->next;
	g_assert(filter3->next == NULL);
	// Verify output section of filter
	g_assert(filter3->output != NULL);
	g_assert(filter3->output->timestep == TIMESTEP_YEARLY);
	g_assert(filter3->output->format == CSV);
	cmp = strcmp(filter3->output->path, "output/fire-project-1");
	g_assert(cmp == 0);
	cmp = strcmp(filter3->output->filename, "scenario-abc3");
	g_assert(cmp == 0);
	// Verify patch section of filter
	// IDs
	p1 = filter3->patches;
	g_assert(p1->output_patch_type == ZONE);
	g_assert(p1->basinID == 4);
	g_assert(p1->hillslopeID == 3);
	g_assert(p1->zoneID == 4);
	g_assert(p1->next == NULL);
	// Variables
	v1 = filter3->variables;
	g_assert(v1->variable_type == ANY_VAR);
	g_assert(v1->next == NULL);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test output_filter2", test_output_filter2);
	return g_test_run();
}
