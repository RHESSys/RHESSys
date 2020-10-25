#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "pervious_search_predicate.h"

typedef struct pervious_context_s {
    int maxr_;
    int maxc_;
    const double* roofs_;
    const int* impervious_;
    const int* priorityCells_;
    const double* elevation_;
    const int* patch_;
    int priorityWeight_;
} pervious_context_t;

bool pervious_make_context(
    int _maxr,
    int _maxc,
    const double* _roofs,
    const int* _impervious,
    const int* _priorityCells,
    const double* _elevation,
    const int* _patch,
    int _priorityWeight,
    void** _rtn_ctx)
{
    bool result = true;
    if(_rtn_ctx == 0) {
	fprintf(stderr, "ERROR: Return impervious context pointer is NULL.\n");
	result = false;
    } else {
	pervious_context_t* context = (pervious_context_t*)calloc(1, sizeof(pervious_context_t));
	if(context == 0) {
	    fprintf(stderr, "ERROR: Failed to allocate memory for the impervious search context.\n");
	    result = false;
	} else {
	    context->maxr_ = _maxr;
	    context->maxc_ = _maxc;
	    context->roofs_ = _roofs;
	    context->impervious_ = _impervious;
	    context->priorityCells_ = _priorityCells;
	    context->elevation_ = _elevation;
	    context->patch_ = _patch;
	    context->priorityWeight_ = _priorityWeight;

	    *_rtn_ctx = (void*)context;
	}
    }    
    return result;
}

bool pervious_search_predicate(
	int _subjectRow,
	int _subjectCol,
    int _row,
    int _col,
    void* _context,
    int* _rtn_vote)
{
    bool result = true;
    if (_context == 0) {
    	fprintf(stderr, "ERROR: Search context pointer is NULL.\n");
    	result = false;
    } else {
    	/* kinda ugly but it is C */
    	pervious_context_t* context = (pervious_context_t*)_context;
    	int subjectIndex;
    	int index;
    	if (!row_col_to_index(_subjectRow, _subjectCol, context->maxr_, context->maxc_, &subjectIndex) ||
    			!row_col_to_index(_row, _col, context->maxr_, context->maxc_, &index)) {
//	    	fprintf(stderr, "ERROR: Failed to map the row: %d, and column: %d to an index.\n", _row, _col);
    		result = false;
    	} else {

//    		printf("\nSubject patch: %d, target patch: %d",
//    				context->patch_[subjectIndex], context->patch_[index]);

    		if (context->impervious_[index] == 0 && !is_roof(context->roofs_[index])) {
    			if (context->priorityCells_ != NULL &&
    					context->priorityCells_[index] == 1 && context->elevation_[index] < context->elevation_[subjectIndex]) {
    				// Priority cells that are lower in elevation than the subject cell get a higher vote
    				*_rtn_vote = 1;
    			} else {
    				*_rtn_vote = context->priorityWeight_;
    			}
    		} else {
    			*_rtn_vote = 0;
    		}
    	}
    }
    
//    printf("\tVote: %d", *_rtn_vote);

    return result;
}

bool pervious_search_tiebreaker(
	int _rowA,
	int _colA,
	int _rowB,
	int _colB,
	void* _context,
	int* _winnerRow,
	int* _winnerCol) {

	bool result = true;
	if (_context == 0) {
		fprintf(stderr, "ERROR: Search context pointer is NULL.\n");
		result = false;
	} else {
		pervious_context_t* context = (pervious_context_t*)_context;

		int aIndex;
		int bIndex;
		if (!row_col_to_index(_rowA, _colA, context->maxr_, context->maxc_, &aIndex) ||
				!row_col_to_index(_rowB, _colB, context->maxr_, context->maxc_, &bIndex)) {
//	    	fprintf(stderr, "ERROR: Failed to map the row: %d, and column: %d to an index.\n", _row, _col);
			result = false;
		} else {

//			printf("\nTie breaker patch A: %d, patch B: %d",
//			    	context->patch_[aIndex], context->patch_[bIndex]);

			result = true;
			// Default winner is the first (to allow nearer cells to win)
			*_winnerRow = _rowA;
			*_winnerCol = _colA;
			if (context->priorityCells_ != NULL) {
				if ( context->priorityCells_[aIndex] != 1 &&
						context->priorityCells_[bIndex] == 1 ) {
						*_winnerRow = _rowB;
						*_winnerCol = _colB;

//						printf("\tPatch B wins");
				}
			}
		}
	}

	return result;
}

