/*--------------------------------------------------------------*/
/*                                                              */
/*		find_patch_family										*/
/*                                                              */
/*  NAME                                                        */
/*		find_patch_family										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  find_patch_family( struct basin_object *basin)				*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	alternative to find_patch but for families/MSR				*/
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

struct patch_family_object *find_patch_family(int patch_family_ID, int zone_ID, int hill_ID,
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
	struct patch_family_object *patch_family;
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
	/*	find zones													*/
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
	/*	find patch families											*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	while ( (fnd == 0) && (i >= 0) && (i < zone[0].num_patch_families)) {
		if (zone[0].patch_families[i][0].family_ID == patch_family_ID) {
			patch_family = zone[0].patch_families[i];
			fnd = 1;
		}
		else {
			i += 1;
		}
	}
	if (fnd == 0) {
		fprintf(stderr,
			"FATAL ERROR: Could not find patch family %d in zone %d hill %d\n",
			patch_family_ID, zone_ID, hill_ID);
		exit(EXIT_FAILURE);
	}
	return(patch_family);
} /*end find.patch.c*/
