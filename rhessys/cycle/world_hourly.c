/*--------------------------------------------------------------*/
/* 																*/
/*						world_hourly							*/
/*																*/
/*	NAME														*/
/*	world_hourly 												*/
/*				 - performs cycling and output of a world		*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void simulate_world_hourly 									*/
/*				(  struct world_object *,						*/
/*					struct command_line_object *,				*/
/*					struct tec_entry *,							*/
/*					struct date)								*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	basin in the world.  The routine also prints out results 	*/
/*	where specified by current tec events files.				*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void		world_hourly(
						 struct world_object *world,
						 struct command_line_object *command_line,
						 struct tec_entry *event,
						 struct date current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void	basin_hourly (
		struct world_object *,
		struct basin_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	basin;
	/*--------------------------------------------------------------*/
	/*	Create world hourly parameter array.						*/
	/*--------------------------------------------------------------*/
	if ( (world[0].hourly = (struct world_hourly_object *)
		calloc(1,sizeof(struct world_hourly_object))) == NULL ){
		fprintf(stderr,"FATAL ERROR: in simulate_world_hourly\n");
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/*	Simulate the basins											*/
	/*--------------------------------------------------------------*/
	for ( basin = 0 ; basin < world[0].num_basin_files ; basin++ ){
		basin_hourly(
			world,
			world[0].basins[basin],
			command_line,
			event,
			current_date);
	}
	/*--------------------------------------------------------------*/
	/*	Destory world hourly object									*/
	/*--------------------------------------------------------------*/
	free ( world[0].hourly );
	return;
} /*end world_hourly.c*/
