#ifndef _UTIL_H_
#define _UTIL_H_

// Why do they not have a bool?!
typedef short bool;
static const short true = 1;
static const short false = 0;

extern bool row_col_to_index(
    int _row,			// The row
    int _col,			// The column
    int _maxr,			// The max row
    int _maxc,			// The max column
    int* _rtn_index);		// Pointer to the index that will be returned
extern bool index_to_row_col(
    int _index,			// The index
    int _maxr,			// The max row
    int _maxc,			// The max column
    int* _rtn_row,		// Pointer to the row that will be returned
    int* _rtn_col);		// Pointer to the column that will be returned

/// @brief Returns true if roof value indicates the presence of a roof
extern bool is_roof(
    double _roof_value);	// The value from the roof array

#endif // _UTIL_H_

