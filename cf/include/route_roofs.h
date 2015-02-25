#ifndef _ROUTE_ROOFS_TO_ROADS_H_
#define _ROUTE_ROOFS_TO_ROADS_H_

#include "blender.h"
#include "util.h"
#include "patch_hash_table.h"

/// @brief function to account for roof contribution to impervious patches
extern bool route_roofs_to_roads(
    struct flow_struct* _flow_table, // The flow table for updating the flowy bits
    int _num_patches,		     // The number of patches in the flow table
    PatchTable_t *_patchTable, // Hash table to speed lookups of flow table indices
    const double* _roofs, // The array of the contribution of roof squares to the nearest impervous surface
    const int* _impervious, // The array of flags indicating whether a square is an impervious surface
    const int* _stream,     // The map of stream pixels
    const int* _priority, // Priority flow receiver raster array
    const double* _elevation, // The digital elevation model (DEM) raster array
    int priorityWeight, // Weight to give priority flow receivers
    const int* _patch,	    // The map of pixels to patch ids
    const int* _hill,	    // The map of squares to hill ids
    const int* _zone,	    // The map of squares to zone ids
    int _maxr,		    // The maximum row
    int _maxc,		    // The maximum column
    int* const _receiver_out); // Map of cells that actually received flow. Value of each cell represents number of contributor cells.

#endif // _ROUTE_ROOFS_TO_ROADS_H_
