#include <stdio.h>
#include <limits.h>
#include <math.h>

#include "util.h"
#include "nearest_neighbor_grid_search.h"

static double grid_dist(int _row1, int _row2, int _col1, int _col2) {
	int squaredDist = 0;
	/* We now need the actual distance because we are multiplying distances by weights */
	squaredDist = (_row1 - _row2) * (_row1 - _row2)
			+ (_col1 - _col2) * (_col1 - _col2);
	return sqrt( (double)squaredDist );
}

bool grid_search(int _max_dist, int _start_row, int _start_col, int _max_row, int _max_col,
		search_predicate_t _predicate, search_predicate_tiebreaker_t _tiebreaker, void* _context, int* _rtn_row,
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
		double min_dist = (double)INT_MAX;
		for (int offset = 1; keepSearching && offset < _max_dist; ++offset) {
			int min_row = _start_row - offset;
			if (min_row < 0) min_row = 0;
			int min_col = _start_col - offset;
			if (min_col < 0) min_col = 0;
			int max_row = _start_row + offset;
			if (max_row > _max_row) max_row = _max_row;
			int max_col = _start_col + offset;
			if (max_col > _max_col) max_col = _max_col;

			for (int col = min_col; result && col <= max_col; ++col) {
				int vote = 0;
				if (!_predicate(_start_row, _start_col, min_row, col, _context, &vote)) {
					keepSearching = false;
				} else {
					if (vote) {
						double dist = vote * grid_dist(_start_row, min_row,
								_start_col, col);

//						printf("\n\tDistance: %f (min dist: %f)\n", dist, min_dist);

						if (dist < min_dist) {
							found_row = min_row;
							found_col = col;
							min_dist = dist;
							found = true;
						} else if (dist == min_dist && found) {
							// Determine tiebreaker
							if ( !_tiebreaker(found_row, found_col, min_row, col, _context, &found_row, &found_col) ) {
								fprintf(stderr, "ERROR: Error determining tiebreaker.\n");
								result = false;
								keepSearching = false;
							}
						}
					}
					vote = 0;
					if (!_predicate(_start_row, _start_col, max_row, col, _context, &vote)) {
						keepSearching = false;
					} else {
						if (vote) {
							double dist = vote * grid_dist(_start_row, max_row,
									_start_col, col);

//							printf("\n\tDistance: %f (min dist: %f)\n", dist, min_dist);

							if (dist < min_dist) {
								found_row = max_row;
								found_col = col;
								min_dist = dist;
								found = true;
							} else if (dist == min_dist && found) {
								// Determine tiebreaker
								if ( !_tiebreaker(found_row, found_col, max_row, col, _context, &found_row, &found_col) ) {
									fprintf(stderr, "ERROR: Error determining tiebreaker.\n");
									result = false;
									keepSearching = false;
								}
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
						double dist = vote * grid_dist(_start_row, row, _start_col,
								min_col);

//						printf("\n\tDistance: %f (min dist: %f)\n", dist, min_dist);

						if (dist < min_dist) {
							found_row = row;
							found_col = min_col;
							min_dist = dist;
							found = true;
						} else if (dist == min_dist && found) {
							// Determine tiebreaker
							if ( !_tiebreaker(found_row, found_col, row, min_col, _context, &found_row, &found_col) ) {
								fprintf(stderr, "ERROR: Error determining tiebreaker.\n");
								result = false;
								keepSearching = false;
							}
						}
					}
					vote = 0;
					if (!_predicate(_start_row, _start_col, row, max_col, _context, &vote)) {
						keepSearching = false;
					} else {
						if (vote) {
							double dist = vote * grid_dist(_start_row, row, _start_col,
									max_col);

//							printf("\n\tDistance: %f (min dist: %f)\n", dist, min_dist);

							if (dist < min_dist) {
								found_row = row;
								found_col = max_col;
								min_dist = dist;
								found = true;
							} else if (dist == min_dist && found) {
								// Determine tiebreaker
								if ( !_tiebreaker(found_row, found_col, row, max_col, _context, &found_row, &found_col) ) {
									fprintf(stderr, "ERROR: Error determining tiebreaker.\n");
									result = false;
									keepSearching = false;
								}
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
