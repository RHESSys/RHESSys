
/*--------------------------------------------------------------*/
/* 								*/
/*			surface_hourly			*/
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
/*								*/
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
	double  surface_NO3;
	double	litter_NO3;
	
	/*--------------------------------------------------------------*/
	/*	Initialize litter variables.				*/
	/*--------------------------------------------------------------*/
	
	litter = &(patch[0].litter);
	surface_NO3 = 0;
	litter_NO3 = 0;
	/*--------------------------------------------------------------*/
	/*	LITTER STORE INTERCEPTION:				*/
	/*--------------------------------------------------------------*/
	
	
	if ( (patch[0].detention_store > (max(litter[0].rain_capacity - litter[0].rain_stored, 0.0)))
                                        && (patch[0].soil_defaults[0][0].detention_store_size >= 0.0)) {
	/* assume if det store over litter then litter is saturated */
	     	patch[0].detention_store -= (litter[0].rain_capacity - litter[0].rain_stored);	
		litter[0].rain_stored = litter[0].rain_capacity;
		surface_NO3 = patch[0].surface_NO3;
		litter_NO3 = litter[0].NO3_stored;
		litter[0].NO3_stored = litter[0].rain_stored / (litter[0].rain_stored + patch[0].detention_store) *
					(litter_NO3 + surface_NO3);
		patch[0].surface_NO3 = patch[0].detention_store / (litter[0].rain_stored + patch[0].detention_store) *
					(litter_NO3 + surface_NO3);

		}

	if ( patch[0].detention_store <= (litter[0].rain_capacity - litter[0].rain_stored) ) {
		
									
		/*--------------------------------------------------------------*/
		/*	Update rain storage ( this also updates the patch level	*/
		/*	detention store and potential_evaporation	*/
		/*--------------------------------------------------------------*/
		litter[0].rain_stored  = compute_hourly_litter_rain_stored(
			command_line[0].verbose_flag,
			patch);
		litter[0].NO3_stored += patch[0].surface_NO3;
		patch[0].surface_NO3 = 0;
	}

	
	return;
}/*end surface_hourly.c*/
