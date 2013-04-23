#ifndef _NORMALIZE_ROOF_PATCHES_H_
#define _NORMALIZE_ROOF_PATCHES_H_

#include "blender.h"
#include "util.h"

/// @brief Function to remove duplicates and normalize receiver gammas for all roof patches
bool normalize_roof_patches(
    struct flow_struct* _flow_table, // The flow table
    int _num_patches);		     // The number of patches in the flow table

#endif // _NORMALIZE_ROOF_PATCHES_H_
