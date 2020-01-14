#ifndef _IMPERVIOS_SEARCH_PREDICATE_H_
#define _IMPERVIOS_SEARCH_PREDICATE_H_

#include "util.h"

/// @brief Creates a context that the impervious search predicate can use for its comparisons.
extern bool impervious_make_context(
    int _maxr,			// The maximum row
    int _maxc,			// The maximum column
    const double* _roofs,       // The array of roof square contribution proportions to impervious surfaces
    const int* _impervious,     // The array indicating whether a square is impervious
    void** _rtn_ctx);		// The pointer to the returned context

/// @brief Callback for determining if a grid square satisfies our search for an impervious surface
bool impervious_search_predicate(
	int _subjectRow,		// Row from which search begins
	int _subjectCol, 	// Column from which search begins
    int _row,			// The squares row
    int _col,			// The squares column
    void* _context,		// The previously created impervious search context
    int* _rtn_vote);		// The returned vote for this square. For this predicate it will be 0 or 1.

/// @brief Callback for breaking ties when two grid squares equally satisfy our search for an impervious surface
bool impervious_search_tiebreaker(
	int _rowA,
	int _colA,
	int _rowB,
	int _colB,
	void* _context,
	int* _winnerRow,
	int* _winnerCol);

#endif // _IMPERVIOS_SEARCH_PREDICATE_H_
