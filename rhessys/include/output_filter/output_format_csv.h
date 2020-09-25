#ifndef INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_CSV_H_
#define INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_CSV_H_

#include <stdio.h>
#include <stdarg.h>

#include "output_filter.h"
#include "output_filter/output_filter_output.h"

#define OUTPUT_FORMAT_EXT_CSV "csv"

#define CSV_DELIM_NONE ""
#define CSV_DELIM_DEFAULT ","
#define CSV_EOL "\n"

#define CSV_HEADER_YEAR "year"
#define CSV_HEADER_MONTH "month"
#define CSV_HEADER_DAY "day"
#define CSV_HEADER_HOUR "hour"


bool output_format_csv_init(OutputFilter * const filter);
bool output_format_csv_destroy(OutputFilter * const filter);
bool output_format_csv_write_headers(OutputFilter * const filter);
bool output_format_csv_write_data(char * const error, size_t error_len,
		struct date date, OutputFilter * const filter,
		EntityID id, MaterializedVariable * const vars, bool flush);

#endif /* INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_CSV_H_ */
