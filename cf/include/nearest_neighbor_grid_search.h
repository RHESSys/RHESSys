#ifndef _NEAREST_NEIGHBOR_GRID_SEARCH_H_
#define _NEAREST_NEIGHBOR_GRID_SEARCH_H_

#include "util.h"

#define NEAREST_NEIGHBOR_GRID_SEARCH_MAX_DIST 20

/// @brief Interface for the search comparison callback function.
typedef bool (*search_predicate_t)(
    int,			// The row
    int,			// The column
    void*,			// Pointer to the predicate context
    int*);			// Returned vote value

/// @brief Do a nearest neighbor search over the grid squares using a callback to determine when a square is selected.
/// @note Search is limited to cardinal directions
bool grid_search(
    int _max_dist,		// The max distance out from which the search will look before giving up.
    int _start_row,		// The row of the starting square of the search
    int _start_col,		// The column of the starting square of the search
    search_predicate_t _predicate, // The callback which determines if a specified square is selected
    void* _context,		    // The context within which to do the comparison
    int* _rtn_row,		    // The row of the found square
    int* _rtn_column,		    // The column of the found square
    bool* _rtn_found);		    // flag indicating whether we found a square within the given max distance

#endif // _NEAREST_NEIGHBOR_GRID_SEARCH_H_
