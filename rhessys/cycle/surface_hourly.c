
/*--------------------------------------------------------------*/
/* 								*/
/*			surface_hourly_F			*/
/*								*/
/*	NAME							*/
/*	surface_hourly 					*/
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	void surface_hourly 				*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*	Jan 2014 Xiaoli						*/
/*	calculate the interception only, leave all evaporation in surface_daily_F.c	*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

void		surface_hourly(
							struct	world_object		*world,
							struct	basin_object		*basin,
							struct	hillslope_object	*hillslope, 
							struct	zone_object		*zone,
							struct	patch_object		*patch,
							struct 	command_line_object	*command_line,
							struct	tec_entry		*event,
							struct 	date 			current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	

	
	double	compute_hourly_litter_rain_stored(
		int,
		struct	patch_object *);
	
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	struct	litter_object	*litter;
	
	/*--------------------------------------------------------------*/
	/*	Initialize litter variables.				*/
	/*--------------------------------------------------------------*/
	
	litter = &(patch[0].litter);
	
	/*--------------------------------------------------------------*/
	/*	LITTER STORE INTERCEPTION:				*/
	/*--------------------------------------------------------------*/
	
	if ( patch[0].detention_store <= (litter[0].rain_capacity - litter[0].rain_stored) ) {
		
									
		/*--------------------------------------------------------------*/
		/*	Update rain storage ( this also updates the patch level	*/
		/*	detention store and potential_evaporation	*/
		/*--------------------------------------------------------------*/
		litter[0].rain_stored  = compute_hourly_litter_rain_stored(
			command_line[0].verbose_flag,
			patch);
						
	}

		
	return;
}/*end surface_daily_F.c*/
