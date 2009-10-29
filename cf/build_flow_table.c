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

int build_flow_table(flow_table, dem, slope, hill, zone, patch, stream, roads, sewers, K, m_par, flna, f1,
			maxr, maxc, f_flag, sc_flag, sewer_flag, slp_flag, cell, scale_dem)
	struct flow_struct *flow_table;
	double *dem;
	int *patch;
	int *zone;
	int *hill;
	int *stream;
	int *roads;
	int *sewers;
	float *slope;
	float *flna;
	float *K;
	float	*m_par;
	double cell, scale_dem;
	int maxr, maxc;
	int f_flag, sewer_flag, slp_flag, sc_flag;
	FILE *f1;

    {

 	/* local variable declarations */
	int num_patches;
	int inx;
	int r,c,pch;

 	/* local function definitions */
	void	zero_flow_table();
	int		find_patch();
	int		check_neighbours();


	num_patches = 0;

	zero_flow_table(flow_table, maxr, maxc);

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
				flow_table[pch].x += ( float ) (1.0 * r);
				flow_table[pch].y += ( float ) (1.0 * c);
				flow_table[pch].z += ( float ) dem[inx];
				flow_table[pch].K += ( float ) (1.0 * K[inx]);
				if (sewer_flag == 1)
					flow_table[pch].sewer += (int)sewers[inx];
				if ((sc_flag == 1) || (slp_flag > 0)) {
					flow_table[pch].internal_slope += ( float ) (1.0 * slope[inx]* DtoR );
					if (flow_table[pch].max_slope < slope[inx])
						flow_table[pch].max_slope = ( float ) (1.0 * slope[inx]);
						}
				if (roads[inx] >= 1)
					flow_table[pch].land = 2;
				if (stream[inx] >= 1)
					flow_table[pch].land = 1;
	
			
				if (f_flag)
					flow_table[pch].flna += ( float ) (1.0 * flna[inx]);
				else
					flow_table[pch].flna = 0.0;

				flow_table[pch].m_par += m_par[inx];
				flow_table[pch].num_adjacent += check_neighbours(r,c, patch, 
								zone, hill, stream,
								&flow_table[pch],
								flow_table[pch].num_adjacent,
								f1, maxr, maxc, sc_flag, cell);
				} /* end if */
			}		
			
		}

	printf("\n Total number of patches is %d", num_patches);

	return(num_patches);


    }



