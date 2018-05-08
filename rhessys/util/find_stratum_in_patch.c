/*--------------------------------------------------------------*/
/*                                                              */ 
/*		find_stratum_in_patch					*/
/*                                                              */
/*  NAME                                                        */
/*		find_stratum_in_patch					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  find_stratum_in_patch( struct basin_object *basin)			*/
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

struct canopy_strata_object *find_stratum_in_patch(
										  int stratum_ID,
										  struct patch_object *patch)
{
	/*------------------------------------------------------*/
	/*	Local Function Definition. 							*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int i;
	int fnd;
	struct canopy_strata_object *stratum;

	/*--------------------------------------------------------------*/
	/*	find stratum						*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	stratum = NULL;
	
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
		/*fprintf(stderr,
			"FATAL ERROR: Could not find stratum %d in  patch %d \n",
			stratum_ID,
			patch[0].ID);*/
		stratum = NULL;
		/*exit(EXIT_FAILURE);*/
	}
	return(stratum);
}/*end find.stratum*/
