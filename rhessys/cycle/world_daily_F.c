/*--------------------------------------------------------------*/
/* 																*/
/*							world_daily_F							*/
/*																*/
/*	NAME														*/
/*	world_daily_F													*/
/*				 - performs cycling and output of a world		*/
/*					with parallel hillslope simulations			*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void		world_daily_F(									*/
/*					long   	day,								*/
/*					struct world_object *world,					*/
/*					struct command_line_object *command_line,	*/
/*					struct tec_entry *event,					*/
/*					struct date current_date)					*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Performs daily simulation of the world with hillslope and	*/
/*	lower level executed in parallel.							*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	At the moment this routine only handles setting Io and 		*/
/*	declination for the world.  								*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

void world_daily_F(
				   long   	day,
				   struct world_object *world,
				   struct command_line_object *command_line,
				   struct tec_entry *event,
				   struct date current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void	basin_daily_F(
		long	,
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
	/*	Simulate over all of the basins.							*/
	/*--------------------------------------------------------------*/
  world[0].target_status = 1;
	for ( basin = 0; basin < world[0].num_basin_files; basin++ ){
		basin_daily_F(	day,
			world,
			world[0].basins[basin],
			command_line,
			event,
			current_date);
	}
	return;
} /*end world_daily_F.c*/
