/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#ifndef _COMPUTE_CONNECTED_H_
#define _COMPUTE_CONNECTED_H_

#include "patch_hash_table.h"

/// @brief Finds the nearest pervious surface and adds the appropriate entry to the flow table
extern bool compute_roof_connected_routing(
    struct flow_struct* _flow_table, // The flow table
    int _num_patches,                // The number of patches in the flow table
    PatchTable_t *_patchTable,		 // Hash table to speed lookups of flow table indices
    roof_geometry_t* _roof_geometry, // The geometry of the current roof
    const double* _roofs,            // The roof raster array
    const int* _impervious,          // The impervious surface raster array
    const int* _patch,               // The map of pixels to patch ids
    const int* _hill,                // The map of square to hill ids
    const int* _zone,                // The map of the square to zone ids
    int _maxr,                       // The max rows
    int _maxc);                      // The max columns

#endif // _COMPUTE_CONNECTED_H_
