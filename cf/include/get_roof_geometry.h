/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#ifndef _GET_ROOF_GEOMETRY_H_
#define _GET_ROOF_GEOMETRY_H_

#include "util.h"
#include "roof_geometry.h"

/// @brief Recursively build up the geometry for a roof in grid squares.
extern bool get_roof_geometry(
    const double* _roofs, // The array of proportional contributions of flow to impervious surfaces for a roof square
    int _grid_index, // Index of the roof square from which we are expanding
    int _maxr,       // The number of rows in the grid
    int _maxc,       // The number of columns in the grid
    bool* _roof_processed, // An array of flags indicating whether we have visited a square already
    roof_geometry_t* _roof_geometry); // The geometry for the indexed roof square

#endif // _GET_ROOF_GEOMETRY_H_
