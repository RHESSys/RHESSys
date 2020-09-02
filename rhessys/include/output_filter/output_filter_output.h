/*
 * output_filter_output.h
 *
 *  Created on: Aug 30, 2020
 *      Author: miles
 */

#ifndef INCLUDE_OUTPUT_FILTER_OUTPUT_FILTER_OUTPUT_H_
#define INCLUDE_OUTPUT_FILTER_OUTPUT_FILTER_OUTPUT_H_

#include "output_filter.h"

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

MaterializedVariable *alloc_materialized_variable_array(size_t num_elements);

bool output_filter_output_daily(char * const error, size_t error_len, OutputFilter * const filter);

#endif /* INCLUDE_OUTPUT_FILTER_OUTPUT_FILTER_OUTPUT_H_ */
