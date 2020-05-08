/** @file zero_flow_table.h
 *  @brief Find index of patch in flow_table structure given fully-qualified
 *  patch ID (composed of hillslope, zone, and patch ID)
 */
#ifndef FIND_PATCH_H
#define FIND_PATCH_H

#include "blender.h"
#include "patch_hash_table.h"

/** @brief Find index of patch in flow_table structure given fully-qualified
 *  patch ID (composed of hillslope, zone, and patch ID)
 *
 *	@param patchTable Hash table mapping fully qualified patch ID to flow
 *			table indices
 * 	@param patchID Int, ID of current patch
 * 	@param zoneID Int, zone ID of current patch
 * 	@param hillID Int, hillslope ID of current patch
 *
 * 	@return The index in the flow table of the patch
 */
int find_patch(PatchTable_t *patchTable, int patchID, int zoneID, int hillID);

#endif
