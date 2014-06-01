#ifndef _PERVIOS_SEARCH_PREDICATE_H_
#define _PERVIOS_SEARCH_PREDICATE_H_

#include "util.h"

/// @brief Creates a context that the pervious search predicate can use for its comparisons.
extern bool pervious_make_context(
    int _maxr,			// The maximum row
    int _maxc,			// The maximum column
    const double* _roofs,       // The array of roof square contribution proportions to impervious surfaces
    const int* _impervious,     // The array indicating whether a square is impervious
    const int* _priorityCells,	// Array indicating cells to prioritize to receive flow from roofs
    const double* _elevation, // Array of elevation values
    const int* _patch,		 // Patch map array
    int _priorityWeight,
    void** _rtn_ctx);		// The pointer to the returned context

/// @brief Callback for determining if a grid square satisfies our search for an pervious surface
bool pervious_search_predicate(
	int _subjectRow,		// Row from which search begins
	int _subjectCol, 	// Column from which search begins
	int _row,			// The squares row
    int _col,			// The squares column
    void* _context,		// The previously created impervious search context
    int* _rtn_vote);		// The returned vote for this square. For this predicate it will be 0 or 1.

/// @brief Callback for breaking ties when two grid squares equally satisfy our search for an pervious surface
bool pervious_search_tiebreaker(
	int _rowA,
	int _colA,
	int _rowB,
	int _colB,
	void* _context,
	int* _winnerRow,
	int* _winnerCol);

#endif // _PERVIOS_SEARCH_PREDICATE_H_
