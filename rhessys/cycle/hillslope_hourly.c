/*--------------------------------------------------------------*/
/* 																*/
/*						hillslope_hourly						*/
/*																*/
/*	NAME														*/
/*	hillslope_hourly 											*/
/*				 - performs cycling and output of a hillslope	*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void hillslope_hourly(										*/
/*					struct	world_object		*,				*/
/*					struct	basin_object		*,				*/
/*					struct 	hillslope_object 	*,				*/
/*					struct 	command_line_object *,				*/
/*					struct 	tec_entry 			*,				*/
/*					struct 	date 				);				*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	hillslope.			  The routine also prints out results	*/
/*	where specified by current tec events files.				*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	March 9, 1997 C. Tague										*/
/*	- included if/print statement to get rid of random error	*/
/*		which changed the value of zone.Kdown_diffuse			*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void		hillslope_hourly(
							 struct	world_object		*world,
							 struct	basin_object		*basin,
							 struct 	hillslope_object 	*hillslope,
							 struct 	command_line_object *command_line,
							 struct 	tec_entry 			*event,
							 struct 	date 				current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void zone_hourly (
		struct world_object *,
		struct basin_object *,
		struct hillslope_object *,
		struct zone_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	zone;
	/*--------------------------------------------------------------*/
	/*	Allocate the hillslope houly parameter array.				*/
	/*--------------------------------------------------------------*/
	if ( (hillslope[0].hourly = (struct hillslope_hourly_object *)
		calloc(1,sizeof(struct hillslope_hourly_object))) == NULL ){
		fprintf(stderr,"FATAL ERROR: in hillslope_hourly\n");
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/* do redistribution of saturated zone at patch level based on 	*/
	/* previous time steps hillslope level soilwater 				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/* an alternative to TOPMODEL could go here (with a flag)		*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Simulate zones requested.								*/
	/*--------------------------------------------------------------*/
	for ( zone=0 ; zone < hillslope[0].num_zones ; zone++ ){
		if ( hillslope[0].zones[zone][0].Kdown_diffuse > 1.0e100)
			printf("\n Date %d %d %d %d is Zone Hourly Diffuse is %10.6f",
			current_date.year,
			current_date.month,
			current_date.day,
			current_date.hour,
			hillslope[0].zones[zone][0].Kdown_diffuse);
		zone_hourly(
			world,
			basin,
			hillslope,
			hillslope[0].zones[zone],
			command_line,
			event,
			current_date );
          //fprintf(stderr, "\nzone %d, hillslope : %d, hour: %d", hillslope[0].zones[zone], hillslope[0].ID, current_date.hour);
	}
     
	/*--------------------------------------------------------------*/
	/*	Destroy the hillslope hourloy object.						*/
	/*--------------------------------------------------------------*/
	free( hillslope[0].hourly );

     
} /*end hillslope_hourly.c*/
