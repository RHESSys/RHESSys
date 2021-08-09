/** @file zero_flow_table.h
 *  @brief Initialize flow table structure
 */
#ifndef ZERO_FLOW_TABLE_H
#define ZERO_FLOW_TABLE_H

#include "blender.h"

/** @brief Initialize flow table structure. Will modify flow table structure
 * pointed to by flow_table parameter.
 *
 * 	@param flow_table Pointer to memory allocated to store an array of struct flow_table
 * 	@param maxr Int, the maximum index of rows in the study area
 * 	@param maxc Int, the maximum index of columns in the study area
 */
void zero_flow_table(struct flow_struct *flow_table, int maxr, int maxc);

#endif
