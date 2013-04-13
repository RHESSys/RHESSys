/** @file check_neighbours.h
 *  @brief Examines the neighbourhood of the patch figures out
 *  if it is at a border at any border, length of perimeter is added
 */
#ifndef CHECK_NEIGHBOURS_H
#define CHECK_NEIGHBOURS_H

#include "blender.h"

/** @brief Examine the neighbourhood of the patch figures out
 *  if it is at a border at any border, length of perimeter is added
 *
 *	@param inputRow Int, row of cell whose neighborhood should be checked
 *	@param inputCol Int, column of cell whose neighborhood should be checked
 *	@param patch Array of type int representing the patch map
 *	@param zone Array of type int representing the zone map
 *	@param hill Array of type int representing the hillslope map
 *	@param stream Array of type int representing the stream map
 *	@param flow_entry flow_struct entry for current patch
 *	@param num_adj Int, number of adjacent patches
 *	@param f1 File handle of output flow table. (is not used)
 *	@param maxr Int, the maximum index of rows in the study area
 *	@param maxc Int, the maximum index of columns in the study area
 *	@param sc_flag Int defined in main.h, indicates stream connectivitiy (is not used)
 *	@param cell Double, raster resolution of DEM
 *
 *	@deprecated
 *		Parameter f1 is not used.
 *		Parameter sc_flag is not used.
 *
 *	@return The number of new adjacent patches added to the current patch's adjacency list.
 */
int check_neighbours(int inputRow, int inputCol, int *patch, int *zone, int *hill,
		int *stream, struct flow_struct *flow_entry, int num_adj, FILE *f1,
		int maxr, int maxc, int sc_flag, double cell);

#endif
