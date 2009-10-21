/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_gamma									        */
/*                                                              */
/*  NAME                                                        */
/*		 compute_gamma										    */
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 compute_gamma( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*																*/
/*	computes mean x,y,z for each pch							*/
/*	sorts pches by elevation									*/
/*	computes slope and gamma for neighbours of each pch		    */
/*		- gamma is 0 if slope is negative						*/
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
 

int compute_gamma(flow_table, num_patches, f1,scale_trans, cell, sc_flag,
                  slp_flag)
	struct flow_struct *flow_table;
	int num_patches;
	int sc_flag;
    int slp_flag;
	float scale_trans;
	double cell;
	FILE *f1;

    {

 	/* local fuction declarations */
	struct ID_struct  sort_flow_table();

	int	find_patch();

 	/* local variable declarations */
	int p,z,h;
	int inx;
	int neigh;
	int num_str, pch;

	
	double mult, rise, run;
	double xrun, yrun;


	struct adj_struct *aptr;
	struct adj_struct *str_aptr;
	struct ID_struct max_ID;

	printf("\n Compute_gamma");

	num_str = 0;
	/* compute mean pch values */
	for (pch = 1; pch <= num_patches; pch++) 
		{
		flow_table[pch].x = flow_table[pch].x / flow_table[pch].area;
		flow_table[pch].y = flow_table[pch].y / flow_table[pch].area;
		flow_table[pch].z = flow_table[pch].z / flow_table[pch].area;
		flow_table[pch].K = flow_table[pch].K / flow_table[pch].area;
		if ((sc_flag == 1) || (slp_flag == 1)) {
			flow_table[pch].internal_slope = flow_table[pch].internal_slope
						 / flow_table[pch].area;
			flow_table[pch].internal_slope = ( float )( tan(flow_table[pch].internal_slope) );
			flow_table[pch].max_slope = ( float )( tan(flow_table[pch].max_slope * DtoR) );
		}
		flow_table[pch].m_par = flow_table[pch].m_par / flow_table[pch].area;
		flow_table[pch].flna = flow_table[pch].flna / flow_table[pch].area;
		flow_table[pch].total_gamma = 0.0;
		flow_table[pch].acc_area = 0.0;
		flow_table[pch].total_perimeter = 0.0;
		flow_table[pch].total_str_gamma = 0.0;
		flow_table[pch].num_str = 0;
		flow_table[pch].road_dist = 0.0;
		flow_table[pch].inflow_cnt = 0;
		if (flow_table[pch].land == 1) {
			num_str += 1;
			 /* printf("\nid %d nstr %d", flow_table[pch].patchID, num_str); */
			}
		}

	max_ID = sort_flow_table(flow_table, num_patches);

	/* create a mapping between ID's and partition name ID's 
	printf("\n Max's %d %d %d\n", max_ID.hill, max_ID.zone, max_ID.patch); */

	/* calculate gamma for each neighbour */
	for (pch = 1; pch <= num_patches; pch++) 
		{

		printf("\n nex pch %d", pch);
		printf("\n Processing patch %ld", flow_table[pch].patchID);
		printf("\n Processing patch %ld", flow_table[pch].patchID);
		aptr = flow_table[pch].adj_list;
		str_aptr = flow_table[pch].adj_str_list;

		mult = flow_table[pch].m_par * flow_table[pch].K;

		flow_table[pch].slope = 0.0;

		printf("\n number of neighbours %d", flow_table[pch].num_dsa);
		printf("\n number of neighbours %d", flow_table[pch].num_dsa);
		/* first do processing for stream table */
	/*
		for (neigh = 1; neigh <= flow_table[pch].num_dsa; neigh ++)
			{

			p = str_aptr->patchID;
			z = str_aptr->zoneID;
			h = str_aptr->hillID;

				printf("\n neigh %d is %d", neigh, p);
				printf("\n neigh %d is %d", neigh, p);
			inx = find_patch(num_patches, flow_table, p, z, h);
			if (inx == 0) {
				printf("\n For patch %d %d %d Neighbour not found %d %d %d\n", 
						flow_table[pch].hillID, flow_table[pch].zoneID,
						flow_table[pch].patchID, p, z, h);
				exit(1);
				}

			str_aptr->inx = inx;
			rise = flow_table[pch].z - flow_table[inx].z;
			if (rise > 0.0) {
				str_aptr->gamma = rise;
				}
			else 
				str_aptr->gamma = 0.0;
			flow_table[pch].total_str_gamma += str_aptr->gamma;
			str_aptr = str_aptr->next;

			}

*/
		printf("\n number of  aneighbours %d", flow_table[pch].num_adjacent);
		printf("\n number of a neighbours %d", flow_table[pch].num_adjacent);
		/* now do processing for flow table */
		for (neigh = 1; neigh <= flow_table[pch].num_adjacent; neigh ++)
			{

			p = aptr->patchID;
			z = aptr->zoneID;
			h = aptr->hillID;

			printf("\n neigh %d is %d", neigh, p);
			printf("\n neigh %d is %d", neigh, p);

			inx = find_patch(num_patches, flow_table, p, z, h);

			if (inx == 0) {
				printf("\n For patch %d %d %d Neighbour not found %d %d %d\n", 
						flow_table[pch].hillID, flow_table[pch].zoneID,
						flow_table[pch].patchID, p, z, h);
				exit(1);
				}

			aptr->inx = inx;



			rise = flow_table[pch].z - flow_table[inx].z;
			xrun =  pow( (flow_table[pch].x - flow_table[inx].x), 2.0);
			yrun =  pow( (flow_table[pch].y - flow_table[inx].y), 2.0);

			run =  sqrt(xrun+yrun) * (cell);
			printf(" \nrise %lf run %lf", rise, run);

			if ( run <= 0) {
				printf("\n Slope is zero for ( %d, %d, %d) to (%d, %d, %d)",
					flow_table[pch].hillID,
					flow_table[pch].zoneID,
					flow_table[pch].patchID,
					flow_table[inx].hillID,
					flow_table[inx].zoneID,
					flow_table[inx].patchID);
				run = 0.01;
				}


			aptr->slope = ( float )(rise / run );
			aptr->z = flow_table[inx].z;



			aptr->gamma = ( float )( aptr->perimeter * mult * aptr->slope / (cell) );

			if (aptr->gamma < 0.0) aptr->gamma = 0.0;

			/* do not send flow to outside of the basin */
			/*
			if ( (aptr->patchID == 0) || (aptr->zoneID == 0) || (aptr->hillID == 0) )	
						aptr->gamma = 0.0;

			*/

			flow_table[pch].total_gamma += aptr->gamma;
			flow_table[pch].total_perimeter += aptr->perimeter;

			flow_table[pch].slope += aptr->slope * aptr->gamma;

			aptr = aptr->next;
			}


		/*  divided by total_gamma */
		aptr = flow_table[pch].adj_list;

	
		if (flow_table[pch].total_gamma == 0.0)
			flow_table[pch].slope = 0.0;
		else
			flow_table[pch].slope = flow_table[pch].slope / flow_table[pch].total_gamma;

		for (neigh = 1; neigh <= flow_table[pch].num_adjacent; neigh ++)
			{
			if (flow_table[pch].total_gamma != 0.0) 
				aptr->gamma  = aptr->gamma / flow_table[pch].total_gamma;
			else 
				aptr->gamma = 0.0;
			aptr = aptr->next;

			}


		if (slp_flag == 1) {
			flow_table[pch].total_gamma = mult * flow_table[pch].internal_slope * flow_table[pch].area *cell* cell;;
			}

		if (slp_flag == 2) {
			flow_table[pch].total_gamma = mult * flow_table[pch].max_slope * flow_table[pch].area *cell* cell;;
			}

		printf("\n Total gamma for %d is %lf", flow_table[pch].patchID, flow_table[pch].total_gamma);

		}


			

	return(num_str);


    }



