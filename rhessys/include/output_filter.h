#ifndef _OUTPUT_FILTER_H_
#define _OUTPUT_FILTER_H_

#include <stdio.h>

#include "types.h"

#define OUTPUT_TIMESTEP_HOURLY "hourly"
#define OUTPUT_TIMESTEP_DAILY "daily"
#define OUTPUT_TIMESTEP_MONTHLY "monthly"
#define OUTPUT_TIMESTEP_YEARLY "yearly"

#define OUTPUT_FORMAT_CSV "csv"
#define OUTPUT_FORMAT_NETCDF "netcdf"


typedef enum {
	TIMESTEP_UNDEFINED,
	TIMESTEP_HOURLY,
	TIMESTEP_DAILY,
	TIMESTEP_MONTHLY,
	TIMESTEP_YEARLY
} OutputFilterTimestep;

typedef enum {
	OUTPUT_TYPE_CSV,
	OUTPUT_TYPE_NETCDF
} OutputFormat;

typedef enum {
	ANY_VAR,
	NAMED
} VariableType;

typedef enum {
	BASIN = 4,
	HILLSLOPE = 3,
	ZONE = 2,
	PATCH = 1
} OutputPatchType;

typedef unsigned short num_elements_t;

typedef struct materialized_variable {
	DataType data_type;
	union {
		bool bool_val;
		char char_val;
		char *char_array;
		int int_val;
		long long_val;
		long *long_array;
		float float_val;
		double double_val;
		double *double_array;
	} u;
} MaterializedVariable;

typedef struct of_output_output {
	OutputFormat format;
	char *path;
	char *filename;
	void *meta;
	MaterializedVariable *materialized_variables;
	FILE *fp;
} OutputFilterOutput;

typedef struct of_var {
	VariableType variable_type;
	struct of_var *next;

	DataType data_type;
	char *name;
	size_t offset;
	void *meta;
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

typedef enum {
	OUTPUT_FILTER_UNDEFINED,
	OUTPUT_FILTER_PATCH,
	OUTPUT_FILTER_CANOPY_STRATA
} OutputFilterType;

typedef struct of_filter {
	OutputFilterType type;
	OutputFilterTimestep timestep;
	struct of_filter *next;
	OutputFilterOutput *output;
	OutputFilterPatch *patches;
	OutputFilterVariable *variables;
	num_elements_t num_named_variables;
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

void print_output_filter_output(OutputFilterOutput *output, char *prefix);
void print_output_filter_patch(OutputFilterPatch *patch, char *prefix);
void print_output_filter_variale(OutputFilterVariable *variable, char *prefix);
void print_output_filter(OutputFilter *filter);

bool return_with_error(char * const error, size_t error_len, char *error_mesg);

#endif

