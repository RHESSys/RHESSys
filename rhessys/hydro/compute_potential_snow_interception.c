/*--------------------------------------------------------------*/
/* 								*/
/*		compute_potential_snow_interception		*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_potential_snow__interception  - computes amount	*/
/*		snow that can be intercepted by the canopy.	*/
/*								*/
/*	SYNOPSIS						*/
/*	compute_potential_interception(   			*/
/*			int	,				*/	
/*			double	,				*/
/*			struct	canopy_strata_object	*);	*/
/*								*/
/*	returns:						*/
/*	potential_interceptione (m) - amount of snow that can be*/
/*		intercepted by the canopy.			*/
/*								*/
/*	OPTIONS							*/
/*	snow (m) - amount of snow on stratum for whole day	*/
/*	canopy_strata_object - state of canopy strata		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Computes the amount of snow that is interceptible	*/
/*	given the snow during the day and the current canopy	*/
/*	start of day storage.  Note that this interceptible	*/
/*	snow may be later dripped or evaporated if the code 	*/
/*	choses in compute_snow_stored.				*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/* Sept. 14	C. Tague					*/
/*	- include a snow  interception coefficient		*/
/*	(different from rain interception coeff)		*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

double	compute_potential_snow_interception(
											int	verbose_flag,
											double	snow,
											double Tair,
											struct	canopy_strata_object	*stratum)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double	potential_interception;
	double interception_coef;
	double leaf_area_ratio, Imax;
	/*--------------------------------------------------------------*/
	/*	Compute amount potentially intercepted.			*/
	/*								*/
	/*	Defined as the minimum of the amount the stratum can cat*/
	/*	and the amount that fell from the stratum (or sky) above*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Compute amount potentially intercepted.			*/
	/*	m = m2PlANT / m2ground *  ( (kg  / m2 * day * m2PLANT )	*/
	/*		* ( 1 m3 H20 / 1000 kg H20 )			*/
	/*	limit rain interception by gap_fraction			*/
	/*--------------------------------------------------------------*/
	/* 60% interception efficiency for various conifers per Storck 2002 */
	/* and as applied in Andreadis 2009 */
	/*interception_coef = 0.6 * (1.0 - stratum[0].gap_fraction);*/
	interception_coef = 1.0 - stratum[0].gap_fraction;
	/*--------------------------------------------------------------*/
	/* Leaf area ratio from Andreadis 2009 to account for reduction in */
	/* holding capacity on narrow surfaces under cold temps */ 
	/*if (Tair > -1.0) leaf_area_ratio = 4.0;
	else {
		if (Tair > -3.0) leaf_area_ratio = 1.5 * Tair + 5.5;
		else leaf_area_ratio = 1.0;
		}*/
	leaf_area_ratio = 1.0;
	/*--------------------------------------------------------------*/
	if (stratum[0].defaults[0][0].epc.veg_type != NON_VEG) 
		potential_interception = min(interception_coef * snow,
			leaf_area_ratio * stratum[0].epv.all_pai
			* stratum[0].defaults[0][0].specific_snow_capacity
			- stratum[0].snow_stored);
	else
		potential_interception = min (snow,
			(stratum[0].defaults[0][0].specific_snow_capacity
			- stratum[0].snow_stored));
	
	/* Liston & Elder */
	/*Imax = 4.4 * stratum[0].epv.all_pai / 1000; /* converted from kg/m2 to m */
	/*potential_interception = 0.7 * (Imax - stratum[0].snow_stored) * (1.0 - exp(-snow/Imax));*/
	
		
	potential_interception = max(potential_interception, 0.0);
	return( potential_interception );
} /*end compute_potential_snow_interception */
