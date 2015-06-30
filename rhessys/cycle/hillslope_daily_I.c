/*--------------------------------------------------------------*/
/* 								*/
/*		hillslope_daily_I					*/
/*								*/
/*	NAME							*/
/*	hillslope_daily						*/
/*		 - performs cycling of hillslope state		*/
/*			for beginning of the  day. 		*/
/*								*/
/*	SYNOPSIS						*/
/*	void hillslope_daily_I(					*/
/*			 long	,				*/
/*			 struct world_daily_object *,		*/
/*			 struct basin_daily_object *,		*/
/*			 struct hillslope_object *,		*/
/*			 struct command_line_object *,  	*/
/*			 struct tec_entry *,			*/
/*			 struct date)				*/
/*								*/
/*	OPTIONS							*/
/*	long	day,						*/
/*	struct	world_object *world,				*/
/*	struct	basin_object *basin,				*/
/*	struct 	hillslope_object *hillslope,			*/
/*	struct 	command_line_object *command_line		*/
/*	struct 	tec_entry *event - event control file		*/
/*	struct 	date current_date - local time (?)		*/
/*								*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This routine performs simulation cycles on an identified*/
/*	basin in the hillslope.					*/
/*								*/
/*	executes zone start of day simulations			*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*  	March 4/97  C. Tague                                    */
/*      added baseflow to top_model call			*/
/*								*/
/*	Sep 15 1997 RAF						*/
/*	Removed call to topmodel and placed it with baseflow	*/
/*	in hillslope_daily_F					*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void		hillslope_daily_I(
							  long	day,
							  struct	world_object *world,
							  struct	basin_object *basin,
							  struct 	hillslope_object *hillslope,
							  struct 	command_line_object *command_line,
							  struct 	tec_entry *event,
							  struct 	date current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void zone_daily_I(
		long,
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
	int zone;
	/*--------------------------------------------------------------*/
	/* zero hillslope hydro fluxes					*/
	/*--------------------------------------------------------------*/
	hillslope[0].base_flow = 0.0;
	hillslope[0].gw.Qout = 0.0;
	hillslope[0].gw.NO3out = 0.0;
	hillslope[0].gw.NH4out = 0.0;
	hillslope[0].gw.DOCout = 0.0;
	hillslope[0].gw.DONout = 0.0;
	hillslope[0].streamflow_NO3 = 0.0;
	hillslope[0].streamflow_NH4 = 0.0;
	hillslope[0].streamflow_DOC = 0.0;
	hillslope[0].streamflow_DON = 0.0;
	/*--------------------------------------------------------------*/
	/*	We do one zone at a time for the whole day to save on		*/
	/*	allocating sub-daily arrays for each zone.  Note that we	*/
	/*	would have to do some additional work to allow hourly		*/
	/*	hillslope simulation time steps.							*/
	/*																*/
	/*	Here we purposely distinguish between daily and hourly		*/
	/*	zone level simulation since, if there were hourly processes */
	/*	at a higher object level (e.g. hillslope) we would have		*/
	/*	to slip it in after the zone_daily simulation.  Note that	*/
	/*	it may seem more reasonable to simulate all of the daily	*/
	/*	processes for all objects of a level; before switching to	*/
	/*	hourly processes for objects below it; however this woukld 	*/
	/*	mean storing all of the (numerous) daily parameters .		*/
	/*--------------------------------------------------------------*/
	#pragma omp parallel for
	for ( zone=0 ; zone<hillslope[0].num_zones; zone++ ){
		zone_daily_I( 	day,
			world,
			basin,
			hillslope,
			hillslope[0].zones[zone],
			command_line,
			event,
			current_date );
	}
	return;
} /*end hillslopee_daily_I.c*/
