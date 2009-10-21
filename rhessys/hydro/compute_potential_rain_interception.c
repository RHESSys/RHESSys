/*--------------------------------------------------------------*/
/* 								*/
/*		compute_potential_rain__interception		*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_potential_rain_interception  - computes amount 	*/
/*		rain that can be intercepted by the canopy.	*/
/*								*/
/*	SYNOPSIS						*/
/*	compute_potential_rain_interception(   			*/
/*			int	,				*/	
/*			double	,				*/
/*			struct	canopy_strata_object	*);	*/
/*								*/
/*	returns:						*/
/*	potential_interception (m) - amount of rain that can be */
/*		intercepted by the canopy.			*/
/*								*/
/*	OPTIONS							*/
/*	rain (m) - amount of rain on stratum for whole day	*/
/*	canopy_strata_object - state of canopy strata		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Computes the amount of rain that is interceptible	*/
/*	given the rain during the day and the current canopy	*/
/*	start of day storage.  Note that this interceptible	*/
/*	rain may be later dripped or evaporated if the code 	*/
/*	choses in compute_rain_stored.				*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	compute_potential_rain_interception(
											int	verbose_flag,
											double	rain,
											struct	canopy_strata_object	*stratum) 
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double	potential_interception;
	double	interception_coef;
	/*--------------------------------------------------------------*/
	/*	Compute amount potentially intercepted.			*/
	/*	m = m2PlANT / m2ground *  ( (kg  / m2 * day * m2PLANT )	*/
	/*		* ( 1 m3 H20 / 1000 kg H20 )			*/
	/*	limit incoming rain by gap_fraction			*/
	/*--------------------------------------------------------------*/
	interception_coef = 1-stratum[0].gap_fraction;
	
	if (stratum[0].defaults[0][0].epc.veg_type != NON_VEG) 
		potential_interception = min(interception_coef * rain,
			stratum[0].epv.proj_pai
			* stratum[0].defaults[0][0].specific_rain_capacity
			- stratum[0].rain_stored);
	else 
		potential_interception = min(rain, (
			stratum[0].defaults[0][0].specific_rain_capacity 
			- stratum[0].rain_stored));

	potential_interception = max(potential_interception, 0.0);

	return( potential_interception );
} /*end compute_potential_rain_interception */
