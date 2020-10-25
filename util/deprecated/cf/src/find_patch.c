/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/** @file find_patch.c
 *  @brief Find index of patch in flow_table structure given fully-qualified
 *  patch ID (composed of hillslope, zone, and patch ID).  Assumes that
 *  PatchTable_t was previously initialized
 */
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "fileio.h"
#include "find_patch.h"
#include "patch_hash_table.h"

int find_patch(PatchTable_t *patchTable, int patchID, int zoneID, int hillID) {
	PatchKey_t k = { patchID, zoneID, hillID };
	return (int)patchHashTableGet(patchTable, k);
}

