/*--------------------------------------------------------------*/
/* 																*/
/*					construct_ddn_routing_topology					*/
/*																*/
/*	construct_ddn_routing_topology.c - creates a patch object		*/
/*																*/
/*	NAME														*/
/*	construct_ddn_routing_topology.c - creates a patch object		*/
/*																*/
/*	SYNOPSIS													*/
/*	struct routing_list_object construct_ddn_routing_topology( 		*/
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
#include <stdlib.h>
#include "rhessys.h"

struct routing_list_object *construct_ddn_routing_topology(char *routing_filename,
		  struct basin_object *basin)
													  
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
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i, d;
	int		num_patches, num_innundation_depths, num_neighbours;
	int		patch_ID, zone_ID, hill_ID;
	int		drainage_type;
	double	x,y,z, area, gamma, width, critical_depth;
	FILE	*routing_file;
	struct routing_list_object	*rlist;
	struct	patch_object	*patch;
	struct	patch_object	*stream;
	
	rlist = (struct routing_list_object	*)alloc( sizeof(struct routing_list_object), "rlist", "construct_routing_topology");

	/*--------------------------------------------------------------*/
	/*  Try to open the routing file in read mode.                    */
	/*--------------------------------------------------------------*/
	if ( (routing_file = fopen(routing_filename,"r")) == NULL ){
		fprintf(stderr,"FATAL ERROR:  Cannot open routing file %s\n",
			routing_filename);
		exit(EXIT_FAILURE);
	} /*end if*/
	fscanf(routing_file,"%d",&num_patches);
	rlist->num_patches = num_patches;
	rlist->list = (struct patch_object **)alloc(
		num_patches * sizeof(struct patch_object *), "patch list",
		"construct_ddn_routing_topography");
	/*--------------------------------------------------------------*/
	/*	Read in  each patch record and find it		.				*/
	/*	if it is a stream add it to the basin level routing list	*/
	/*	otherwise add it to the hillslope level routing list		*/
	/*--------------------------------------------------------------*/
	for (i=0; i< num_patches; ++i) {
		fscanf(routing_file,"%d %d %d",
			&patch_ID,
			&zone_ID,
			&hill_ID);
		fscanf(routing_file,"%lf %lf %lf", &x,&y,&z);
		fscanf(routing_file,"%lf %d %d", 
			&area,
			&area,
			&drainage_type,
			&num_innundation_depths);

		if  ( (patch_ID != 0) && (zone_ID != 0) && (hill_ID != 0) )
			patch = find_patch(patch_ID, zone_ID, hill_ID, basin);
		else
			patch = basin[0].outside_region;
		rlist->list[i] = patch;
		patch[0].num_innundation_depths = num_innundation_depths;
		patch[0].stream_gamma = 0.0;
		patch[0].drainage_type = drainage_type;
		/*--------------------------------------------------------------*/
		/*  Allocate innundation depth array				*/
		/*--------------------------------------------------------------*/
		patch[0].innundation_list = (struct innundation_object *)alloc(num_innundation_depths *
		sizeof(struct innundation_object), "innundation_list", "assign_neighbours");

		for (d=0; d<num_innundation_depths; d++) {
			fscanf(routing_file,"%lf %lf %d", &critical_depth, &gamma, &num_neighbours);

			if (num_innundation_depths > 1)
				patch[0].innundation_list[d].critical_depth = critical_depth;
			else
				patch[0].innundation_list[d].critical_depth = NULLVAL;

			gamma = gamma * patch[0].soil_defaults[0][0].m * patch[0].soil_defaults[0][0].Ksat_0;
			
			patch[0].innundation_list[d].gamma	 = gamma;
			/*--------------------------------------------------------------*/
			/*  Allocate neighbour array									*/
			/*--------------------------------------------------------------*/
			patch[0].innundation_list[d].neighbours = (struct neighbour_object *)alloc(num_neighbours *
			sizeof(struct neighbour_object), "neighbours", "assign_neighbours");
			patch[0].innundation_list[d].num_neighbours = assign_neighbours(patch[0].innundation_list[d].neighbours, num_neighbours, basin, routing_file);
		
		}
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
	}

	fclose(routing_file);

	return(rlist);
} /*end construct_ddn_routing_topology.c*/

