/*--------------------------------------------------------------*/
/* 																*/
/*					construct_zone								*/
/*																*/
/*	construct_zone.c - creates a zone object					*/
/*																*/
/*	NAME														*/
/*	construct_zone.c - creates a zone object					*/
/*																*/
/*	SYNOPSIS													*/
/*	struct zone_object *construct_zone(										*/
/*					struct	command_line_object	*command_line,	*/
/*					FILE	*world_file,					*/
/*					struct	default_object						*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Allocates memory for a zone object and reads in parameters  */
/*	from an already opened hillslopes file.						*/
/*																*/
/*	Invokes construction of patches in the zone.				*/
/*																*/
/*	Refer to construct_basin.c for a specification of the 		*/
/*	hillslopes file.											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We assume that the FILE pointers to the 					*/
/*	hillslope file are positioned properly.						*/
/*	 															*/
/*	We assume that the hillslope file has correct				*/
/*	syntax.														*/
/*																*/
/*	Original code, January 16, 1996.							*/
/*																*/
/*	 															*/ 
/* May 15, 1997		C.Tague					*/
/*	- C assumes radians so slope and aspect must 		*/
/*	be converted from degrees				*/
/* July 28, 1997	C.tague					*/
/*	- isohyet changed to a precip. lapse rate 		*/
/*													*/
/*	Sep 2, 1997 - RAF								*/
/*	Removed all references to extended objects or	*/
/*	grow objects in code.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

struct zone_object *construct_zone(
								   struct	command_line_object	*command_line,
								   FILE	*world_file,
								   int		num_world_base_stations,
								   struct base_station_object **world_base_stations,
								   struct	default_object	*defaults)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct	base_station_object *assign_base_station(
		int ,
		int ,
		struct base_station_object **);
	
	struct patch_object *construct_patch(
		struct command_line_object *,
		FILE	*,
		int		num_world_base_stations,
		struct base_station_object **world_base_stations,
		struct	default_object	*defaults);
	
	void	*alloc(size_t, char *, char *);
	double	atm_pres( double );
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		base_stationID;
	int		i;
	int		default_object_ID;
	char		record[MAXSTR];
	struct	zone_object *zone;

	/*--------------------------------------------------------------*/
	/*	Allocate a zone object.								*/
	/*--------------------------------------------------------------*/
	zone = (struct zone_object *) alloc( 1 *
		sizeof( struct zone_object ),"zone","construct_zone" );
	/*--------------------------------------------------------------*/
	/*	Read in the next zone record for this hillslope.			*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(zone[0].ID));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(zone[0].x));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(zone[0].y));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(zone[0].z));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(default_object_ID));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(zone[0].area));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(zone[0].slope));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(zone[0].aspect));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(zone[0].precip_lapse_rate));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(zone[0].e_horizon));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(zone[0].w_horizon));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(zone[0].num_base_stations));
	read_record(world_file, record);
     
	/*--------------------------------------------------------------*/
	/*	convert from degrees to radians for slope and aspects 	*/
	/*--------------------------------------------------------------*/
	zone[0].aspect = zone[0].aspect * DtoR;
	zone[0].slope = zone[0].slope * DtoR;
	/*--------------------------------------------------------------*/
	/*	Define cos and sine of slopes and aspects.					*/
	/*--------------------------------------------------------------*/
	zone[0].cos_aspect = cos(zone[0].aspect);
	zone[0].cos_slope = cos(zone[0].slope);
	zone[0].sin_aspect = sin(zone[0].aspect);
	zone[0].sin_slope = sin(zone[0].slope);
	/*--------------------------------------------------------------*/
	/*      Initialize accumulator variables                        */
	/*--------------------------------------------------------------*/
	zone[0].acc_month.K_direct = 0.0;
	zone[0].acc_month.K_diffuse = 0.0;
	zone[0].acc_month.tmax = 0.0;
	zone[0].acc_month.tmin = 0.0;
	zone[0].acc_month.precip = 0.0;
	zone[0].acc_month.length = 0;
	/*--------------------------------------------------------------*/
	/*	Define hourly array zone				*/
	/*--------------------------------------------------------------*/
	zone[0].hourly = (struct zone_hourly_object *) alloc(
		sizeof( struct zone_hourly_object ),
		"hourly","zone_hourly");
	/*--------------------------------------------------------------*/
	/*	Assign	defaults for this zone								*/
	/*--------------------------------------------------------------*/
	zone[0].defaults = (struct zone_default **)
		alloc( sizeof(struct zone_default *),"defaults",
		"construct_zone" );
	i = 0;
	while (defaults[0].zone[i].ID != default_object_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this zone.						    */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_zone_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_zone, zone default ID %d not found.\n",
				default_object_ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	zone[0].defaults[0] = &defaults[0].zone[i];
	/*--------------------------------------------------------------*/
	/*	Allocate a list of base stations for this zone.          */
	/*--------------------------------------------------------------*/
	zone[0].base_stations = (struct base_station_object **)
		alloc(zone[0].num_base_stations *
		sizeof(struct base_station_object *),
		"base_stations","construct_zone" );
	

     /*--------------------------------------------------------------*/
	/*	Read each base_station ID and then point to that base station */
	/*--------------------------------------------------------------*/
	for (i=0 ; i<zone[0].num_base_stations ; i++ ){
		fscanf(world_file,"%d",&(base_stationID));
		read_record(world_file, record);
		/*--------------------------------------------------------------*/
		/*  Point to the appropriate base station in the base           */
		/*              station list for this world.                    */
		/*                                                              */
		/*--------------------------------------------------------------*/
		zone[0].base_stations[i] =	assign_base_station(
			base_stationID,
			num_world_base_stations,
			world_base_stations);
	} /*end for*/
 
	/*--------------------------------------------------------------*/
	/*	Read in number of patches in this zone.						*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(zone[0].num_patches));
	read_record(world_file, record);
	/*--------------------------------------------------------------*/
	/*	Allocate list of pointers to patch objects .				*/
	/*--------------------------------------------------------------*/
     zone[0].patches = ( struct patch_object ** )
		alloc( zone[0].num_patches * sizeof( struct patch_object *),
		"patches","construct_zone");

	/*--------------------------------------------------------------*/
	/*	Initialize any variables that should be initialized at	*/
	/*	the start of a simulation run for the zone.				*/
	/* for temperaturature we initialize to 999 so that they will be set on the first day based */
	/* on air temperatures on that day - which we don't know at this point */
	/*--------------------------------------------------------------*/
	zone[0].metv.pa	= atm_pres( zone[0].z );
	zone[0].metv.tsoil_sum = 0.0;
	zone[0].metv.tsoil = 0.0;
	zone[0].metv.tmin_ravg = 3.0;
	zone[0].metv.vpd_ravg = 900;
	zone[0].metv.dayl_ravg = 38000;
     

	/*--------------------------------------------------------------*/
	/*	Construct the intervals in this zone.						*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<zone[0].num_patches ; i++ ){
		zone[0].patches[i] = construct_patch(
			command_line,
			world_file,
			num_world_base_stations,
			world_base_stations,
			defaults);
		zone[0].patches[i][0].zone = zone;
	} /*end for*/
     

	return(zone);
} /*end construct_zone.c*/
