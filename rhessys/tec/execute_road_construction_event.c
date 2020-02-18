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
  char    redefine_routing_filename[ MAXSTR ];
	char		ext[14];

	struct	basin_object	*basin;
  struct  hillslope_object *hillslope;

  FILE * redefine_routing_file;
  FILE * redefine_surface_routing_file;
	
	for (b=0; b< world[0].num_basin_files; b++) {
		basin = world[0].basins[b];
		/*--------------------------------------------------------------*/
		/*  Read in a new routing topology file.                    */
		/*--------------------------------------------------------------*/
		sprintf(
      ext,
      ".Y%4dM%dD%dH%d",
      current_date.year,
			current_date.month,
			current_date.day,
			current_date.hour
    );

		strncpy(redefine_routing_filename, command_line[0].routing_filename, MAXSTR);
		strncat(redefine_routing_filename, ext, MAXSTR);
		printf("\nRedefining Flow Connectivity using %s\n", routing_filename);
		//basin->route_list = construct_routing_topology( routing_filename, basin, command_line, false );

		if ( command_line->surface_routing_flag ) {
			strncpy( redefine_routing_filename, command_line->surface_routing_filename, MAXSTR);
			strncat( redefine_routing_filename, ext, MAXSTR);
			printf("\nRedefining Surface Flow Connectivity using %s\n", redefine_routing_filename);
		}
		//basin->surface_route_list = construct_routing_topology( routing_filename, basin, command_line, true );


	/*--------------------------------------------------------------*/
	/*	Read in flow routing topology for routing option	*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].routing_flag == 1 ) {

	/*--------------------------------------------------------------*/
	/*  Try to open the routing file in read mode.                    */
	/*--------------------------------------------------------------*/
	  if ( (redefine_routing_file = fopen( routing_filename,"r")) == NULL ){
		  fprintf(stderr,"FATAL ERROR:  Cannot open routing file %s\n",
			command_line[0].routing_filename);
		  exit(EXIT_FAILURE);
	  } 

	  if ( (redefine_surface_routing_file = fopen( redefine_routing_filename,"r")) == NULL ){
		  fprintf(stderr,"FATAL ERROR:  Cannot open routing file %s\n",
			command_line[0].surface_routing_filename);
		  exit(EXIT_FAILURE);
	  }

	  int num_hillslopes = basin[0].num_hillslopes;
      // steps:
      // 1. get hillslope ID by reading from routing file
      // 2. get pointer to hillslope using id from #1 and 
      //    find_hillslope_in_basin()
      // 3. call construct_routing_topology and pass in hillslope
      //    from #2

    // THIS IS WHERE OPENMP WILL PARALLELIZE STUFF
	  for (int i=0; i<num_hillslopes; i++){
          
      hillslope = basin[0].hillslopes[ i ];//find_hillslope_in_basin(hillslope[0].ID, basin);
      free(hillslope->route_list->list);
      free(hillslope->route_list);
      free(hillslope->surface_route_list->list);
      free(hillslope->surface_route_list);

      if ( command_line[0].ddn_routing_flag == 1 ) {
        hillslope->route_list = construct_ddn_routing_topology( redefine_routing_file, hillslope );
      } else {
        hillslope->route_list = construct_routing_topology( redefine_routing_file, hillslope, command_line, false);

        if ( command_line->surface_routing_flag == 1 ) {
          printf("\tReading surface routing table\n");
          hillslope->surface_route_list = construct_routing_topology( redefine_surface_routing_file, hillslope, command_line, true);

          if ( hillslope->surface_route_list->num_patches != hillslope->route_list->num_patches ) {
             fprintf(stderr,
                "\nFATAL ERROR: in construct_hillslope, surface routing table has %d patches, but subsurface routing table has %d patches. The number of patches must be identical.\n",
                hillslope->surface_route_list->num_patches, hillslope->route_list->num_patches);
                exit(EXIT_FAILURE);
          }
        } else {
         // No surface routing table specified, use sub-surface for surface
         hillslope->surface_route_list = construct_routing_topology( redefine_routing_file, hillslope, command_line, true);
        }
      }
      if (hillslope->surface_route_list->num_patches != hillslope->route_list->num_patches) {
			  fprintf(
          stderr,
					"\nFATAL ERROR: in execute_road_construction_event, surface routing table has %d patches, but subsurface routing table has %d patches. The number of patches must be identical.\n",
					hillslope->surface_route_list->num_patches,
					hillslope->route_list->num_patches
        );
			  exit(EXIT_FAILURE);
		  }
          //construct_routing_topology(hillslope, command_line, false);
    }	
	} 
	} /* end basins */
	return;
} /*end execute_road_construction_event.c*/
