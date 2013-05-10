/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_drainage_density								*/
/*                                                              */
/*  NAME                                                        */
/*		 compute_drainage_density								*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 compute_drainage_density( 						        */
/*                                                              */
/*  OPTIONS                                                     */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*																*/
/*	computes  drainage_density for basin	                    */
/*                                                              */
/*  revision: 6.0  29 April, 2005                               */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"

int compute_drainage_density(flow_table, num_patches, cell)
	struct flow_struct *flow_table;int num_patches;double cell;

{


	/* local variable declarations */

	int inx;
	int neigh;
	int pch;
	int n_adjacent;
	float run, xrun, yrun;
	float drainage_density, total_area;

	struct adj_struct *aptr;
	struct ID_struct;

	total_area = 0.0;

	/* send area to  each upslope neighbour for each patch in the sort list  */

	drainage_density = 0.0;
	n_adjacent = 0;

	for (pch = 1; pch <= num_patches; pch++) {
		aptr = flow_table[pch].adj_list;

		total_area += flow_table[pch].area;

		/* check to see if it is a stream patch */
		/* if is is, add the distance to downstream patch */

		if ((flow_table[pch].land == 1)) {

			n_adjacent = 0;
			for (neigh = 1; neigh <= flow_table[pch].num_adjacent; neigh++) {

				inx = aptr->inx;

				if (aptr->gamma > 0.0) {
					xrun = (float) (pow((flow_table[pch].x - flow_table[inx].x),
							2.0));
					yrun = (float) (pow((flow_table[pch].y - flow_table[inx].y),
							2.0));

					run = (float) (sqrt(xrun + yrun) * (cell));

					drainage_density += run;
					if (flow_table[inx].land != 1)
						printf("\nDownstream patch from %d is not a stream %d",
								flow_table[pch].patchID,
								flow_table[inx].patchID);
					n_adjacent += 1;
				}

				aptr = aptr->next;
			}
			if (n_adjacent == 0) {
				printf("\n STREAM %d to outlet", flow_table[pch].patchID);
				drainage_density +=
						(float) (sqrt(flow_table[pch].area) * (cell));
			}
		}

	}

	drainage_density = (float) (drainage_density / (cell * total_area));
	printf("\n Drainage Density is %f\n", drainage_density);

	return (1);

}

