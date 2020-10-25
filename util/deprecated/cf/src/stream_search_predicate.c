#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "stream_search_predicate.h"

typedef struct stream_context_s {
	int maxr_;
	int maxc_;
	const int* stream_;
} stream_context_t;

bool stream_make_context(
		int _maxr,
		int _maxc,
		const int* _stream,
		void** _rtn_ctx)
{
	bool result = true;
	if(_rtn_ctx == 0) {
		fprintf(stderr, "ERROR: Return stream context pointer is NULL.\n");
		result = false;
	} else {
		stream_context_t* context = (stream_context_t*)calloc(1, sizeof(stream_context_t));
		if(context == 0) {
			fprintf(stderr, "ERROR: Failed to allocate memory for the stream search context.\n");
			result = false;
		} else {
			context->maxr_ = _maxr;
			context->maxc_ = _maxc;
			context->stream_ = _stream;

			*_rtn_ctx = (void*)context;
		}
	}
	return result;
}

bool stream_search_predicate(
		int _subjectRow,
		int _subjectCol,
		int _row,
		int _col,
		void* _context,
		int* _rtn_vote)
{
	bool result = true;
	if(_context == 0) {
		fprintf(stderr, "ERROR: Search context pointer is NULL.\n");
		result = false;
	} else {
		/* kinda ugly but it is C */
		stream_context_t* context = (stream_context_t*)_context;
		int index;
		if(!row_col_to_index(_row, _col, context->maxr_, context->maxc_, &index)) {
			//	    fprintf(stderr, "ERROR: Failed to map the row: %d, and column: %d to an index.\n", _row, _col);
			result = false;
		} else {
			if(context->stream_[index] == 1) {
				*_rtn_vote = 1;
			} else {
				*_rtn_vote = 0;
			}
		}
	}

	return result;
}

bool stream_search_tiebreaker(
	int _rowA,
	int _colA,
	int _rowB,
	int _colB,
	void* _context,
	int* _winnerRow,
	int* _winnerCol) {
	// Default winner is the first
	*_winnerRow = _rowA;
	*_winnerCol = _colA;
	return true;
}

