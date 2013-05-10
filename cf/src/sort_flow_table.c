/** @file sort_flow_table.c
 *  @brief sorts patches by elevation generates ID's based on sort list position
 *	finds maximum partition list ID
 */
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "patch_hash_table.h"

int _comparatorDesc(const void *a, const void *b) {
	// Descending sort
	struct flow_struct *flowA = (struct flow_struct *)a;
	struct flow_struct *flowB = (struct flow_struct *)b;
	if ( flowA->z > flowB->z ) return -1;
	else if (flowA->z < flowB->z) return 1;
	return 0; // Equal
}

struct ID_struct sort_flow_table(struct flow_struct *flow_table, int num_patches,
		PatchTable_t *patchTable) {

	struct ID_struct max_ID;

	// Sort using stdlib's merge sort implementation
	//   merge sort is a stable sort, as was the original
	//   bubble sort, so merge sort is preferable
	//   to quick sort and just as fast. If a non-stable
	//   sort is used, flow tables may not match those created
	//   with older version of CF.
	// Note: we don't use index 0 in the flow table, so we need to
	//   increment by one the flow table pointer we pass in to the
	//   sort function.
	mergesort(flow_table+1, num_patches, sizeof(struct flow_struct), _comparatorDesc);

	// Determine max patch ID and update patch hash table
	max_ID.patch = 0;
	max_ID.zone = 0;
	max_ID.hill = 0;

	PatchKey_t k;
	//int tableI;
	for (int i = 1; i <= num_patches; i++) {
		// Determine max IDs
		if (flow_table[i].patchID > max_ID.patch)
			max_ID.patch = flow_table[i].patchID;
		if (flow_table[i].zoneID > max_ID.zone)
			max_ID.zone = flow_table[i].zoneID;
		if (flow_table[i].hillID > max_ID.hill)
			max_ID.hill = flow_table[i].hillID;
		flow_table[i].ID_order = i;

		// Update patch hash table
		//tableI = i - 1;
		k.patchID = flow_table[i].patchID;
		k.zoneID = flow_table[i].zoneID;
		k.hillID = flow_table[i].hillID;
		patchHashTableInsert(patchTable, k, i);
	}

	return (max_ID);

}


