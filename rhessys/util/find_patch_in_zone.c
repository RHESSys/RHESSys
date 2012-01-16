/*--------------------------------------------------------------*/
/*                                                              */ 
/*		find_patch_in_zone					*/
/*                                                              */
/*  NAME                                                        */
/*		find_patch_in_zone					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  find_patch_in_zone( struct basin_object *basin)			*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
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

struct patch_object *find_patch_in_zone(
										  int patch_ID,
										  struct zone_object *zone)
{
	/*------------------------------------------------------*/
	/*	Local Function Definition. 							*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int i;
	int fnd;
	struct patch_object *patch;

	/*--------------------------------------------------------------*/
	/*	find stratum						*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	patch = NULL;
	
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
		/*fprintf(stderr,
			"FATAL ERROR: Could not find patch %d in  zone %d \n",
			patch_ID,
			zone[0].ID);*/
		patch = NULL;
		/*exit(EXIT_FAILURE);*/
	}
	return(patch);
}/*end find_patch_in_zone */
