/*--------------------------------------------------------------*/
/*                                                              */
/*		remove_pits									            */
/*                                                              */
/*  NAME                                                        */
/*		 remove_pits										    */
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 remove_pits( 								            */
/*                                                              */
/*  OPTIONS                                                     */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*																*/
/*                                                              */
/*	remove pits from flow_table by 								*/
/*	having all pixels in the pit point to 						*/
/*   the minimum elevation patch that points to 				*/
/*	 somewhere outside of the pit								*/
/*	a pixel is in the pit if it points to a pixel that 			*/ 
/* 	eventually has no outgoing neighbours AND is below			*/
/*  minimum elevation as described above						*/
/*																*/
/*  revision:  6.0  29 April, 2005                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
 

void  remove_pits(flow_table, num_patches, sc_flag, slp_flag, cell, f1)
	struct flow_struct *flow_table;
	int num_patches;
    int sc_flag;
    int slp_flag;
	double cell;
	FILE *f1;

    {

 	/* local fuction declarations */
	struct ID_struct  sort_flow_table();
	int	find_patch();

 	double	find_top( struct flow_struct *, int, double, int *, int *, int *);

	void adjust_pit();


 	/* local variable declarations */
	int pch;
	int num_pit, num_in_pit;
	int	edge_inx;

	int *upslope_list;

	double edge_elev, top_elev;

	/* allocate list */
	upslope_list = (int *)malloc(num_patches * sizeof(int));
	num_pit = 0;
	top_elev=0.0;

	for (pch = 1; pch <= num_patches; pch++) {
	
	
		/* check to see if it is a pit */
		if ( (flow_table[pch].gamma_neigh == 0) &&
				 ((flow_table[pch].land != 1) )  ) {

			
			fprintf(f1,"\n %d ", flow_table[pch].patchID);

			num_pit += 1;
			num_in_pit = 1;
			upslope_list[1] = pch;
			edge_inx = 0;
			top_elev = 0.0;
			
			top_elev = find_top(flow_table, pch, flow_table[pch].z,
						&num_in_pit, &upslope_list, &edge_inx);
		
			if (top_elev > 0.0) {
				edge_elev = flow_table[edge_inx].z;
				adjust_pit(flow_table, pch, edge_inx, edge_elev, cell, slp_flag); 
				}
			else {
                		printf("\n Cannot resolve pit for %d", flow_table[pch].patchID);
				printf(" for now will route it to outlet");
				adjust_pit(flow_table, pch, num_patches, flow_table[pch].z-0.5, cell, slp_flag);
				}
		}

  
	} /* end patch for */



	printf("\n Number of pits %d", num_pit);
	fprintf(f1,"\n Number of pits  %d", num_pit);
	/*fclose(f1);*/

	/*free(upslope_list); */




	return;
    }

