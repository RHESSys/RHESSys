/*--------------------------------------------------------------*/
/*                                                              */
/*		build_flow_table									    */
/*                                                              */
/*  NAME                                                        */
/*		 build_flow_table										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 build_flow_table( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*  revision:  6.0 29 April, 2005                               */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "fileio.h"

int build_flow_table(f1, flow_table, vegid, lai, hill, zone, patch, maxr, maxc )
	struct flow_struct *flow_table;
	int *patch;
	int *zone;
	int *hill;
	int *vegid;
	float *lai;
	int maxr, maxc;
	FILE *f1;

    {

 	/* local variable declarations */
	int num_patches;
	int inx;
	int r,c,pch;

 	/* local function definitions */
	void	zero_flow_table();
	int		find_patch();


	num_patches = 0;

	printf("\n Initializing flowtable");
	zero_flow_table(flow_table, maxr, maxc);


	printf("\n searching %d rows and %d cols", maxr, maxc);
	for (r=0; r< maxr; r++) 
		{
		for (c=0; c< maxc; c++)
			{
			inx = r*maxc+c;
			if (patch[inx] == -99999) {
				printf("error in patch file use of -99999 as a patch label not allowed \n");
				exit(1);
				}


			/* ignore areas outside the basin */
			if (( patch[inx] > 0) && (zone[inx] > 0) && (hill[inx] > 0)) {
				pch = find_patch(num_patches, flow_table, patch[inx], zone[inx], hill[inx]);
				if (pch == 0) {
					num_patches += 1;
					pch = num_patches;
					}
				flow_table[pch].patchID = patch[inx];
				flow_table[pch].hillID = hill[inx];
				flow_table[pch].zoneID = zone[inx];
				flow_table[pch].area += 1;
				flow_table[pch].vegid = vegid[inx];
				flow_table[pch].lai += ( float ) (1.0 * lai[inx]);
				if (flow_table[pch].lai < ZERO)
					flow_table[pch].lai = 0.0;

				} /* end if */
			}		
			
		}

	printf("\n Total number of patches is %d", num_patches);


	/* compute mean pch values */
	for (pch = 1; pch <= num_patches; pch++) 
		{
		if (flow_table[pch].area > 0.0)
			flow_table[pch].lai = flow_table[pch].lai / flow_table[pch].area;
		else {
			printf("\n patch %d has zero area and %f lai", flow_table[pch].patchID, flow_table[pch].lai);
			flow_table[pch].lai = flow_table[pch].lai;
			}
		}


	return(num_patches);


    }



