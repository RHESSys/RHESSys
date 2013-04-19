#include "get_roof_geometry.h"

bool get_roof_geometry(
    const double* _roofs,
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
    } else if(!_roof_processed[_grid_index] && is_roof(_roofs[_grid_index])) {
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

