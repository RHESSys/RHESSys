/*--------------------------------------------------------------*/
/*                                                              */
/*		assign_neighbours									*/
/*                                                              */
/*  NAME                                                        */
/*		assign_neighbours									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  assign_neighbours( struct patch_object *patch)				*/
/*						int num_neighbours,						*/
/*						FILE *routing_file)						*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*                                                              */
/*	assigns pointers to neighbours of each patch				*/
/*	as given in topology input file								*/
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
int assign_neighbours_in_hillslope( struct neighbour_object *neighbours,
					   int num_neighbours,
             struct hillslope_object *hillslope,
					   FILE *routing_file)
{
	/*--------------------------------------------------------------*/
	/*  Local function declaration                                  */
	/*--------------------------------------------------------------*/
	void *alloc (size_t, char *, char *);
	struct patch_object *find_patch_in_hillslope( int, int, struct hillslope_object*);

	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int i, inx, new_num_neighbours;
	int patch_ID, zone_ID, hill_ID;
	double gamma;
	double sum_gamma;
	struct patch_object *neigh;

	/*--------------------------------------------------------------*/
	/*  find and assign each neighbour to array						*/
	/*--------------------------------------------------------------*/
	inx = 0;
	sum_gamma = 0.0;
	new_num_neighbours = num_neighbours;
	for (i=0; i< num_neighbours; i++) {
		fscanf(routing_file,"%d %d %d %lf",
			&patch_ID,
			&zone_ID,
			&hill_ID,
			&gamma);
		/*----------------------------------------------------------------------*/
		/*	only attach neighbours which have a gamma > 0 			*/
		/* patches should point only to other patches in the same basin		*/
		/*	- ie. gamma to adjacent areas outside the basin 		*/
		/* 	should be zero							*/
		/* 	excepth in the case of a stream patch 				*/
		/*	 - this however is not strictly enforced			*/
		/*----------------------------------------------------------------------*/

//		printf("\t\tassign_neigh(%d): patch: %d, zone: %d, hill: %d\n", i, patch_ID, zone_ID, hill_ID);

		if (gamma > 0.0 ) {
			if( (patch_ID != 0) && (zone_ID != 0) && (hill_ID != 0) ) {
				neigh = find_patch_in_hillslope( patch_ID, zone_ID, hillslope);
			}else{
        fprintf( stderr, "cannot find patch %d in hill_ID %d, [zone_ID %d], [gamma %lf], [n_neig %d] \n", patch_ID, hill_ID, zone_ID, gamma, num_neighbours);
        exit(EXIT_FAILURE);
      }

			sum_gamma += gamma;
			neighbours[inx].gamma = gamma;
			neighbours[inx].patch = neigh;
			inx += 1;
		} else {
    	new_num_neighbours -= 1;
    }
	}


	return(new_num_neighbours);
}/*end assign_neighbours.c*/
