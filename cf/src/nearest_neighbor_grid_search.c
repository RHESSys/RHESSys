#include <stdio.h>
#include <limits.h>

#include "util.h"
#include "nearest_neighbor_grid_search.h"

static int grid_dist(int _row1, int _row2, int _col1, int _col2) {
	int result = 0;
	/* Use the sqr distance because we just need relative measure of distance */
	result = (_row1 - _row2) * (_row1 - _row2)
			+ (_col1 - _col2) * (_col1 - _col2);
	return result;
}

bool grid_search(int _max_dist, int _start_row, int _start_col,
		search_predicate_t _predicate, void* _context, int* _rtn_row,
		int* _rtn_col, bool* _rtn_found) {
	bool keepSearching = true;
	bool result = true;
	if (_predicate == 0) {
		fprintf(stderr, "ERROR: Predicate function pointer is NULL.\n");
		result = false;
		keepSearching = false;
	} else if (_context == 0) {
		fprintf(stderr, "ERROR: Predicate context pointer is NULL.\n");
		result = false;
		keepSearching = false;
	} else if (_rtn_row == 0) {
		fprintf(stderr, "ERROR: Return row pointer is NULL.\n");
		result = false;
		keepSearching = false;
	} else if (_rtn_col == 0) {
		fprintf(stderr, "ERROR: Return column pointer is NULL.\n");
		result = false;
		keepSearching = false;
	} else if (_rtn_found == 0) {
		fprintf(stderr, "ERROR: Return found pointer is NULL.\n");
		result = false;
		keepSearching = false;
	} else {
		bool found = false;
		int found_row = 0;
		int found_col = 0;
		int min_dist = INT_MAX;
		for (int offset = 1; keepSearching && offset < _max_dist; ++offset) {
			int min_row = _start_row - offset;
			int min_col = _start_col - offset;
			int max_row = _start_row + offset;
			int max_col = _start_col + offset;
			for (int col = min_col; result && col <= max_col; ++col) {
				int vote = 0;
				if (!_predicate(_start_row, _start_col, min_row, col, _context, &vote)) {
					keepSearching = false;
				} else {
					if (vote) {
						int dist = vote * grid_dist(_start_row, min_row, _start_col,
								col);
						if (dist < min_dist) {
							found_row = min_row;
							found_col = col;
							min_dist = dist;
							found = true;
						}
					}
					vote = 0;
					if (!_predicate(_start_row, _start_col, max_row, col, _context, &vote)) {
						keepSearching = false;
					} else {
						if (vote) {
							int dist = vote * grid_dist(_start_row, max_row,
									_start_col, col);
							if (dist < min_dist) {
								found_row = max_row;
								found_col = col;
								min_dist = dist;
								found = true;
							}
						}
					}

				}
			}
			for (int row = min_row + 1; result && row <= max_row - 1; ++row) {
				int vote = 0;
				if (!_predicate(_start_row, _start_col, row, min_col, _context, &vote)) {
					keepSearching = false;
				} else {
					if (vote) {
						int dist = vote * grid_dist(_start_row, row, _start_col,
								min_col);
						if (dist < min_dist) {
							found_row = row;
							found_col = min_col;
							min_dist = dist;
							found = true;
						}
					}
					vote = 0;
					if (!_predicate(_start_row, _start_col, row, max_col, _context, &vote)) {
						keepSearching = false;
					} else {
						if (vote) {
							int dist = vote * grid_dist(_start_row, row, _start_col,
									max_col);
							if (dist < min_dist) {
								found_row = row;
								found_col = max_col;
								min_dist = dist;
								found = true;
							}
						}
					}
				}
			}
		}
		if (found) {
			*_rtn_row = found_row;
			*_rtn_col = found_col;
		}
		*_rtn_found = found;
	}
	return result;
}
