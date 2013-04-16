#include <stdio.h>

#include "blender.h"
#include "route_roofs.h"
#include "roof_geometry.h"
#include "util.h"

/// @brief Recursively build up the geometry for a roof in grid squares.
bool get_roof_geometry(
    double* _roofs,
    int _grid_index,
    int _maxr,
    int _maxc,
    bool* _roof_processed,
    roof_geometry_t* _roof_geometry)
{
    bool result = true;
    int row = 0;
    int col = 0;
    if(_roofs == 0) {
	fprintf(stderr, "ERROR: Roof map pointer is NULL.\n");
	result = false;
    } else if(_roof_processed == 0) {
	fprintf(stderr, "ERROR: Roof processed flag array pointer is NULL.\n");
	result = false;
    } else if(_roof_geometry == 0) {
	fprintf(stderr, "ERROR: Roof geometry pointer is NULL.\n");
	result = false;
    } else if(!index_to_row_col(_grid_index, _maxr, _maxc, &row, &col)) {
	fprintf(stderr, "ERROR: Unable to decompose index into row and column.\n");
	result = false;
    } else if(!_roof_processed[_grid_index] && _roofs[_grid_index] != NO_DATA) {
	if(!add_roof_square(_roof_geometry, row, col)) {
	    fprintf(stderr, "ERROR: Failed to add the roof square at %d, %d to the roof geometry.\n", row, col);
	    result = false;
	} else {
	    _roof_processed[_grid_index] = true;
	    int new_row = 0;
	    int new_col = 0;
	    int index = 0;
	    new_row = row - 1;
	    new_col = col;
	    if(result && row_col_to_index(new_row, new_col, _maxr, _maxc, &index)) {
		if(!get_roof_geometry(_roofs, index, _maxr, _maxc, _roof_processed, _roof_geometry)) {
		    fprintf(stderr, "ERROR Get roof geometry failed.\n");
		    result = false;
		}
	    }
	    new_row = row;
	    new_col = col - 1;
	    if(result && row_col_to_index(new_row, new_col, _maxr, _maxc, &index)) {
		if(!get_roof_geometry(_roofs, index, _maxr, _maxc, _roof_processed, _roof_geometry)) {
		    fprintf(stderr, "ERROR Get roof geometry failed.\n");
		    result = false;
		}
	    }
	    new_row = row ;
	    new_col = col + 1;
	    if(result && row_col_to_index(new_row, new_col, _maxr, _maxc, &index)) {
		if(!get_roof_geometry(_roofs, index, _maxr, _maxc, _roof_processed, _roof_geometry)) {
		    fprintf(stderr, "ERROR Get roof geometry failed.\n");
		    result = false;
		}
	    }
	    new_row = row + 1;
	    new_col = col;
	    if(result && row_col_to_index(new_row, new_col, _maxr, _maxc, &index)) {
		if(!get_roof_geometry(_roofs, index, _maxr, _maxc, _roof_processed, _roof_geometry)) {
		    fprintf(stderr, "ERROR Get roof geometry failed.\n");
		    result = false;
		}
	    }
	}
    }
    return result;
}

bool compute_roof_non_connected_routing(
    struct flow_struct* _flow_table,
    const roof_geometry_t* _roof_geometry,
    int _maxr,
    int _maxc)
{
    bool result = true;
    return result;
}

bool compute_roof_connected_routing(
    struct flow_struct* _flow_table,
    const roof_geometry_t* _roof_geometry,
    int _maxr,
    int _maxc)
{
    bool result = true;
    return result;
}

/// @brief function to short circuit roof patches to road (impervious) patches
bool route_roofs_to_roads(
    struct flow_struct* _flow_table,
    double* _roofs,
    int _maxr,
    int _maxc)
{
    bool result = true;
    int num_squares = _maxr * _maxc;
    bool roof_processed[num_squares];
    for(int index = 0; result && index < num_squares; ++index) {
	if(_roofs[index] != NO_DATA && roof_processed[index] != true) {

	    /* Construct a new roof geometry */
	    roof_geometry_t* roof_geometry;
	    if(!construct_roof_geometry(&roof_geometry)) {
		fprintf(stderr, "ERROR: Failed to construct a roof geometry.\n");
		result = false;
	    }
	    
	    // Compute the geometry of the entire contiguous roof
	    else if(!get_roof_geometry(_roofs, index, _maxr, _maxc, roof_processed, roof_geometry)) {
		fprintf(stderr, "ERROR: failed to get roof geometry for square index: %d\n", index);
		result = false;
	    }

	    // Compute routing for the non-connected (pervious) flow
	    else if(!compute_roof_non_connected_routing(_flow_table, roof_geometry, _maxr, _maxc)) {
		fprintf(stderr, "ERROR: failed to perform non-connected roof routing");
		result = false;
	    }

	    // Compute routing for the connected (impervious) flow
	    else if(!compute_roof_connected_routing(_flow_table, roof_geometry, _maxr, _maxc)) {
		fprintf(stderr, "ERROR: failed to perform the connected roof routing");
		result = false;
	    }

	    // Destory the roof geometry
	    else if(destroy_roof_geometry(&roof_geometry)) {
		fprintf(stderr, "ERROR: Failed to destroy the roof geometry.\n");
		result = false;
	    }
	}
    }
    return result;
}
