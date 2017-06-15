/*--------------------------------------------------------------*/
/* 								*/
/*		compute_return_flow				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_return_flow - computes return flow		*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_return_flow(				*/
/*			int	,				*/
/*			double, *	)			*/
/*								*/
/*	returns:						*/
/*	return_flow (m)	- amount of water lost from patch	*/
/*								*/
/*	OPTIONS							*/
/*	int verbose_flag 					*/
/*	double	z - (m) water table depth			*/
/*	struct litter_object litter (patch litter object) 	*/
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
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Sept. 29, 1997 CT					*/
/*	changed to depth of water accounting			*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double	compute_return_flow(
							int	verbose_flag,
							double	detention_store_size,
							double	sat_deficit,
							struct	litter_object	*litter )
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	return_flow;
	double  excess, add_to_litter; 
	/*--------------------------------------------------------------*/
	/*	Return flow = all water above patch surface.		*/
	/*--------------------------------------------------------------*/
	excess = max(0,-1 * sat_deficit - detention_store_size);
	return_flow = max( 0, excess );

	add_to_litter = max( (litter[0].rain_capacity - litter[0].rain_stored), 0.0);
	return_flow -= add_to_litter;
	litter[0].rain_stored += add_to_litter;

	return(return_flow);
} /*compute_return_flow*/
