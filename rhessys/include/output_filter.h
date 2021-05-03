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

#define OF_VAR_EXPR_AST_NODE_UNARY_MINUS 'M'
#define OF_VAR_EXPR_AST_NODE_CONST 'K'
#define OF_VAR_EXPR_AST_NODE_NAME 'N'

#define FILENAME_LEN 255

typedef enum {
	TIMESTEP_UNDEFINED,
	TIMESTEP_HOURLY,
	TIMESTEP_DAILY,
	TIMESTEP_MONTHLY,
	TIMESTEP_YEARLY
} OutputFilterTimestep;

typedef enum {
	OF_HIERARCHY_LEVEL_UNDEFINED,
	OF_HIERARCHY_LEVEL_BASIN,
	OF_HIERARCHY_LEVEL_HILLSLOPE,
	OF_HIERARCHY_LEVEL_ZONE,
	OF_HIERARCHY_LEVEL_PATCH,
	OF_HIERARCHY_LEVEL_STRATUM
} HierarchyLevel;

typedef enum {
	OUTPUT_TYPE_CSV,
	OUTPUT_TYPE_NETCDF
} OutputFormat;

typedef enum {
	ANY_VAR,
	NAMED,
	VAR_TYPE_EXPR
} VariableType;

typedef enum {
	ZONE_TYPE_BASIN = 3,
	ZONE_TYPE_HILLSLOPE = 2,
	ZONE_TYPE_ZONE = 1,
} OutputZoneType;

typedef enum {
	PATCH_TYPE_BASIN = 4,
	PATCH_TYPE_HILLSLOPE = 3,
	PATCH_TYPE_ZONE = 2,
	PATCH_TYPE_PATCH = 1
} OutputPatchType;

typedef enum {
	STRATUM_TYPE_BASIN = 5,
	STRATUM_TYPE_HILLSLOPE = 4,
	STRATUM_TYPE_ZONE = 3,
	STRATUM_TYPE_PATCH = 2,
	STRATUM_TYPE_STRATUM = 1
} OutputStratumType;

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
	void *meta;
} MaterializedVariable;

typedef struct of_output_output {
	OutputFormat format;
	char *path;
	char *filename;
	void *meta;
	MaterializedVariable *materialized_variables;
	FILE *fp;
} OutputFilterOutput;

// output_filter_variable_list
typedef struct of_var {
	VariableType variable_type;
	HierarchyLevel hierarchy_level;
	struct of_var *next;

	DataType data_type;
	char *name;
	char *sub_struct_varname;
	size_t offset;
	size_t sub_struct_var_offset;
	struct of_var_expr_ast *expr;
	void *meta;
} OutputFilterVariable;

// output_filter_basin_list
typedef struct of_basin {
	struct of_basin *next;

	int basinID;
	struct basin_object *basin;
} OutputFilterBasin;

// output_filter_zone_list
typedef struct of_zone {
	OutputZoneType output_zone_type;
	struct of_zone *next;

	int basinID;
	struct basin_object *basin;

	int hillslopeID;
	struct hillslope_object *hill;

	int zoneID;
	struct zone_object *zone;
} OutputFilterZone;

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

// output_filter_canopy_strata_list
typedef struct of_stratum {
	OutputStratumType output_stratum_type;
	struct of_stratum *next;

	int basinID;
	struct basin_object *basin;

	int hillslopeID;
	struct hillslope_object *hill;

	int zoneID;
	struct zone_object *zone;

	int patchID;
	struct patch_object *patch;

	int stratumID;
	struct canopy_strata_object *stratum;
} OutputFilterStratum;

typedef enum {
	OUTPUT_FILTER_UNDEFINED,
	OUTPUT_FILTER_BASIN,
	OUTPUT_FILTER_ZONE,
	OUTPUT_FILTER_PATCH,
	OUTPUT_FILTER_CANOPY_STRATUM
} OutputFilterType;

typedef struct of_filter {
	OutputFilterType type;
	OutputFilterTimestep timestep;
	struct of_filter *next;
	OutputFilterOutput *output;
	OutputFilterBasin *basins;
	OutputFilterZone *zones;
	OutputFilterPatch *patches;
	OutputFilterStratum *strata;
	OutputFilterVariable *variables;
	num_elements_t num_variables;
	bool parse_error;
} OutputFilter;

typedef struct of_var_expr_ast {
    int nodetype;
    struct of_var_expr_ast *l;
    struct of_var_expr_ast *r;
} OutputFilterExprAst;

typedef struct of_var_expr_numval {
    int nodetype;   /* type K for constant */
    double number;
} OutputFilterExprNumval;

typedef struct of_var_expr_name {
    int nodetype;  /* type N for name */
    OutputFilterVariable *var;
} OutputFilterExprName;

OutputFilterExprAst *new_of_expr_ast(int nodetype, OutputFilterExprAst *l, OutputFilterExprAst *r);
OutputFilterExprAst *new_of_expr_const(double d);
OutputFilterExprName *new_of_expr_name(OutputFilterVariable *var);
void free_of_expr_ast(OutputFilterExprAst *ast);
void print_of_expr_ast(OutputFilterExprAst *ast, int level);

OutputFilterBasin *create_new_output_filter_basin();
OutputFilterBasin *add_to_output_filter_basin_list(OutputFilterBasin * const head,
		OutputFilterBasin * const new_basin);
void free_output_filter_basin_list(OutputFilterBasin *head);

OutputFilterPatch *create_new_output_filter_patch();
OutputFilterPatch *add_to_output_filter_patch_list(OutputFilterPatch * const head,
		OutputFilterPatch * const new_patch);
void free_output_filter_patch_list(OutputFilterPatch *head);

OutputFilterStratum *create_new_output_filter_stratum();
OutputFilterStratum *add_to_output_filter_stratum_list(OutputFilterStratum * const head,
		OutputFilterStratum * const new_stratum);
void free_output_filter_stratum_list(OutputFilterStratum *head);

OutputFilterVariable *create_new_output_filter_variable(HierarchyLevel level, char *name);
OutputFilterVariable *create_new_output_filter_sub_struct_variable(HierarchyLevel level, char *name,
                                                                   char *sub_struct_varname);
OutputFilterVariable *create_new_output_filter_variable_any();
OutputFilterVariable *create_new_output_filter_expr_variable(HierarchyLevel level, char *name,
                                                             OutputFilterExprAst *expr);
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
void print_output_filter_stratum(OutputFilterStratum *stratum, char *prefix);
void print_output_filter_variale(OutputFilterVariable *variable, char *prefix);
void print_output_filter(OutputFilter *filter);

bool return_with_error(char * const error, size_t error_len, char *error_mesg);

#endif

