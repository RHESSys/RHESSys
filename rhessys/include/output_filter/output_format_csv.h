/*
 * output_format_csv.h
 *
 *  Created on: Aug 24, 2020
 *      Author: miles
 */

#ifndef INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_CSV_H_
#define INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_CSV_H_

#include <stdio.h>
#include <stdarg.h>

#include "output_filter.h"

#define OUTPUT_FORMAT_EXT_CSV "csv"

bool output_format_csv_init(OutputFilter * const filter);
bool output_format_csv_destroy(OutputFilter * const filter);
bool output_format_csv_write_headers(OutputFilter * const filter);
bool output_format_csv_write_data(char * const error, size_t error_len,
		FILE *fp, bool flush, MaterializedVariable *vars, num_elements_t nvars);


#endif /* INCLUDE_OUTPUT_FILTER_OUTPUT_FORMAT_CSV_H_ */
