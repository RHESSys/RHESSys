#ifndef INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_NETCDF_H_
#define INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_NETCDF_H_

#include <stdio.h>
#include <stdarg.h>

#include "types.h"
#include "output_filter.h"
#include "output_filter/output_filter_output.h"

#define OUTPUT_FORMAT_EXT_NETCDF "nc"
#define INVALID_TYPE -1
#define OF_DIMENSION_TIME "time"


typedef struct of_fmt_netcdf_meta {
	char *abs_path;
	int ncid;
	int dim_time_id;
	int var_time_id;
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
