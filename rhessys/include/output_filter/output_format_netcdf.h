#ifndef INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_NETCDF_H_
#define INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_NETCDF_H_

#include <stdio.h>
#include <stdarg.h>

#include "types.h"
#include "output_filter.h"
#include "output_filter/output_filter_output.h"

#define OUTPUT_FORMAT_EXT_NETCDF "nc"
#define INVALID_TYPE -1
#define OF_DIM_VECTOR 1

#define OF_DIMENSION_IDX "index"
#define OF_VAR_YEAR "year"
#define OF_VAR_MONTH "month"
#define OF_VAR_DAY "day"
#define OF_VAR_HOUR "hour"
#define OF_VAR_BASIN "basinID"
#define OF_VAR_HILL "hillID"
#define OF_VAR_ZONE "zoneID"
#define OF_VAR_PATCH "patchID"
#define OF_VAR_STRATUM "stratumID"



typedef struct of_fmt_netcdf_meta {
	char *abs_path;
	int ncid;
	int dim_idx_id;
	int var_idx_id;
	int var_time_hour_id;
	int var_time_day_id;
	int var_time_month_id;
	int var_time_year_id;
	int var_id_basin_id;
	int var_id_hill_id;
	int var_id_zone_id;
	int var_id_patch_id;
	int var_id_stratum_id;
	int index;
} OutputFormatNetCDFMetadata;

typedef struct of_fmt_netcdf_var_meta {
	int varid;
} OutputFormatNetCDFVariableMetadata;

int get_netcdf_data_type(DataType type);

bool output_format_netcdf_init(OutputFilter * const filter);
bool output_format_netcdf_destroy(OutputFilter * const filter);
bool output_format_netcdf_write_headers(OutputFilter * const filter);
bool output_format_netcdf_write_data(char * const error, size_t error_len,
		struct date date, OutputFilter * const filter,
		EntityID id, MaterializedVariable * const vars, bool flush);

#endif /* INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_NETCDF_H_ */
