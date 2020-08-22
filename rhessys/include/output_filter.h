#ifndef _OUTPUT_FILTER_H_
#define _OUTPUT_FILTER_H_

#include "types.h"

#define OUTPUT_TIMESTEP_HOURLY "hourly"
#define OUTPUT_TIMESTEP_DAILY "daily"
#define OUTPUT_TIMESTEP_MONTHLY "monthly"
#define OUTPUT_TIMESTEP_YEARLY "yearly"

#define OUTPUT_FORMAT_CSV "csv"
#define OUTPUT_FORMAT_NETCDF "netcdf"


typedef enum {
	TIMESTEP_HOURLY,
	TIMESTEP_DAILY,
	TIMESTEP_MONTHLY,
	TIMESTEP_YEARLY
} OutputFormatTimestep;

typedef enum {
	CSV,
	NETCDF
} OutputFormat;

typedef enum {
	ANY_VAR,
	NAMED
} VariableType;

typedef enum {
	ALL_PATCHES = 5,
	BASIN = 4,
	HILLSLOPE = 3,
	ZONE = 2,
	PATCH = 1
} OutputPatchType;

typedef struct of_output_output {
	OutputFormatTimestep timestep;
	OutputFormat format;
	char *path;
	char *filename;
} OutputFilterOutput;

typedef struct of_var {
	VariableType variable_type;
	struct of_var *next;

	char *name;
	size_t offset;
} OutputFilterVariable;

// output_filter_patch_list
typedef struct of_patch {
	OutputPatchType output_patch_type;
	struct of_patch *next;

	int basinID;
	struct basin_object *basin;

	int hillslopeID;
	struct hillslope_object *hill;

	int zoneID;
	struct zone_object *zone;

	int patchID;
	struct patch_object *patch;
} OutputFilterPatch;

typedef struct of_filter {
	struct of_filter *next;
	OutputFilterOutput *output;
	OutputFilterPatch *patches;
	OutputFilterVariable *variables;
	bool parse_error;
} OutputFilter;

OutputFilterPatch *create_new_output_filter_patch();
OutputFilterPatch *add_to_output_filter_patch_list(OutputFilterPatch * const head,
		OutputFilterPatch * const new_patch);
void free_output_filter_patch_list(OutputFilterPatch *head);

OutputFilterVariable *create_new_output_filter_variable(char *name);
OutputFilterVariable *create_new_output_filter_variable_any();
OutputFilterVariable *add_to_output_filter_variable_list(OutputFilterVariable * const head,
		OutputFilterVariable * const new_var);
void free_output_filter_variable_list(OutputFilterVariable *head);

OutputFilterOutput *create_new_output_filter_output();
void free_output_filter_output(OutputFilterOutput *output);

OutputFilter *create_new_output_filter();
OutputFilter *add_to_output_filter_list(OutputFilter * const head,
		OutputFilter * const new_filter);
void free_output_filter(OutputFilter *filter);

#endif

