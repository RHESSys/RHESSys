
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
    /* Need to check to see if patch is SCM - some SCM patches may have "zero"  potential soil detention store, but can still store surface water */
	if(command_line[0].scm_flag == 1){
		if ( ( (patch[0].detention_store > (max(litter[0].rain_capacity - litter[0].rain_stored, 0.0))) && (patch[0].soil_defaults[0][0].detention_store_size > 0.0)) || ( (patch[0].detention_store > (max(litter[0].rain_capacity - litter[0].rain_stored, 0.0))) && (patch[0].scm_defaults[0][0].ID != 0))) {
			det_on_litter_flag = 1;
		}
		else {
			det_on_litter_flag = 0;
		}
	} 
	
	/* If not in SCM mode, call routines when there is detention store > leaf store AND there is potential soil detention storage */
	else {
		if ( ( (patch[0].detention_store > (max(litter[0].rain_capacity - litter[0].rain_stored, 0.0)))	&& (patch[0].soil_defaults[0][0].detention_store_size > 0.0) )) {
			det_on_litter_flag = 1;
		}
		else {
			det_on_litter_flag = 0;
		}
	}
	
	/* Case where detention store sits on top of litter - routines */
	if ( det_on_litter_flag == 1) {
	/* assume if det store over litter then litter is saturated */
		litter[0].rain_stored = litter[0].rain_capacity;
		patch[0].detention_store -= (litter[0].rain_capacity - litter[0].rain_stored);	
	}

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
