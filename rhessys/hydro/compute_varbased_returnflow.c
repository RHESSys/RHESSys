/*--------------------------------------------------------------*/
/* 								*/
/*		compute_varbased_returnflow				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_varbased_returnflow - computes return flow		*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_varbased_returnflow(				*/
/*			double	,				*/
/*			double	,				*/
/*			double	)				*/
/*								*/
/*	returns:						*/
/*	varbased_returnflow (m)	- amount of water lost from patch	*/
/*								*/
/*	OPTIONS							*/
/*	double	std standard deviation of saturation deficit - (m) water 			*/
/*	double	unsat_storage - (m) water 			*/
/*	double	sat_deficit - (m) water 			*/
/*	struct	litter (litter object)				*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This routine estimates the water leaving a patch 	*/
/*	as saturation excess flow.  It is not currently		*/
/*	linked to neighbouring patches or the hillslope.	*/
/*								*/
/*	At the moment the return flow is all water above the 	*/
/*	soil surface but we could make use of a pond size 	*/
/*	parameter later to hold water.				*/
/*	in the varbased version we include a normal distribution */
/*	around mean saturation deficit to account for variance 	*/
/*	in water holding capacity within a patch and then 	*/
/*	extent to which this can impact non-linear return	*/
/*	flow production						*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Sept. 29, 1997 CT					*/
/*	changed to depth of water accounting			*/
/*	added a filling up of litter storage prior to rf	*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double	compute_varbased_returnflow( double std, 
				     double unsat_storage,
					double	sat_deficit ,
					struct	litter_object	*litter)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	add_to_litter, return_flow,sd;
	double	normal[9], perc[9];
	int i;

	normal[0] = 0;
	normal[1] = 0.253;
	normal[2] = 0.524;
	normal[3] = 0.842;
	normal[4] = 1.283;
	normal[5] = -0.253;
	normal[6] = -0.524;
	normal[7] = -0.842;
	normal[8] = -1.283;

	perc[0] = 0.2;
	for (i=1; i<8; i++)
		perc[i] = 0.1;

	return_flow = 0.0;
	/*--------------------------------------------------------------*/
	/*	Return flow = all water above patch surface.		*/
	/*--------------------------------------------------------------*/
	if (std > ZERO) {
	for (i=0; i <9; i++) {
	   	sd = (sat_deficit - unsat_storage) 
			+ std*normal[i];
		if (sd < -1.0*ZERO)
			return_flow +=  -1 * sd * perc[i];
	}
	}
	else return_flow = max(-1*(sat_deficit-unsat_storage),0.0);

	add_to_litter = max( (litter[0].rain_capacity - litter[0].rain_stored), 0.0);
	add_to_litter = min(add_to_litter, return_flow);
	add_to_litter = 0.0;
	return_flow -= add_to_litter;
	litter[0].rain_stored += add_to_litter;

	return(return_flow);
} /*compute_varbased_returnflow*/
