#ifndef _VERIFY_H_
#define _VERIFY_H_

#include "blender.h"
#include "util.h"

/// @brief Verifies the number of adjacencies of a patch and tne num_adjacent field of the patch are equal.
bool verify_num_adjacent_for_patch(
    struct flow_struct* _flow_table, // The flow table
    int _patch);		     // The index of the patch in the flow table

/// @brief Verifies that the number of adjacencies and num_adjacent fields are equal for all patches.
bool verify_num_adjacent(
    struct flow_struct* _flow_table, // The flow table
    int _num_patches);		     // The number of patches in the flow table

#endif // _VERIFY_H_
