/** @file build_flow_table.h
 * 	@brief Build the overall structure of a flow table
 */
#ifndef BUILD_FLOW_TABLE_H
#define BUILD_FLOW_TABLE_H

#include "blender.h"
#include "util.h"
#include "patch_hash_table.h"

/** @brief Build the overall structure of a flow table
 *
 *  @param flow_table Pointer to memory allocated to store an array of struct flow_table
 *  @param patchTable Pointer to PatchTable_t used for mapping between fully qualified patch IDs
 *  		and flow table indices.
 *	@param dem Array of type double, the DEM
 *	@param slope Array of type float, slope at each cell in the domain
 *	@param hill Array of type int, the hillslope map
 *	@param zone Array of type int, the zone map
 *	@param patch Array of type int, the patch map
 *	@param stream Array of type int, the stream map
 *	@param roads Array of type int, the road map
 *	@param sewers Array of type int, the sewer map
 *  @param roofs Array of type double, the roofs map
 *	@param flna Array of type double, the map of natural log (ln) of a
 *	@param f1 File handle of output flow table. (check_nieghbours does not appear to use f1)
 *	@param maxr Int, the maximum index of rows in the study area
 *	@param maxc Int, the maximum index of columns in the study area
 *	@param f_flag Int, boolean value determining whether flna should be stored for each patch
 *	@param sc_flag Int defined in main.h. Used in conjunction with slp_flag to update internal patch slope
 *	@param sewer_flag Int, boolean indicating whether sewer map is present
 *	@param slp_flag Int defined in main.h. Determines method that should be used for calculating the slope of a patch
 *	@param cell Double, raster resolution of DEM
 *	@param scale_dem Double, DEM scaling factor (is not used)
 *      @param surface boolean indicating we are processing a surface flow table
 *
 *	@deprecated
 *		Parameter flna, flna mode will be removed in a future version (?)
 *		Parameter f1 is not used
 *		Parameter f_flag (associated with flna mode)
 *		Parameter scale_dem is not used
 *
 *	@return The number of patches in the flow table
 */
int build_flow_table(struct flow_struct* flow_table, PatchTable_t *patchTable, double* dem, float* slope,
		     int* hill, int* zone, int* patch, int* stream, int* roads, int* sewers, double* roofs,
		     double* flna, FILE* f1, int maxr, int maxc, int f_flag, int sc_flag,
		     int sewer_flag, int slp_flag, double cell, double scale_dem, bool surface);

#endif
