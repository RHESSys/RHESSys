#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "roof_geometry.h"
#include "util.h"

struct roof_square_s {
	int row_;
	int col_;
	struct roof_square_s* next_;
};

/* Typedef in header */
struct roof_geometry_s {
	int min_row_;
	int min_col_;
	int max_row_;
	int max_col_;
	int num_squares_;
	roof_square_t* squares_;
};

/// @brief construct a roof square
static bool construct_roof_square(int _row, int _col,
		roof_square_t** _rtn_square) {
	bool result = true;
	if (_rtn_square == 0) {
		fprintf(stderr, "ERROR: Roof square return pointer is NULL.\n");
		result = false;
	} else {
		roof_square_t* square = (roof_square_t*) calloc(1,
				sizeof(roof_square_t));
		if (square == 0) {
			fprintf(stderr,
					"ERROR: Failed allocating memory for a roof grid square.\n");
			result = false;
		} else {
			square->row_ = _row;
			square->col_ = _col;
			*_rtn_square = square;
		}
	}
	return result;
}

/// @brief constructs a roof geometry data structure
bool construct_roof_geometry(roof_geometry_t** _rtn_geometry) {
	bool result = true;
	if (_rtn_geometry != 0) {
		roof_geometry_t* geometry = (roof_geometry_t*) calloc(1,
				sizeof(roof_geometry_t));
		if (geometry == 0) {
			fprintf(stderr,
					"ERROR: Failed allocating memory for a roof geometry.\n");
			result = false;
		} else {
			geometry->min_row_ = INT_MAX;
			geometry->min_col_ = INT_MAX;
			*_rtn_geometry = geometry;
		}
	} else {
		fprintf(stderr, "ERROR: Roof geometry pointer is NULL.\n");
		result = false;
	}

	return result;
}

/// @brief Frees a roof geometry data structure
bool destroy_roof_geometry(roof_geometry_t** _rtn_geometry) {
	bool result = true;
	if (_rtn_geometry != 0) {
		roof_geometry_t* geometry = *_rtn_geometry;
		if (geometry == 0) {
			fprintf(stderr,
					"ERROR: Attempting to destroy NULL roof geometry.\n");
			result = false;
		} else {
			roof_square_t* square = geometry->squares_;
			while (result && square != 0) {
				roof_square_t* current_square = square;
				square = square->next_;
				free(current_square);
			}
			if (result) {
				free(geometry);
				*_rtn_geometry = 0;
			}
		}
	} else {
		fprintf(stderr, "ERROR: Roof geometry pointer is NULL.\n");
		result = false;
	}

	return result;
}

/// @brief Adds a roof grid square to the roof geometry
bool add_roof_square(roof_geometry_t* _roof_geometry, int _row, int _col) {
	bool result = true;
	if (_roof_geometry != 0) {
		if (_row > _roof_geometry->max_row_) {
			_roof_geometry->max_row_ = _row;
		}
		if (_row < _roof_geometry->min_row_) {
			_roof_geometry->min_row_ = _row;
		}
		if (_col > _roof_geometry->max_col_) {
			_roof_geometry->max_col_ = _col;
		}
		if (_col < _roof_geometry->min_col_) {
			_roof_geometry->min_col_ = _col;
		}
		roof_square_t* square = 0;
		if (!construct_roof_square(_row, _col, &square)) {
			fprintf(stderr, "ERROR: Failed to construct a new roof square.\n");
			result = false;
		} else {
			square->next_ = _roof_geometry->squares_;
			++(_roof_geometry->num_squares_);
			_roof_geometry->squares_ = square;
		}
	} else {
		fprintf(stderr, "ERROR: Roof geometry pointer is NULL.\n");
		result = false;
	}
	return result;
}

/// @brief Print roof geometry to the specified file
bool print_roof_geometry(FILE* _fid, roof_geometry_t* _roof_geometry) {
	bool result = true;
	if (_fid == 0) {
		fprintf(stderr, "ERROR: File pointer is NULL.\n");
		result = false;
	} else if (_roof_geometry == 0) {
		fprintf(stderr, "ERROR: Roof geometry pointer is NULL.\n");
		result = false;
	} else if (_roof_geometry->num_squares_ == 0) {
		fprintf(stderr, "ERROR: Roof geometry has zero squares.\n");
		result = false;
	} else if (_roof_geometry->squares_ == 0) {
		fprintf(stderr,
				"ERROR: Roof geometry square list is NULL but count not zero.\n");
		result = false;
	} else {
		fprintf(_fid, "\nPrinting roof geometry with %d squares:\n",
				_roof_geometry->num_squares_);
		int count = 0;
		roof_square_t* square = _roof_geometry->squares_;
		while (square) {
			fprintf(_fid, "\tSquare %d:\tRow: %d\tColumn: %d\n", count,
					square->row_, square->col_);
			++count;
			square = square->next_;
		}
		fprintf(_fid, "Done with roof geometry.\n\n");
	}
	return result;
}

bool roof_square_row(roof_square_t* _square, int* _rtn_row) {
	bool result = true;
	if (_square == 0) {
		fprintf(stderr, "ERROR: Roof square pointer is NULL.\n");
		result = false;
	} else if (_rtn_row == 0) {
		fprintf(stderr, "ERROR: Return row pointer is NULL.\n");
		result = false;
	} else {
		*_rtn_row = _square->row_;
	}
	return result;
}

bool roof_square_col(roof_square_t* _square, int* _rtn_col) {
	bool result = true;
	if (_square == 0) {
		fprintf(stderr, "ERROR: Roof square pointer is NULL.\n");
		result = false;
	} else if (_rtn_col == 0) {
		fprintf(stderr, "ERROR: Return col pointer is NULL.\n");
		result = false;
	} else {
		*_rtn_col = _square->col_;
	}
	return result;
}

bool roof_square_next(roof_square_t* _square, roof_square_t** _rtn_next) {
	bool result = true;
	if (_square == 0) {
		fprintf(stderr, "ERROR: Roof square pointer is NULL.\n");
		result = false;
	} else if (_rtn_next == 0) {
		fprintf(stderr, "ERROR: Return next pointer is NULL.\n");
		result = false;
	} else {
		*_rtn_next = _square->next_;
	}
	return result;
}

bool roof_geometry_squares(roof_geometry_t* _geometry,
		roof_square_t** _rtn_squares) {
	bool result = true;
	if (_geometry == 0) {
		fprintf(stderr, "ERROR: Roof geometry pointer is NULL.\n");
		result = false;
	} else if (_rtn_squares == 0) {
		fprintf(stderr, "ERROR: Return next pointer is NULL.\n");
		result = false;
	} else {
		*_rtn_squares = _geometry->squares_;
	}
	return result;
}
