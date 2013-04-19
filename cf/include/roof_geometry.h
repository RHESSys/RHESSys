#ifndef _ROOF_GEOMETRY_H_
#define _ROOF_GEOMETRY_H_

#include <stdio.h>

#include "util.h"

/// @brief Incomplete type for a roof geometry
typedef struct roof_geometry_s roof_geometry_t;

/// @brief Incomplete type for a roof square within a geometry
typedef struct roof_square_s roof_square_t;

/// @brief Function to allocate and construct a roof geometry
extern bool construct_roof_geometry(
    roof_geometry_t** _rtn_geometry); // The returned allocated roof geometry

/// @brief Function to free a the specified roof geometry and all of its associated squares
extern bool destroy_roof_geometry(
    roof_geometry_t** _rtn_geometry); // Pointer to the roof geometry to destroy. It is re-initialized to NULL.

/// @brief Add a roof square to the specified roof geometry at the specified row and column
extern bool add_roof_square(
    roof_geometry_t* _roof_geometry, // The specified roof geometry
    int _row,			     // The row of the square
    int _col);			     // The column of the square

/// @brief Utility function to print the roof geometry to a specified open file pointer
extern bool print_roof_geometry(
    FILE* _fid,			// The file pointer to which to write the geometry
    roof_geometry_t* _roof_geometry); // The geometry to write to the file

/// @brief Returns the row of the specified roof square
extern bool roof_square_row(
    roof_square_t* _square,	// A roof square
    int* _rtn_row);		// The returned row of the square

/// @brief Returns the column of the specified roof square
extern bool roof_square_col(
    roof_square_t* _square,	// A roof square
    int* _rtn_col);		// The returned column of the square

/// @brief Returns the next pointer of the specified roof square
extern bool roof_square_next(
    roof_square_t* _square,	// A roof square
    roof_square_t** _rtn_next);	// The returned next pointer of the square

/// @brief Returns the head of the linked list of roof squares within the specified geometry
extern bool roof_geometry_squares(
    roof_geometry_t* _geometry,	// A roof geometry
    roof_square_t** _rtn_squares); // The head of the linked list of roof squares within the geometry

#endif // _ROOF_GEOMETRY_H_
