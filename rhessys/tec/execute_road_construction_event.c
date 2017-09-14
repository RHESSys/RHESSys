/*--------------------------------------------------------------*/
/* 																*/
/*					execute_road_construction_event								*/
/*																*/
/*	execute_road_construction_event.c - creates a patch object					*/
/*																*/
/*	NAME														*/
/*	execute_road_construction_event.c - creates a patch object					*/
/*																*/
/*	SYNOPSIS													*/
/*	struct routing_list_object execute_road_construction_event( 		*/
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
#include <string.h>
#include <stdio.h>

#include "rhessys.h"
#include "functions.h"

void execute_road_construction_event(
									 struct	world_object *world,
									 struct	command_line_object	*command_line,
									 struct date	current_date)
{
//	/*--------------------------------------------------------------*/
//	/*	Local function definition.									*/
//	/*--------------------------------------------------------------*/
//
//	struct routing_list_object construct_routing_topology(
//		char *,
//		struct basin_object *);
	

	void *alloc(size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i, b;
	char		routing_filename[MAXSTR];
	char		ext[11];

	struct	basin_object	*basin;
	
	for (b=0; b< world[0].num_basin_files; b++) {
		basin = world[0].basins[b];
		free(basin->route_list->list);
		free(basin->route_list);
		free(basin->surface_route_list->list);
		free(basin->surface_route_list);
		/*--------------------------------------------------------------*/
		/*  Read in a new routing topology file.                    */
		/*--------------------------------------------------------------*/
		sprintf(ext,".Y%4dM%dD%dH%d",current_date.year,
			current_date.month,
			current_date.day,
			current_date.hour);
		strncpy(routing_filename, command_line[0].routing_filename, MAXSTR);
		strncat(routing_filename, ext, MAXSTR);
		printf("\nRedefining Flow Connectivity using %s\n", routing_filename);
		basin->route_list = construct_routing_topology( routing_filename, basin, command_line, false );

		if ( command_line->surface_routing_flag ) {
			strncpy(routing_filename, command_line->surface_routing_filename, MAXSTR);
			strncat(routing_filename, ext, MAXSTR);
			printf("\nRedefining Surface Flow Connectivity using %s\n", routing_filename);
		}
		basin->surface_route_list = construct_routing_topology( routing_filename, basin, command_line, true );
		if (basin->surface_route_list->num_patches != basin->route_list->num_patches) {
			fprintf(stderr,
					"\nFATAL ERROR: in execute_road_construction_event, surface routing table has %d patches, but subsurface routing table has %d patches. The number of patches must be identical.\n",
					basin->surface_route_list->num_patches,
					basin->route_list->num_patches);
			exit(EXIT_FAILURE);
		}

	} /* end basins */
	return;
} /*end execute_road_construction_event.c*/
