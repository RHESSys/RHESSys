/*--------------------------------------------------------------*/
/* 								*/
/*		compute_delta_water				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_delta_water - computes amount of water lost	*/
/*		or gained due to a change in the water table	*/
/*		height						*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_delta_water(				*/
/*			int	,				*/
/*			double	,				*/
/*			double	,				*/
/*			double	,				*/
/*			double	)				*/
/*								*/
/*	returns:						*/
/*	delta_water (m)	- amout of water lost/gained		*/
/*								*/
/*	OPTIONS							*/
/*	int verbose_flag 					*/
/*	double	n_0 - porosityat z=0				*/
/*	double	p - POROSITY scaling parameter			*/
/*	double	z_initial - (m) initial water table depth	*/
/*	double	z_final - (m) final water table depth		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This routine can be used to estimate the amount		*/
/*	of water lost or gained by a change in the water	*/
/*	table depth.  It is neccessary where pososity is not	*/
/*	fixed with depth.					*/
/*								*/
/*	The user supplied porosity is used scaled by a		*/
/*	depth dependant function.				*/
/*								*/
/*	Refer to compute_z_final for full details.		*/
/*	Both this routine and compute_z_final should be		*/
/*	theoretically consistent.				*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double	compute_delta_water(
							int	verbose_flag,
							double	n_0,
							double	p,
							double	soil_depth,
							double	z_initial,
							double	z_final)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	delta_water;
	
	/*--------------------------------------------------------------*/
	/*	User defined function relating water table depth change */
	/*	to water lost or gained.				*/
	/*								*/
	/*	A delta_water > 0 means water added to the water table.	*/
	/*	Given that the POROSITY parameter p is a 		*/
	/*	depth decay parameter for porosity:			*/
	/*								*/
	/*	n(z) = n_0 * exp( - z / p )				*/
	/*								*/
	/*	Then the addition in water content due to a change in z	*/
	/*	is the integral of n(z) from z_final to z_initial:	*/
	/*								*/
	/*	delta_water = n_0 * -p *				*/
	/*		(exp(-z_final/p) - exp(-z_initial/p));		*/
	/*								*/
	/*	However, the case where either z is negative (i.e.	*/
	/*	above the land surface) must be handled first.		*/
	/*--------------------------------------------------------------*/
	delta_water = min(z_initial,0) - min(z_final,0);
	
	/*--------------------------------------------------------------*/
	/*	Now we look at below the land surface.			*/
	/*--------------------------------------------------------------*/
	z_final = min(max(z_final,0),soil_depth);
	z_initial = max(z_initial,0);
	if ( ( p != 0 ) && (p < 999.0) ){
		delta_water += n_0 * p *
			(exp(-z_final/p) - exp(-z_initial/p));
	}
	else{
		delta_water += n_0 * (z_initial - z_final );
	}
	
	
	return(delta_water);
	
} /*compute_delta_water*/
