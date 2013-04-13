/** @file zero_flow_table.h
 *  @brief Find index of patch in flow_table structure given fully-qualified
 *  patch ID (composed of hillslope, zone, and patch ID)
 */
#ifndef FIND_PATCH_H
#define FIND_PATCH_H

#include "blender.h"

/** @brief Find index of patch in flow_table structure given fully-qualified
 *  patch ID (composed of hillslope, zone, and patch ID)
 *
 * 	@param num_patches Int, The maximum number of patches in the flow table structure to search
 * 	@param flow_table Pointer to memory allocated to store an array of struct flow_table
 * 	@param patchID Int, ID of current patch
 * 	@param zoneID Int, zone ID of current patch
 * 	@param hillID Int, hillslope ID of current patch
 */
int find_patch(int num_patches, struct flow_struct *flow_table,
		int patchID, int zoneID, int hillID);

#endif
