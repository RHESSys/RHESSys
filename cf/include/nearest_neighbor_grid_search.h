#ifndef _NEAREST_NEIGHBOR_GRID_SEARCH_H_
#define _NEAREST_NEIGHBOR_GRID_SEARCH_H_

#include "util.h"

#define NEAREST_NEIGHBOR_GRID_SEARCH_MAX_DIST 20

/// @brief Interface for the search comparison callback function.
typedef bool (*search_predicate_t)(
	int,			// The row of the cell where the search begins
	int, 			// The column of the cell where the search begins
    int,			// The row of the cell being compared to the start cell
    int,			// The column of the cell being compared to the start cell
    void*,			// Pointer to the predicate context
    int*);			// Returned vote value

typedef bool (*search_predicate_tiebreaker_t)(
	int,			// Row of first cell
	int, 			// Col of first cell
    int,			// Row of second cell
    int,			// Col of second cell
    void*,			// Pointer to the predicate context
    int*,		    // Pointer to row of winner of tie breaker
    int*);			// Pointer to col of winner of tie breaker

/// @brief Do a nearest neighbor search over the grid squares using a callback to determine when a square is selected.
/// @note Search is limited to cardinal directions
bool grid_search(
    int _max_dist,		// The max distance out from which the search will look before giving up.
    int _start_row,		// The row of the starting square of the search
    int _start_col,		// The column of the starting square of the search
	int _max_row,		// Maximum row
	int _max_col,		// Maximum column
    search_predicate_t _predicate, // The callback which determines if a specified square is selected
    search_predicate_tiebreaker_t _tiebreaker,
    void* _context,		    // The context within which to do the comparison
    int* _rtn_row,		    // The row of the found square
    int* _rtn_column,		    // The column of the found square
    bool* _rtn_found);		    // flag indicating whether we found a square within the given max distance

#endif // _NEAREST_NEIGHBOR_GRID_SEARCH_H_
