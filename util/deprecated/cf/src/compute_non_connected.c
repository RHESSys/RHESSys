/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include <stdio.h>

#include "nearest_neighbor_grid_search.h"
#include "compute_non_connected.h"
#include "pervious_search_predicate.h"
#include "roof_geometry.h"
#include "add_flow_to_table.h"
#include "blender.h"

bool compute_roof_non_connected_routing(struct flow_struct* _flow_table,
		int _num_patches, PatchTable_t *_patchTable,
		roof_geometry_t* _roof_geometry, const double* _roofs,
		const int* _impervious, const int* _priority,
		const double* _elevation,
		int priorityWeight,
		const int* _patch, const int* _hill,
		const int* _zone, int _maxr, int _maxc,
		int* const _receiver_out) {
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

		// make an pervious surface search context
		void* pervious_search_context;
		if (!pervious_make_context(_maxr, _maxc, _roofs, _impervious, _priority, _elevation, _patch,
				priorityWeight,
				&pervious_search_context)) {
			fprintf(stderr,
					"ERROR: Failed to make impervious surface search context.\n");
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
					// search for the nearest pervious surface to the roof square
					else if (!grid_search(NEAREST_NEIGHBOR_GRID_SEARCH_MAX_DIST, row, col, _maxr, _maxc,
							pervious_search_predicate, pervious_search_tiebreaker, pervious_search_context,
							&found_row, &found_col, &found)) {
						fprintf(stderr,
								"ERROR: compute_non_connected: an error occurred while searching for the nearest impervious grid square.\n");
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
							// The entry in the roofs table is the proportion that goes to impervious surfaces. Since this is the
							// pervious surfaces we need 1 - the roofs table value
							else if (!add_flow_to_table(row, col, found_row,
									found_col, _maxr, _maxc, _flow_table,
									_num_patches, _patchTable, _patch, _hill,
									_zone, 1.0 - _roofs[index], _receiver_out)) {
								fprintf(stderr,
										"ERROR: Failed to add the roof flow for patch %d, hill %d, zone %d to the flow table.\n",
										_patch[index],
										_hill[index],
										_zone[index]);
								result = false;
							}
						} else {
							// Only display a warning that no pervious surfaces where found if
							//   roof connectivity is less than 1.0, that is if some flow needed to go to
							//   pervious, but could not due to lack of nearby pervious
							int index;
							if (!row_col_to_index(row, col, _maxr, _maxc,
									&index)) {
								fprintf(stderr,
										"ERROR: Failed to map row: %d, column: %d to an index.\n",
										row, col);
								result = false;
							}
							if ( _roofs[index] < 1.0 ) {
								fprintf(stderr,
										"WARNING: No pervious surface found for patch: patchID: %d, hillID: %d, zoneID: %d\n",
										_patch[index], _hill[index], _zone[index]);
							}
						}
					}
				}
			}
		}
	}

	return result;
}

