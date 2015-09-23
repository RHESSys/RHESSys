/*--------------------------------------------------------------*/
/*                                                              */ 
/*		sort_by_elevation									*/
/*                                                              */
/*  NAME                                                        */
/*		sort_elevation									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  sort_elevation( struct basin_object *basin)					*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	sorts hillslopes, zones and patches within a basin by 		*/
/*		elevation												*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include "rhessys.h"

void sort_by_elevation( struct basin_object *basin)
{
	/*--------------------------------------------------------------*/
	/*  Local function declaration                                  */
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int i, j;
	int h, z;
	struct zone_object *zone;
	struct zone_object *temp_zone;
	struct hillslope_object *hillslope;
	struct hillslope_object *temp_hill;
	struct patch_object *temp_patch;
	/*--------------------------------------------------------------*/
	/*	sort hillslopes												*/
	/*--------------------------------------------------------------*/
	for (i= basin[0].num_hillslopes-2; i >= 0; i--) {
		for (j=0; j <= i; j++) {
			if (basin[0].hillslopes[j][0].z < basin[0].hillslopes[j+1][0].z) {
				temp_hill = basin[0].hillslopes[j];
				basin[0].hillslopes[j] = basin[0].hillslopes[j+1];
				basin[0].hillslopes[j+1] = temp_hill;
			}
		}
	}
	/*--------------------------------------------------------------*/
	/*	sort zones													*/
	/*--------------------------------------------------------------*/
	for (h=0; h < basin[0].num_hillslopes; h++) {
		hillslope = basin[0].hillslopes[h];
		for (i= hillslope[0].num_zones-2; i >= 0; i--) {
			for (j=0; j <= i; j++) {
				if (hillslope[0].zones[j][0].z < hillslope[0].zones[j+1][0].z) {
					temp_zone = hillslope[0].zones[j];
					hillslope[0].zones[j] = hillslope[0].zones[j+1];
					hillslope[0].zones[j+1] = temp_zone;
				}
			}
		}
		/*--------------------------------------------------------------*/
		/*	sort patches												*/
		/*--------------------------------------------------------------*/
		for (z=0; z < hillslope[0].num_zones; z++) {
			zone = hillslope[0].zones[z];
			for (i= zone[0].num_patches-2; i >= 0; i--) {
				for (j=0; j <= i; j++) {
					if (zone[0].patches[j][0].z < zone[0].patches[j+1][0].z) {
						temp_patch = zone[0].patches[j];
						zone[0].patches[j] = zone[0].patches[j+1];
						zone[0].patches[j+1] = temp_patch;
					}
				}
			}
		} /* end zones */
	} /* end hillslopes */
	return;
} /* end sort_by_elevation.c */
