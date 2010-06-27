/*--------------------------------------------------------------*/
/*                                                              */
/*		adjust_pit												*/
/*                                                              */
/*  NAME                                                        */
/*		 adjust_pit												*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 adjust_pit( 								        	*/
/*                                                              */
/*  OPTIONS                                                     */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*																*/
/*                                                              */
/*																*/
/*	have pit point to patch pointed to by the lowest edge 	    */
/*  revision 6.0:  29 April, 2005                               */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
 

 double	adjust_pit( 
					struct flow_struct *flow_table,
					int curr,
					int edge_inx,
					double edge_elev,
					double cell, int slp_flag)


 {
	int  j;
	double total_perimeter;
	float xrun, yrun;
	float rise;

	struct	adj_struct *aptr;

			aptr = flow_table[curr].adj_list;
			total_perimeter = aptr->perimeter;

			for (j=1; j < flow_table[curr].num_adjacent; j++) {
				aptr = aptr->next;
				total_perimeter += aptr->perimeter;
				}

			if (flow_table[curr].num_adjacent == 0) {
				
			if ((flow_table[curr].adj_list = (struct adj_struct *)malloc(sizeof(struct adj_struct))) == NULL) {
				printf("\n Not enough memory");
				exit(1);
				}
				aptr = flow_table[curr].adj_list;
			}

			else {
			if ((aptr->next = (struct adj_struct *)malloc(sizeof(struct adj_struct))) == NULL) {
				printf("\n Not enough memory");
				exit(1);
				}
			aptr = aptr->next;
			}


			aptr->patchID = flow_table[edge_inx].patchID;
			aptr->zoneID = flow_table[edge_inx].zoneID;
			aptr->hillID = flow_table[edge_inx].hillID;

			aptr->perimeter = total_perimeter;
			aptr->z = ( float )edge_elev;
			aptr->inx = edge_inx;

			xrun =  (flow_table[curr].x - flow_table[edge_inx].x);
			xrun = xrun * xrun;
		   	 yrun =   (flow_table[curr].y - flow_table[edge_inx].y);
			yrun = yrun * yrun;
			

			rise = flow_table[curr].z -flow_table[edge_inx].z;

			
			if ((yrun + xrun) <= 0.000000001)
				aptr->slope = rise;
			else
				aptr->slope = ( float )(rise / (sqrt(xrun+yrun) * (cell) ) );


			aptr->gamma = 1.0;

			flow_table[curr].gamma_neigh = 	( float )(aptr->perimeter *  aptr->slope  );


		if (slp_flag == 0) {
			flow_table[curr].total_gamma = 	( float )(flow_table[curr].area* aptr->slope * cell * cell  );
			}


		if (slp_flag == 1) {
			flow_table[curr].total_gamma = 	( float )(flow_table[curr].area* flow_table[curr].internal_slope * cell * cell  );
			}

		if (slp_flag == 2) {
			flow_table[curr].total_gamma = 	( float )(flow_table[curr].area * flow_table[curr].max_slope * cell * cell  );
			}

			flow_table[curr].z = ( float )( edge_elev+MIN_RISE );

			flow_table[curr].num_adjacent += 1;



	return(1) ;  

	} /* end adjust_pit */

