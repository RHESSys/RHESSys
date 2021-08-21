/*
 * construct_output_filter.h
 *
 *  Created on: Aug 23, 2020
 *      Author: miles
 */

#ifndef INCLUDE_OUTPUT_FILTER_CONSTRUCT_OUTPUT_FILTER_H_
#define INCLUDE_OUTPUT_FILTER_CONSTRUCT_OUTPUT_FILTER_H_

#include "../rhessys.h"

bool construct_output_filter(char * const error, size_t error_len,
		struct command_line_object * const command_line,
		struct world_object * const world);

#endif /* INCLUDE_OUTPUT_FILTER_CONSTRUCT_OUTPUT_FILTER_H_ */
