#include <stdio.h>

#include "nearest_neighbor_grid_search.h"
#include "impervious_search_predicate.h"
#include "stream_search_predicate.h"
#include "roof_geometry.h"
#include "blender.h"
#include "add_flow_to_table.h"

/// @brief Route the roof to the nearest impervious surfaces.
bool compute_roof_connected_routing(struct flow_struct* _flow_table,
		int _num_patches, PatchTable_t *_patchTable,
		roof_geometry_t* _roof_geometry, const double* _roofs,
		const int* _impervious, const int* _stream, const int* _patch, const int* _hill,
		const int* _zone, int _maxr, int _maxc) {
	bool result = true;

	// check input parameters
	if (_flow_table == 0) {
		fprintf(stderr, "ERROR: Flow table pointer is NULL.\n");
		result = false;
	} else if (_roof_geometry == 0) {
		fprintf(stderr, "ERROR: Roof geometry pointer is NULL.\n");
		result = false;
	} else if (_roofs == 0) {
		fprintf(stderr, "ERROR: Roof values pointer is NULL.\n");
		result = false;
	} else if (_impervious == 0) {
		fprintf(stderr, "ERROR: Impervious surface pointer is NULL.\n");
		result = false;
	} else {



		// make an impervious surface search context
		void* impervious_search_context;
		if (!impervious_make_context(_maxr, _maxc, _roofs, _impervious,
				&impervious_search_context)) {
			fprintf(stderr,
					"ERROR: Failed to make impervious surface search context.\n");
			result = false;
		} else {

			// make a stream search context
			void* stream_search_context;
			if (!stream_make_context(_maxr, _maxc, _stream,
					&stream_search_context)) {
				fprintf(stderr,
						"ERROR: Failed to make stream search context.\n");
				result = false;
			} else {

				// loop over all of the roof squares in the roof geometry
				roof_square_t* roof_square = 0;
				if (!roof_geometry_squares(_roof_geometry, &roof_square)) {
					fprintf(stderr,
							"ERROR: Failed to retrieve the list of squares from the roof geometry.\n");
					result = false;
				} else {
					while (result && roof_square != 0) {
						int found_row = 0;
						int found_col = 0;
						bool found = false;
						int row = 0;
						int col = 0;
						if (!roof_square_row(roof_square, &row)) {
							fprintf(stderr,
									"ERROR: Failed to get the row from the roof square.\n");
							result = false;
						} else if (!roof_square_col(roof_square, &col)) {
							fprintf(stderr,
									"ERROR: Failed to get the column from the roof square.\n");
							result = false;
						} else if (!roof_square_next(roof_square, &roof_square)) {
							fprintf(stderr,
									"ERROR: Failed to get the next pointer from the roof square.\n");
							result = false;
						}
						// search for the nearest impervious surface to the roof square
						else if (!grid_search(NEAREST_NEIGHBOR_GRID_SEARCH_MAX_DIST,
								row, col, _maxr, _maxc, impervious_search_predicate, impervious_search_tiebreaker,
								impervious_search_context, &found_row, &found_col,
								&found)) {
							fprintf(stderr,
									"ERROR: compute_connected: an error occurred while searching for the nearest impervious grid square.\n");
							result = false;

						} else {
							if (found) {
								// Here lies the sciences
								int index;
								if (!row_col_to_index(row, col, _maxr, _maxc,
										&index)) {
									fprintf(stderr,
											"ERROR: Failed to map row: %d, column: %d to an index.\n",
											row, col);
									result = false;
								}
								// The entry in the roofs table is the proportion that goes to impervious surfaces.
								else if (!add_flow_to_table(row, col, found_row,
										found_col, _maxr, _maxc, _flow_table,
										_num_patches, _patchTable, _patch, _hill,
										_zone, _roofs[index], NULL) ) {
									fprintf(stderr,
											"ERROR: Failed to add the roof flow to the flow table.\n");
									result = false;
								}
							} else {
								int index;
								if (!row_col_to_index(row, col, _maxr, _maxc,
										&index)) {
									fprintf(stderr,
											"ERROR: Failed to map row: %d, column: %d to an index.\n",
											row, col);
								} else {
									printf("No impervious surfaces found within %d cells of patch %d, routing to stream\n",
											NEAREST_NEIGHBOR_GRID_SEARCH_MAX_DIST, _patch[index]);
									// Route directly to stream
									int str_row, str_col;
									if (!grid_search(2* NEAREST_NEIGHBOR_GRID_SEARCH_MAX_DIST,
											row, col, _maxr, _maxc, stream_search_predicate, stream_search_tiebreaker,
											stream_search_context, &str_row, &str_col,
											&found)) {
										fprintf(stderr,
												"ERROR: compute_connected: an error occurred while searching for the nearest stream pixel.\n");
										result = false;
									} else {
										if (found) {
											if (!add_flow_to_table(row, col, str_row,
													str_col, _maxr, _maxc, _flow_table,
													_num_patches, _patchTable, _patch, _hill,
													_zone, _roofs[index], NULL) ) {
												fprintf(stderr,
														"ERROR: Failed to add the roof flow to outlet.\n");
												result = false;
											}
										} else {
											// Route to watershed outlet if we can't find a stream pixel nearby
											printf("\tUnable to find stream. Routing to watershed outlet.\n");

											// Watershed outlet is the last patch in the table
											// Get row and column of watershed outlet
											if(!index_to_row_col(_num_patches,
													_maxr,
													_maxc,
													&str_row, &str_col)) {
												fprintf(stderr,
														"ERROR: Failed to map index: %d to a row and column.\n",
														_num_patches);
												result = false;
											}

											if (!add_flow_to_table(row, col, str_row,
													str_col, _maxr, _maxc, _flow_table,
													_num_patches, _patchTable, _patch, _hill,
													_zone, _roofs[index], NULL) ) {
												fprintf(stderr,
														"ERROR: Failed to add the roof flow to outlet.\n");
												result = false;
											}

										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return result;
}

