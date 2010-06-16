/*--------------------------------------------------------------*/
/*                                                              */
/*		check_neighbours									    */
/*                                                              */
/*  NAME                                                        */
/*		 check_neighbours										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 check_neighbours( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*		- examines the neighbourhood of the patch				*/
/*		figures out if it is at a border						*/
/*		at any border, length of perimeter is added 			*/
/*                                                              */
/*  revision: 6.0  29 April, 2005                               */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h> 
#include "blender.h"
#include <stdlib.h> 
#include <string.h>
#include "fileio.h"

int	check_neighbours(er,ec, patch, zone, hill, stream, flow_entry, num_adj, f1,
				maxr, maxc, sc_flag, cell)
	struct flow_struct *flow_entry;
	int	er,ec, num_adj;
	int	*patch;
	int	*hill;
	int	*stream;
	int	*zone;
	int	maxr, maxc;
	int	sc_flag;
	double	cell;
	FILE	*f1; 

    {

	/* local function declarations */

	struct adj_struct  *check_list();
	int check_list_min();
	int r,c;
	int p_neigh;
	int z_neigh;
	int h_neigh;
	int stream_neigh;
	int new_adj = 0;


	for (r=-1; r <= 1; r++) {
		for (c=-1; c<=1; c++) {
			/* don't look at neighbours beyond the edge */
			if ( (er+r < maxr) && (er+r >= 0) &&
				(ec+c < maxc) && (ec+c >= 0)) {

				/* is the neighbour a different patch*/
				/* or is it outside the basin - in which case we ignore it */
				/* also, for stream pixels, ignore non-stream neighbours */
				p_neigh = patch[(er+r)*maxc+(ec+c)];
				h_neigh = hill[(er+r)*maxc+(ec+c)];
				z_neigh = zone[(er+r)*maxc+(ec+c)];
				stream_neigh = stream[(er+r)*maxc+(ec+c)];


				if ((( p_neigh != flow_entry->patchID) || 
					( z_neigh != flow_entry->zoneID) || 
					( h_neigh != flow_entry->hillID) ) &&
					(( p_neigh > 0) && (z_neigh > 0) && (h_neigh > 0)))
				{

					/* if stream  add in stream network processing */
					/* create a list of downstream neighbours if it does not exist already */

					if ((flow_entry->land == 1) && (stream_neigh > 0)) {
					if ( (flow_entry->num_dsa == 0)  ) {
						flow_entry->num_dsa = 1;
						if ( (flow_entry->adj_str_list = (struct adj_struct *)malloc(
									sizeof(struct adj_struct))) == NULL)
						{
							printf("\nMemory Allocation Failed for %d", flow_entry->patchID);
							exit(1);
						}
						flow_entry->adj_str_list->patchID = p_neigh;
						flow_entry->adj_str_list->zoneID = z_neigh;
						flow_entry->adj_str_list->hillID = h_neigh;
						flow_entry->adj_str_list->perimeter = 0.0; 
						flow_entry->adj_str_list->next = NULL; 
						flow_entry->adj_str_ptr = flow_entry->adj_str_list;
						}
					else {
						/* search list for other entries for this neighbour */
						flow_entry->adj_str_ptr = flow_entry->adj_str_list;
						flow_entry->adj_str_ptr = (struct adj_struct *)check_list( 
									p_neigh, z_neigh, h_neigh, flow_entry->num_dsa, flow_entry->adj_str_list);
									
						/* is this the first instance of the neighbour */
					 	if ((( flow_entry->adj_str_ptr->patchID != p_neigh) || 
					 		(flow_entry->adj_str_ptr->zoneID != z_neigh) ||
					 		(flow_entry->adj_str_ptr->hillID != h_neigh)) &&
							 (( p_neigh > 0) && (z_neigh > 0) && (h_neigh > 0)))
							
							{
							if ( ( flow_entry->adj_str_ptr->next = (struct adj_struct *)malloc(
									sizeof(struct adj_struct))) == NULL)

							{
                                                        	printf("\nMemory Allocation Failed for %d", 
										flow_entry->patchID);
                                                        	exit(1);
                                              		}
							flow_entry->adj_str_ptr = flow_entry->adj_str_ptr->next;
							flow_entry->num_dsa += 1;
							flow_entry->adj_str_ptr->patchID = p_neigh;
							flow_entry->adj_str_ptr->zoneID = z_neigh;
							flow_entry->adj_str_ptr->hillID = h_neigh;
							flow_entry->adj_str_ptr->next = NULL;
							flow_entry->adj_str_ptr->perimeter = 0.0; 

							} /* end new entry */
					}
						
						if (abs((c+r)/ 2)*1.0/1.0== 0.5 )
							flow_entry->adj_str_ptr->perimeter += cell*0.5;
						else
							flow_entry->adj_str_ptr->perimeter += cell*1.0/sqrt(2.0);
					} /* end if stream */

					/* now do regular flowtable processing */
					/* create a list of neighbours if it does not exist already */
					if ( (num_adj == 0)  ) {
						num_adj = 1;
						if ( (flow_entry->adj_list = (struct adj_struct *)malloc(
									sizeof(struct adj_struct))) == NULL)
						{
							printf("\nMemory Allocation Failed for %d", flow_entry->patchID);
							exit(1);
						}
						flow_entry->adj_list->patchID = p_neigh;
						flow_entry->adj_list->zoneID = z_neigh;
						flow_entry->adj_list->hillID = h_neigh;
						flow_entry->adj_list->perimeter = 0.0; 
						flow_entry->adj_list->next = NULL; 
						flow_entry->adj_ptr = flow_entry->adj_list;
						}
					else {
						/* search list for other entries for this neighbour */
						flow_entry->adj_ptr = flow_entry->adj_list;
						flow_entry->adj_ptr = (struct adj_struct *)check_list( 
									p_neigh, z_neigh, h_neigh, num_adj, flow_entry->adj_list);
									
						/* is this the first instance of the neighbour */
					 	if ((( flow_entry->adj_ptr->patchID != p_neigh) || 
					 		(flow_entry->adj_ptr->zoneID != z_neigh) ||
					 		(flow_entry->adj_ptr->hillID != h_neigh)) &&
							 (( p_neigh > 0) && (z_neigh > 0) && (h_neigh > 0)))
							
							{
							/* land processing */
							if (flow_entry->land != 1) {
							if ( ( flow_entry->adj_ptr->next = (struct adj_struct *)malloc(
									sizeof(struct adj_struct))) == NULL)

							{
                                                        	printf("\nMemory Allocation Failed for %d", 
										flow_entry->patchID);
                                                        	exit(1);
                                              		}
							flow_entry->adj_ptr = flow_entry->adj_ptr->next;
							new_adj += 1;
							num_adj += 1;
							flow_entry->adj_ptr->patchID = p_neigh;
							flow_entry->adj_ptr->zoneID = z_neigh;
							flow_entry->adj_ptr->hillID = h_neigh;
							flow_entry->adj_ptr->perimeter = 0.0; 
							flow_entry->adj_ptr->next = NULL;

							} /* end land processing */
							else { 		/* stream processing */

								if ( ( flow_entry->adj_ptr->next = (struct adj_struct *)malloc(
									sizeof(struct adj_struct))) == NULL)

								{
                                                        		printf("\nMemory Allocation Failed for %d", 
										flow_entry->patchID);
                                                        		exit(1);
                                              			}
								flow_entry->adj_ptr = flow_entry->adj_ptr->next;
								new_adj += 1;
								num_adj += 1;
								flow_entry->adj_ptr->patchID = p_neigh;
								flow_entry->adj_ptr->zoneID = z_neigh;
								flow_entry->adj_ptr->hillID = h_neigh;
								flow_entry->adj_ptr->perimeter = 0.0;
								flow_entry->adj_ptr->next = NULL;
							}


							} /* end new entry */
					}

					/* add perimeter length */
					if (abs((c+r)/ 2)*1.0/1.0== 0.5 )
						flow_entry->adj_ptr->perimeter += cell*0.5;
					else
						flow_entry->adj_ptr->perimeter += cell*1.0/sqrt(2.0);

				} /* end is neigh */


			} /* end edges if */

		} /* end col */
	} /* end row */




	flow_entry->adj_ptr = flow_entry->adj_list;	
	flow_entry->adj_str_ptr = flow_entry->adj_str_list;	

	return(new_adj);

    }



 
struct adj_struct  *check_list( patchID, zoneID, hillID,  max, list) 
	int patchID, zoneID, hillID;
	int max;
	struct adj_struct *list;
	{

	int i, fnd;

	fnd = 0;
	i = 0;


	while ((fnd == 0) && (i < max)) {
		if ( (list->patchID == patchID) &&
			(list->zoneID == zoneID) &&
			(list->hillID == hillID) )
			fnd = 1;
		else {
			if (list->next == NULL)
				i = max;
			else { 
				i += 1;
				list = list->next;
				}
			}

		}

	return(list);

	}
		   

		
