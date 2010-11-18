/*--------------------------------------------------------------*/
/* 																*/
/*					construct_routing_topology					*/
/*																*/
/*	construct_routing_topology.c - creates a patch object		*/
/*																*/
/*	NAME														*/
/*	construct_routing_topology.c - creates a patch object		*/
/*																*/
/*	SYNOPSIS													*/
/*	struct routing_list_object construct_routing_topology( 		*/
/*							struct basin_object *basin)			*/
/*																*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*  reads routing topology from input file						*/
/*	creates neighbourhood structure for each patch in the basin */
/*	returns a list giving order for patch-level routing			*/
/*																*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

struct routing_list_object construct_routing_topology(
					  		  char *routing_filename,
							  struct basin_object *basin,
							  struct command_line_object *command_line)
													  
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct patch_object *find_patch(int, int, int, struct basin_object *);
	
	int assign_neighbours (struct neighbour_object *,
		int,
		struct basin_object *,
		FILE *);
	
	void *alloc(size_t, char *, char *);

	double * compute_transmissivity_curve( double, struct patch_object *, struct command_line_object *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i,d,j;
	int		num_patches, num_neighbours;
	int		patch_ID, zone_ID, hill_ID;
	int		drainage_type;
	double	x,y,z, area, gamma, width;
	FILE	*routing_file;
	struct routing_list_object	rlist;
	struct	patch_object	*patch;
	struct	patch_object	*stream;
	
	/*--------------------------------------------------------------*/
	/*  Try to open the routing file in read mode.                    */
	/*--------------------------------------------------------------*/
	if ( (routing_file = fopen(routing_filename,"r")) == NULL ){
		fprintf(stderr,"FATAL ERROR:  Cannot open routing file %s\n",
			routing_filename);
		exit(0);
	} /*end if*/
	fscanf(routing_file,"%d",&num_patches);
	rlist.num_patches = num_patches;
	rlist.list = (struct patch_object **)alloc(
		num_patches * sizeof(struct patch_object *), "patch list",
		"construct_routing_topography");


	/*--------------------------------------------------------------*/
	/*	Read in  each patch record and find it		.				*/
	/*	if it is a stream add it to the basin level routing list	*/
	/*	otherwise add it to the hillslope level routing list		*/
	/*--------------------------------------------------------------*/
	for (i=0; i< num_patches; ++i) {
		fscanf(routing_file,"%d %d %d %lf %lf %lf %lf %lf %d %lf %d",
			&patch_ID,
			&zone_ID,
			&hill_ID,
			&x,&y,&z,
			&area,
			&area,
			&drainage_type,
			&gamma,
			&num_neighbours);

		if  ( (patch_ID != 0) && (zone_ID != 0) && (hill_ID != 0) )
			patch = find_patch(patch_ID, zone_ID, hill_ID, basin);
		else
			patch = basin[0].outside_region;
		rlist.list[i] = patch;

		if ((patch[0].soil_defaults[0][0].m < ZERO) || (patch[0].soil_defaults[0][0].Ksat_0 < ZERO))	
			printf("\n WARNING m (%lf) or Ksat (%lf) are close to zero for patch %d",
				patch[0].soil_defaults[0][0].m , patch[0].soil_defaults[0][0].Ksat_0, patch[0].ID);
		
		 gamma = gamma * patch[0].soil_defaults[0][0].m * patch[0].soil_defaults[0][0].Ksat_0;

		/*--------------------------------------------------------------*/
		/*  Allocate innundation list array				*/
		/*	note for this routing there is only one innundation depth 	*/
		/*	however it is need to be compatablability 		*/
		/*--------------------------------------------------------------*/
		d=0;
		patch[0].innundation_list = (struct innundation_object *)alloc( 1 *
		sizeof(struct innundation_object), "innundation_list", "construct_routing_topology");


		patch[0].num_innundation_depths=1;

		patch[0].innundation_list[d].num_neighbours = num_neighbours;
		patch[0].innundation_list[d].gamma = gamma;
		patch[0].innundation_list[d].critical_depth = NULLVAL;
		patch[0].stream_gamma = 0.0;
		patch[0].drainage_type = drainage_type;
		if ((patch[0].drainage_type != STREAM) && (patch[0].innundation_list[d].gamma < ZERO)) {
			printf("\n non-stream patches with zero gamma %d switched to stream for now", patch[0].ID);
			patch[0].drainage_type = STREAM;
			}

		/*--------------------------------------------------------------*/
		/*  Allocate neighbour array									*/
		/*--------------------------------------------------------------*/
		patch[0].innundation_list[d].neighbours = (struct neighbour_object *)alloc(num_neighbours *
		sizeof(struct neighbour_object), "neighbours", "construct_routing_topology");
		num_neighbours = assign_neighbours(patch[0].innundation_list[d].neighbours, num_neighbours, basin, routing_file);
		if ((num_neighbours == -9999) && (patch[0].drainage_type != STREAM))
			printf("\n WARNING sum of patch %d neigh gamma is not equal to 1.0", patch[0].ID); 
		else		
			patch[0].innundation_list[d].num_neighbours = num_neighbours;
		if (drainage_type == 2) {
			fscanf(routing_file,"%d %d %d %lf",
				&patch_ID,
				&zone_ID,
				&hill_ID,
				&width);
			patch[0].stream_gamma = gamma;
			patch[0].road_cut_depth = width * tan(patch[0].slope);
			stream = find_patch(patch_ID, zone_ID, hill_ID, basin);
			patch[0].next_stream = stream;
		}

		/*--------------------------------------------------------------*/
		/*	create a vector of transmssivities 			*/
		/*--------------------------------------------------------------*/
		patch[0].num_soil_intervals = (int) lround(patch[0].soil_defaults[0][0].soil_water_cap / INTERVAL_SIZE);
		patch[0].transmissivity_profile = compute_transmissivity_curve(gamma, patch, command_line);


	}
	return(rlist);
} /*end construct_routing_topology.c*/

