#ifndef INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_NETCDF_H_
#define INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_NETCDF_H_

#include <stdio.h>
#include <stdarg.h>

#include "output_filter.h"
#include "output_filter/output_filter_output.h"

#define OUTPUT_FORMAT_EXT_NETCDF "nc"

#define NC_HEADER_YEAR "year"
#define NC_HEADER_MONTH "month"
#define NC_HEADER_DAY "day"
#define NC_HEADER_HOUR "hour"


bool output_format_netcdf_init(OutputFilter * const filter);
bool output_format_netcdf_destroy(OutputFilter * const filter);
bool output_format_netcdf_write_headers(OutputFilter * const filter);
bool output_format_netcdf_write_data(char * const error, size_t error_len,
		struct date date, OutputFilter * const filter,
		EntityID id, MaterializedVariable * const vars, bool flush);

#endif /* INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_NETCDF_H_ */
