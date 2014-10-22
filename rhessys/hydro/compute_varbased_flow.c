/*--------------------------------------------------------------*/
/* 								*/
/*		compute_varbased_flow			*/
/*								*/
/*	NAME							*/
/*	compute_varbased_flow - estimates subsurface flow	*/
/*	by assuming variance around mean saturation deficit	*/
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	compute_varbased_flow(				*/
/*				double	,			*/
/*				double	,			*/
/*				double	,			*/
/*				double	,			*/
/*				double	,			*/
/*				struct patch_object *patch)	    	*/
/*								*/
/*	returns:						*/
/*	estimate of subsurface flow from patch			*/
/*								*/
/*	OPTIONS							*/
/*	double	std - standard deviation of normal distrib	*/
/*	double gamma						*/
/*	double	m - Ksat decay parameter			*/
/*	double	z - (m) depth to the water table		*/
/*	double  D - maximum depth				*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	computes subsurface throughflow by computing 		*/
/*	transmissivity based on a normal distribution 		*/
/*	of saturation deficits (around a given mean)		*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

double	compute_varbased_flow(
				int num_soil_intervals,
				double std,
				double s1,
				double gamma,	
				double interval_size,
				double *transmissivity,
				struct patch_object *patch)
{


	/*--------------------------------------------------------------*/
	/*	Local sub	definition				*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double	flow, accum;
	double	normal[9], perc[9];
	int i;
	int didx;

	// soil deficit threshold, not the soil moisture threshold, sat_store is the soil moiture threshold as
	// percentage of the max soil moisture holding capacity
	double threshold;
	threshold = patch[0].soil_defaults[0][0].soil_depth * patch[0].soil_defaults[0][0].porosity_0 * 0.99 *
		   (1 - patch[0].soil_defaults[0][0].sat_store);
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
	for (i=1; i<9; i++)
		perc[i] = 0.1;

	
	flow = 0.0;
	if (s1 < 0.0) s1 = 0.0;	

	if (std > ZERO) {
	for (i=0; i <9; i++) {
		didx = (int) lround((s1 + normal[i]*std)/interval_size);
		if (didx > num_soil_intervals) didx = num_soil_intervals;
		/* lateral flow below the threshold is 1/3 of the original value, the multiplier is arbitary. Xiaoli */
		accum = transmissivity[didx] * 1/3;
		/* fill and spill */
		if ((patch[0].sat_deficit <= threshold) && ((s1 + normal[i]*std) <= threshold)){
		    accum=transmissivity[didx] * 3;
		}

		flow += accum * perc[i];
	}
	}
	else  {
		didx = (int) lround(s1/interval_size);
		if (didx > num_soil_intervals) didx = num_soil_intervals;

		/* default lateral flow (seepage) below the threshold is 1/3 of the original value, the multiplier is arbitary. Xiaoli */
		flow = transmissivity[didx] * 1/3; // 

		// if water level exceed moisture threshold (or sat_deficit <= soil deficit threshold)
		if (patch[0].sat_deficit <= threshold){
		    flow = transmissivity[didx] * 3; 
		}
	}

	flow = flow*gamma;

	  return(flow);
} /*compute_varbased_flow*/
