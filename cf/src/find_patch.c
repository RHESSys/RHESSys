/** @file find_patch.c
 *  @brief Find index of patch in flow_table structure given fully-qualified
 *  patch ID (composed of hillslope, zone, and patch ID)
 */
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "fileio.h"
#include "find_patch.h"

int find_patch(int num_patches, struct flow_struct *flow_table,
		int patchID, int zoneID, int hillID) {

	int fnd, inx;

	fnd = 0;
	inx = 1;

	while ( inx <= num_patches ) {
		if ((flow_table[inx].patchID == patchID)
				&& (flow_table[inx].hillID == hillID)
				&& (flow_table[inx].zoneID == zoneID)) {
			fnd = inx;
			break;
		}
		inx += 1;
	}

	return (fnd);

}

