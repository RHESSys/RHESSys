/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include <stdio.h>

#include "util.h"

/// @brief function for computing an index from a row and column
bool row_col_to_index(
    int _row,
    int _col,
    int _maxr,
    int _maxc,
    int* _rtn_index)
{
    bool result = true;
    if(_rtn_index != 0) {
        if(_row < 0 || _row > _maxr || _col < 0 || _col > _maxc) {
            fprintf(stderr, "ERROR: Row: %d and/or Column: %d out of range: %d,%d.\n",
                    _row, _col, _maxr, _maxc);
            result = false;
        } else {
            *_rtn_index = _row * _maxc + _col;
        }
    } else {
        fprintf(stderr, "ERROR: Index pointer is NULL.\n");
        result = false;
    }
    return result;
}

/// @brief Function for deriving row and column from an index
bool index_to_row_col(
    int _index,
    int _maxr,
    int _maxc,
    int* _rtn_row,
    int* _rtn_col)
{
    bool result = true;
    if(_rtn_row != 0 && _rtn_col != 0) {
        if(_index < 0 || _index > _maxr * _maxc) {
            fprintf(stderr, "ERROR: Index out of range.\n");
            result = false;
        } else {
            *_rtn_row = _index / _maxc;
            *_rtn_col = _index % _maxc;
        }
    } else {
        fprintf(stderr, "ERROR: Row and/or column pointers are NULL.\n");
        result = false;
    }
    return result;
}

/// @brief Returns true if roof value indicates the presence of a roof
bool is_roof(
    double _roof_value)
{
    return _roof_value >= 0;
}

