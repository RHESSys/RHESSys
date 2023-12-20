/*--------------------------------------------------------------*/
/* 																*/
/*							world_daily_I						*/
/*																*/
/*	NAME														*/
/*	world_daily_I												*/
/*				 - performs cycling and output of a world		*/
/*					with parallel hillslope simulations			*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void		world_daily_I(									*/
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

void		world_daily_I(
						  long   	day,
						  struct world_object *world,
						  struct command_line_object *command_line,
						  struct tec_entry *event,
						  struct date current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	int 	compute_year_day( struct date );
	void	basin_daily_I(
		long,
		struct world_object *,
		struct basin_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	basin;
	int index;
	double	Io_array[13] = { 0.0, 1445.0, 1431.0, 1410.0, 1389.0, 1368.0,1354.0,
		1354.0, 1375.0, 1403.0, 1424.0, 1438.0, 1445.0 };
	double	declination_array[47] = { 0.0, -23.0, -22.0, -21.0, -19.0,
		-17.0, -15.0, -12.0, -9.0, -6.0, -3.0, 0.0, 3.0,
		6.0, 9.0, 12.0, 14.0, 17.0, 19.0, 21.0, 22.0,
		23.0, 23.5, 23.5, 23.0, 21.5, 20.0, 18.0, 16.0, 14.0,
		12.0, 9.0, 6.0, 3.0, 0.0, -3.0, -6.0, -9.0, -12.0,
		-15.0, -17.0, -19.0, -21.0, -22.0, -23.0, -23.5,
		-23.5 };

	/*--------------------------------------------------------------*/
	/*  Compute the solar constant.                                 */
	/*                                                              */
	/*  Total broadband solar flux (W/m2)                           */
	/*                                                              */
	/*  Look up table of values based on C version of Rhessys.      */
	/*  This really should be in simulate_basin_monthly.c but       */
	/*  since there are no other monthly processes I put it here.   */
	/*--------------------------------------------------------------*/
	world[0].Io = Io_array[current_date.month];
	/*--------------------------------------------------------------*/
	/*	Determine the year day of the world (i.e. days from start	*/
	/*		of year)												*/
	/*--------------------------------------------------------------*/
	world[0].year_day =  compute_year_day( current_date );
	/*--------------------------------------------------------------*/
	/*  Compute the solar declination.                              */
	/*                                                              */
	/*  Based on a numerical approximation to the equation:         */
	/*                                                              */
	/*      declination = 23.45 sin(0.986 Nm) degrees               */
	/*                                                              */
	/*      where Nm = number of days since March 21 assuming       */
	/*                  day and night are of equal length.          */
	/*                                                              */
	/*  Equation from Linacre, Climate Data and Resources, p. 148.  */
	/*  Approximation taken from C version of rhessys.              */
	/*	We only store cos(declination) which we need on zone level.	*/
	/*--------------------------------------------------------------*/
	index = (int) ( 1.0 + ((double)world[0].year_day)/8);
	world[0].declin = declination_array[index]*DtoR;
	world[0].cos_declin = cos(declination_array[index]*DtoR);
	world[0].sin_declin = sin(declination_array[index]*DtoR);
	/*--------------------------------------------------------------*/
	/*	Simulate over all of the basins.							*/
	/*--------------------------------------------------------------*/
	for ( basin = 0; basin < world[0].num_basin_files; basin++ ){
		basin_daily_I(	day,
			world,
			world[0].basins[basin],
			command_line,
			event,
			current_date);
	}
	return;
} /*end world_daily_I.c*/
