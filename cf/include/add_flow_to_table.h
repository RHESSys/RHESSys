#ifndef _ADD_FLOW_TO_TABLE_H_
#define _ADD_FLOW_TO_TABLE_H_

#include "util.h"
#include "patch_hash_table.h"

/// @brief Adds a flow entry to the flow table
extern bool add_flow_to_table(
    int _contributor_row,	// The row of the contributing square (pixel)
    int _contributor_col,	// The column of the contributing square
    int _receiver_row,		// The row of the receiving square
    int _receiver_col,		// The column of the receiving square
    int _maxr,			// The max rows
    int _maxc,			// The max columns
    struct flow_struct* _flow_table, // The flow table
    int _num_patches, // The number of patches in the flow table
    PatchTable_t *patchTable, // Hash table to speed lookups of flow table indices
    const int* _patch,		     // The map of squares to patch ids
    const int* _hill,		     // The map of square to hill ids
    const int* _zone,		     // The map of square to zone ids
    double _proportion,			 // The proportion of the contributors gamma to assign to the receiver
    int* const _receiver_out);	 // Map of cells that actually received flow. Value of each cell represents number of contributor cells.

#endif // _ADD_FLOW_TO_TABLE_H_
