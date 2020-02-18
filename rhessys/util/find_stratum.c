/*--------------------------------------------------------------*/
/*                                                              */ 
/*		find_stratum					*/
/*                                                              */
/*  NAME                                                        */
/*		find_stratum					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  find_stratum( struct basin_object *basin)			*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	sorts hillslopes, zones and patches within a basin by	*/
/*		elevation					*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

struct canopy_strata_object *find_stratum(
										  int stratum_ID,
										  int patch_ID,
										  int zone_ID,
										  int hill_ID,
										  int basin_ID,
										  struct world_object *world)
{
	/*------------------------------------------------------*/
	/*	Local Function Definition. 							*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int i;
	int fnd;
	struct zone_object *zone;
	struct hillslope_object *hillslope;
	struct patch_object *patch;
	struct canopy_strata_object *stratum;
	struct basin_object *basin;
	/*--------------------------------------------------------------*/
	/*	find basin						*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	while ( (fnd == 0) && (i >= 0) && (i < world[0].num_basin_files)) {
		if (world[0].basins[i][0].ID == basin_ID) {
			basin = world[0].basins[i];
			fnd = 1;
		}
		else {
			i += 1;
		}
	}
	if (fnd == 0) {
		fprintf(stderr,
			"FATAL ERROR: Could not find basin %d in find_stratum\n",basin_ID);
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/*	find hillslopes												*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	while ( (fnd == 0) && (i >= 0) && (i < basin[0].num_hillslopes)) {
		if (basin[0].hillslopes[i][0].ID == hill_ID) {
			hillslope = basin[0].hillslopes[i];
			fnd = 1;
		}
		else {
			i += 1;
		}
	}
	if (fnd == 0) {
		fprintf(stderr,
			"FATAL ERROR: Could not find hillslope %d in find_stratum\n",
			hill_ID);
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/*	find zones						*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	while ( (fnd == 0) && (i >= 0) && (i < hillslope[0].num_zones)) {
		if (hillslope[0].zones[i][0].ID == zone_ID) {
			zone = hillslope[0].zones[i];
			fnd = 1;
		}
		else {
			i += 1;
		}
	}
	// fprintf("Zone ID: %d\n",zone_ID); // ejh remove this
	if (fnd == 0) {
		fprintf(stderr,
			"FATAL ERROR: Could not find zone %d in find_stratum, stratum = %d \n",zone_ID, stratum_ID);
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/*	find patches						*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	while ( (fnd == 0) && (i >= 0) && (i < zone[0].num_patches)) {
		if (zone[0].patches[i][0].ID == patch_ID) {
			patch = zone[0].patches[i];
			fnd = 1;
		}
		else {
			i += 1;
		}
	}
	if (fnd == 0) {
		fprintf(stderr,
			"FATAL ERROR: Could not find patch %d in zone %d hill %d\n",
			patch_ID,
			zone_ID,
			hill_ID);
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/*	find stratum						*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	while ( (fnd == 0) && (i >= 0) && (i < patch[0].num_canopy_strata)) {
		if (patch[0].canopy_strata[i][0].ID == stratum_ID) {
			stratum = patch[0].canopy_strata[i];
			fnd = 1;
		}
		else {
			i += 1;
		}
	}
	if (fnd == 0) {
		fprintf(stderr,
			"FATAL ERROR: Could not find stratum %d in  patch %d in zone %d hill %d\n",
			stratum_ID,
			patch_ID,
			zone_ID,
			hill_ID);
		exit(EXIT_FAILURE);
	}
	return(stratum);
}/*end find.stratum*/
