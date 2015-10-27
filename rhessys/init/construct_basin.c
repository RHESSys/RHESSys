/*--------------------------------------------------------------*/
/* 																*/
/*					construct_basin								*/
/*																*/
/*	construct_basin.c - creates a basin object					*/
/*																*/
/*	NAME														*/
/*	construct_basin.c - creates a basin object					*/
/*																*/
/*	SYNOPSIS													*/
/*	void construct_basin(										*/
/*			struct	command_line_object	*command_line,			*/
/*			FILE	*world_file									*/
/*			int		num_world_base_stations,					*/
/*			struct base_station_object	**world_base_stations,	*/
/*			struct basin_object	**basin_list,					*/
/*			struct default_object *defaults)					*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Constructs the basin object which consists of:				*/
/*		- basin specific parameters and identification			*/
/*		- a possible extension to a grow object					*/
/*		- a list of hillslopes in the basin.					*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Basins dont own climate files since all of their hillslopes	*/
/*	own them instead.  I guess this means we have to compute	*/
/*	a lot of local climate info but perhaps this is more		*/
/*	correct anyways.  As usual, it is up to the user to         */
/*	aggregate model  output from each hillslope if they want	*/
/*	basin values.												*/
/*																*/
/*	We use a list of pointers to hillslope objects rather than	*/
/*	a contiguous array of hillslope objects with a pointer to	*/
/*	the head of the array.  The list of pointers is a bit less	*/
/*	efficient since the pointer must be placed in the heap (RAM)*/
/*	at the start of each object BUT								*/
/*																*/
/*		1.  We can dynamically add and remove hillslopes.		*/
/*		2.  Most of the processing time is required on the 		*/
/*			sub-hillslope basis so the repositioning of pointers*/
/*			will not be too drastic if it is limited to the 	*/
/*			hillslope level or up.								*/
/*		3.  We will be able to make use of smaller chunks of 	*/
/*			RAM.  												*/
/*	Original code, January 16, 1996.							*/
/*	May 7, 1997	C.Tague											*/
/* 		- added a routine to sort hierarchy by elevation 		*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rhessys.h"
#include "functions.h"

struct basin_object *construct_basin(
									 struct	command_line_object	*command_line,
									 FILE	*world_file,
									 int	*num_world_base_stations,
									 struct base_station_object	**world_base_stations,
									 struct	default_object	*defaults,
									 struct base_station_ncheader_object *base_station_ncheader,
									 struct world_object *world)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
								int,
								int,
								struct base_station_object **);
	
	struct hillslope_object *construct_hillslope(
		struct	command_line_object *,
		FILE    *,
		int		*,
		struct base_station_object **,
		struct	default_object *,
		struct base_station_ncheader_object *,
		struct world_object *);
	
	void	*alloc( 	size_t, char *, char *);
	
	void	sort_by_elevation( struct basin_object *);
	
//	struct routing_list_object construct_ddn_routing_topology(
//		char *,
//		struct basin_object *);

//	struct routing_list_object construct_routing_topology(
//		char *,
//		struct basin_object *,
//		struct	command_line_object *);
	
	struct stream_list_object construct_stream_routing_topology(
		char *,
		struct basin_object *, 
		struct	command_line_object *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	base_stationID;
	int		i,j,z;
	int		default_object_ID;
	double		check_snow_scale;
	double		n_routing_timesteps;
	char		record[MAXSTR];
	struct basin_object	*basin;
	
	/*--------------------------------------------------------------*/
	/*	Allocate a basin object.								*/
	/*--------------------------------------------------------------*/
	basin = (struct basin_object *) alloc( 1 *
		sizeof( struct basin_object ),"basin","construct_basin");
	
	/*--------------------------------------------------------------*/
	/*	Read in the basinID.									*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(basin[0].ID));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(basin[0].x));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(basin[0].y));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(basin[0].z));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(default_object_ID));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(basin[0].latitude));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(basin[0].num_base_stations));
	read_record(world_file, record);
	
	/*--------------------------------------------------------------*/
	/*	Create cosine of latitude to save future computations.		*/
	/*--------------------------------------------------------------*/
	basin[0].cos_latitude = cos(basin[0].latitude*DtoR);
	basin[0].sin_latitude = sin(basin[0].latitude*DtoR);
	
	/*--------------------------------------------------------------*/
	/*    Allocate a list of base stations for this basin.			*/
	/*--------------------------------------------------------------*/
	basin[0].base_stations = (struct base_station_object **)
		alloc(basin[0].num_base_stations *
		sizeof(struct base_station_object *),"base_stations","construct_basin");
	/*--------------------------------------------------------------*/
	/*      Read each base_station ID and then point to that base_statio*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<basin[0].num_base_stations; i++) {
		fscanf(world_file,"%d",&(base_stationID));
		read_record(world_file, record);
		/*--------------------------------------------------------------*/
		/*	Point to the appropriate base station in the base       	*/
		/*              station list for this world.					*/
		/*--------------------------------------------------------------*/
		basin[0].base_stations[i] = assign_base_station(
			base_stationID,
			*num_world_base_stations,
			world_base_stations);
		
	} /*end for*/

	/*--------------------------------------------------------------*/
	/*	Create the grow subobject if needed.						*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].grow_flag == 1 ){
		/*--------------------------------------------------------------*/
		/*		Allocate memory for the grow subobject.					*/
		/*--------------------------------------------------------------*/
		basin[0].grow = (struct grow_basin_object *)
			alloc(1 * sizeof(struct grow_basin_object),
			"grow","construct_basin");
		/*--------------------------------------------------------------*/
		/*	NOTE:  PUT READS FOR GROW SUBOBJECT HERE.					*/
		/*--------------------------------------------------------------*/
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*  Assign  defaults for this basin                             */
	/*--------------------------------------------------------------*/
	basin[0].defaults = (struct basin_default **)
		alloc( sizeof(struct basin_default *),"defaults","construct_basin" );
	i = 0;
	while (defaults[0].basin[i].ID != default_object_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this basin.                         */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_basin_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_basin,basin default ID %d not found.\n",
				default_object_ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	basin[0].defaults[0] = &defaults[0].basin[i];

	/*--------------------------------------------------------------*/
	/*	Read in the number of hillslopes.						*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(basin[0].num_hillslopes));
	read_record(world_file, record);
	
	/*--------------------------------------------------------------*/
	/*	Allocate a list of pointers to hillslope objects.			*/
	/*--------------------------------------------------------------*/
	basin[0].hillslopes = (struct hillslope_object **)
		alloc(basin[0].num_hillslopes * sizeof(struct hillslope_object *),
		"hillslopes","construct_basin");

	basin[0].area = 0.0;
	basin[0].max_slope = 0.0;
	n_routing_timesteps = 0.0;
	check_snow_scale = 0.0;
	/*--------------------------------------------------------------*/
	/*	Construct the hillslopes for this basin.					*/
	/*--------------------------------------------------------------*/
	for (i=0; i<basin[0].num_hillslopes; i++){
		basin[0].hillslopes[i] = construct_hillslope(
			command_line, world_file, num_world_base_stations,
			world_base_stations, defaults, base_station_ncheader, world);
		basin[0].area += basin[0].hillslopes[i][0].area;
		n_routing_timesteps += basin[0].hillslopes[i][0].area *
			basin[0].hillslopes[i][0].defaults[0][0].n_routing_timesteps;
		if (basin[0].max_slope < basin[0].hillslopes[i][0].slope)
			basin[0].max_slope = basin[0].hillslopes[i][0].slope;
		if (command_line[0].snow_scale_flag == 1) {
			for (z = 0; z < basin[0].hillslopes[i][0].num_zones; z++) {
				for (j=0; j < basin[0].hillslopes[i][0].zones[z][0].num_patches; j++) { 
				check_snow_scale +=
				basin[0].hillslopes[i][0].zones[z][0].patches[j][0].snow_redist_scale *
				basin[0].hillslopes[i][0].zones[z][0].patches[j][0].area;
				}
			}	
		}
	};
	
	basin[0].defaults[0][0].n_routing_timesteps = 
			(int) (n_routing_timesteps / basin[0].area);

	if (basin[0].defaults[0][0].n_routing_timesteps < 1)
		basin[0].defaults[0][0].n_routing_timesteps = 1;
 
	if (command_line[0].snow_scale_flag == 1) {
		check_snow_scale /= basin[0].area;
		if (fabs(check_snow_scale - 1.0) > ZERO	) {
			printf(" *******  WARNING  ********** \n");
			printf(" Basin-wide  average snow scale is %lf \n", check_snow_scale);
			printf(" Snow rescaling will alter net precip input by this scale factor \n");
			}
		if (command_line[0].snow_scale_tol > ZERO) {
			if ((check_snow_scale > command_line[0].snow_scale_tol) || 
				(check_snow_scale < 1/command_line[0].snow_scale_tol)) {
				printf("Basin-wide average snow scale %lf is outside tolerance %lf \n", 
				check_snow_scale, command_line[0].snow_scale_tol);
				printf("\n Exiting\n");
				exit(EXIT_FAILURE);
				}
			}
	}

	/*--------------------------------------------------------------*/
	/*      initialize accumulator variables for this patch         */
	/*--------------------------------------------------------------*/
	basin[0].acc_month.et = 0.0;
	basin[0].acc_month.snowpack = 0.0;
	basin[0].acc_month.theta = 0.0;
	basin[0].acc_month.streamflow = 0.0;
	basin[0].acc_month.length = 0;
	basin[0].acc_month.denitrif = 0.0;
	basin[0].acc_month.nitrif = 0.0;
	basin[0].acc_month.mineralized = 0.0;
	basin[0].acc_month.uptake = 0.0;
	basin[0].acc_month.lai = 0.0;
	basin[0].acc_month.leach = 0.0;
	basin[0].acc_month.DOC_loss = 0.0;
	basin[0].acc_month.DON_loss = 0.0;
	basin[0].acc_month.stream_NO3 = 0.0;
	basin[0].acc_month.stream_NH4 = 0.0;
	basin[0].acc_month.stream_DON = 0.0;
	basin[0].acc_month.stream_DOC = 0.0;
	basin[0].acc_month.PET = 0.0;
	basin[0].acc_month.psn = 0.0;
	basin[0].acc_month.num_threshold = 0;


	basin[0].acc_year.et = 0.0;
	basin[0].acc_year.snowpack = 0.0;
	basin[0].acc_year.theta = 0.0;
	basin[0].acc_year.streamflow = 0.0;
	basin[0].acc_year.length = 0;
	basin[0].acc_year.denitrif = 0.0;
	basin[0].acc_year.nitrif = 0.0;
	basin[0].acc_year.mineralized = 0.0;
	basin[0].acc_year.uptake = 0.0;
	basin[0].acc_year.lai = 0.0;
	basin[0].acc_year.leach = 0.0;
	basin[0].acc_year.DOC_loss = 0.0;
	basin[0].acc_year.DON_loss = 0.0;
	basin[0].acc_year.stream_NO3 = 0.0;
	basin[0].acc_year.stream_NH4 = 0.0;
	basin[0].acc_year.stream_DON = 0.0;
	basin[0].acc_year.stream_DOC = 0.0;
	basin[0].acc_year.PET = 0.0;
	basin[0].acc_year.psn = 0.0;
	basin[0].acc_year.num_threshold = 0;

	/*--------------------------------------------------------------*/
	/*	Sort sub-hierarchy in the basin by elevation				*/
	/*--------------------------------------------------------------*/
	sort_by_elevation(basin);

	/*--------------------------------------------------------------*/
	/*	Read in flow routing topology for routing option	*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].routing_flag == 1 ) {
		basin[0].outside_region = (struct patch_object *) alloc (1 *
			sizeof(struct patch_object) , "patch",
			"construct_basin");
		basin[0].outside_region[0].sat_deficit = 0.0;
		basin[0].outside_region[0].ID = 0;
		if ( command_line[0].ddn_routing_flag == 1 ) {
			basin->route_list = construct_ddn_routing_topology( command_line[0].routing_filename, basin);
		} else {
			basin->route_list = construct_routing_topology( command_line[0].routing_filename, basin,
						command_line, false);
			if ( command_line->surface_routing_flag ) {
				printf("\tReading surface routing table\n");
				basin->surface_route_list =
						construct_routing_topology( command_line->surface_routing_filename, basin,
								command_line, true);
				if ( basin->surface_route_list->num_patches != basin->route_list->num_patches ) {
					fprintf(stderr,
							"\nFATAL ERROR: in construct_basin, surface routing table has %d patches, but subsurface routing table has %d patches. The number of patches must be identical.\n",
							basin->surface_route_list->num_patches, basin->route_list->num_patches);
					exit(EXIT_FAILURE);
				}
			} else {
				// No surface routing table specified, use sub-surface for surface
				basin->surface_route_list =
						construct_routing_topology( command_line->routing_filename, basin,
								command_line, true);
			}
		}
	} else { // command_line[0].routing_flag != 1
		// For TOPMODEL mode, make a dummy route list consisting of all patches
		// in the basin, in no particular order.
		basin->route_list = construct_topmodel_patchlist(basin);
	}
	
	/*--------------------------------------------------------------*/
	/*	Read in stream routing topology if needed	*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].stream_routing_flag == 1) {
			basin[0].stream_list = construct_stream_routing_topology( command_line[0].stream_routing_filename, basin,
						command_line);
	}
		else { 
			basin[0].stream_list.stream_network = NULL;
			basin[0].stream_list.streamflow = 0.0;
		}

	return(basin);
} /*end construct_basin.c*/
