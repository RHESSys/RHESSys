/*
 * output_filter_output.h
 *
 *  Created on: Aug 30, 2020
 *      Author: miles
 */

#ifndef INCLUDE_OUTPUT_FILTER_OUTPUT_FILTER_OUTPUT_H_
#define INCLUDE_OUTPUT_FILTER_OUTPUT_FILTER_OUTPUT_H_

#include "output_filter.h"

#define OUTPUT_FILTER_ID_EMPTY -1

typedef struct entity_id_s {
	int basin_ID;
	int hillslope_ID;
	int zone_ID;
	int patch_ID;
	int canopy_strata_ID;
} EntityID;

bool output_filter_output_daily(char * const error, size_t error_len,
		struct date date, OutputFilter * const filters);

#endif /* INCLUDE_OUTPUT_FILTER_OUTPUT_FILTER_OUTPUT_H_ */
