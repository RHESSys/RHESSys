/*--------------------------------------------------------------*/
/* 																*/
/*					input_new_zone								*/
/*																*/
/*	input_new_zone.c - creates a zone object					*/
/*																*/
/*	NAME														*/
/*	input_new_zone.c - creates a zone object					*/
/*																*/
/*	SYNOPSIS													*/
/*	void input_new_zone(										*/
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
#include <math.h>
#include "rhessys.h"

 void skip_zone(
								   struct	command_line_object	*command_line,
								   FILE	*world_file,
								   int		num_world_base_stations,
								   struct base_station_object **world_base_stations,
								   struct	default_object	*defaults,
								   struct	zone_object *zone)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct	base_station_object *assign_base_station(
		int ,
		int ,
		struct base_station_object **);
	
	
	void	*alloc(size_t, char *, char *);
	double	atm_pres( double );
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		base_stationID;
	int		i,dtmp;
	int		default_object_ID;
	char		record[MAXSTR];
	double		ltmp;
	
	/*--------------------------------------------------------------*/
	/*	Read in the next zone record for this hillslope.			*/
	/*--------------------------------------------------------------*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  zone[0].x = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  zone[0].y = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  {
		zone[0].z = ltmp;
		zone[0].metv.pa	= atm_pres( zone[0].z );
		}*/
 	fscanf(world_file,"%d",&(dtmp));
	read_record(world_file, record);
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  zone[0].area = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO) {
		zone[0].slope = ltmp * DtoR;
		zone[0].cos_slope = cos(zone[0].slope);
		zone[0].sin_slope = sin(zone[0].slope);
		}*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO) {
		zone[0].aspect = ltmp * DtoR;
		zone[0].cos_aspect = cos(zone[0].aspect);
		zone[0].sin_aspect = sin(zone[0].aspect);
		}*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  zone[0].precip_lapse_rate = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  zone[0].e_horizon = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  zone[0].w_horizon = ltmp;*/




	/*--------------------------------------------------------------*/
	/*	Allocate a list of base stations for this zone.          */
	/*--------------------------------------------------------------*/
 	fscanf(world_file,"%d",&(dtmp));
	read_record(world_file, record);
	if (dtmp > 0) {
		/*zone[0].num_base_stations = dtmp;*/
		/*zone[0].base_stations = (struct base_station_object **)
			alloc(zone[0].num_base_stations *
			sizeof(struct base_station_object *),
			"base_stations","skip_zone" );*/
		/*--------------------------------------------------------------*/
		/*	Read each base_station ID and then point to that base station */
		/*--------------------------------------------------------------*/
		for (i=0 ; i<dtmp ; i++ ){
			fscanf(world_file,"%d",&(ltmp));
			read_record(world_file, record);
			/*--------------------------------------------------------------*/
			/*  Point to the appropriate base station in the base           */
			/*              station list for this world.                    */
			/*                                                              */
			/*--------------------------------------------------------------*/
	/*		zone[0].base_stations[i] =	assign_base_station(
				base_stationID,
				num_world_base_stations,
				world_base_stations);*/
		} /*end for*/
	}
	else {
 	fscanf(world_file,"%d",&(dtmp));
	read_record(world_file, record);
	}

		/*--------------------------------------------------------------*/
		/*	Initialize any variables that should be initialized at	*/
		/*	the start of a simulation run for the zone.				*/
		/*--------------------------------------------------------------*/
	return;
} /*end input_new_zone.c*/
