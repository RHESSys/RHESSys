/*--------------------------------------------------------------*/
/*                                                              */
/*		find_patch					*/
/*                                                              */
/*  NAME                                                        */
/*		find_patch					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  find_patch( struct basin_object *basin)			*/
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

struct patch_object *find_patch( int patch_ID, int zone_ID, int hill_ID,
								struct basin_object *basin)
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
	if (fnd == 0){
		fprintf(stderr,
			"FATAL ERROR: Could not find hillslope %d in find_patch\n",hill_ID);
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
	if (fnd == 0) {
		fprintf(stderr,
			"FATAL ERROR: Could not find zone %d in find_patch\n",zone_ID);
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
			patch_ID, zone_ID, hill_ID);
		exit(EXIT_FAILURE);
	}
	return(patch);
}/*end find.patch.c*/
