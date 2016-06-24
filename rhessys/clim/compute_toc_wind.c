/*--------------------------------------------------------------*/
/* 								*/
/*			compute_toc_wind.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_toc_wind - computes wind speed at top of canopy	*/
/*				for patch.			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_toc_wind( 	double,			*/
/*					double,			*/	
/*					double,			*/
/*					double);		*/	
/*								*/
/*	returns:						*/
/*	u_z (m/s) - wind speed at height z in stratum		*/
/*								*/
/*	OPTIONS							*/
/*	u_h (m/s) - wind speed at reference height h		*/
/*	z (m) - height at which wind speed is required.		*/
/*	h (m) - height of measurement of wind speed.		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	ONly from screen to top of canopy - not for		*/
/*	within canoppys.					*/
/*	We use a log decay from a point above the canopy to	*/
/*	the top of the canopy.					*/
/*								*/
/*	Based on results of "Hydrological Modelling of Boreal	*/
/*	Forest Ecosystems:, I. Haddeland & D. Lettenmaier,	*/
/*	Water Resources Series, Tech. Report No. 143, 		*/
/*	April 1995, U. of Washington, Dept. pf Civil and	*/
/*	Environmental Engineering.				*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

double	compute_toc_wind(
						 int	verbose_flag,
						 double	u_h,
						 double	h,
						 double canht,
						 double	z)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	u_z;
	double	d_o;
	double	z_o;
	/*--------------------------------------------------------------*/
	/*	compute winds.						*/
	/*--------------------------------------------------------------*/
	if (canht > 0.0) {
		d_o = 0.63 * canht;
		z_o = 0.1 * canht;
	}
	else {
		d_o = 0.0;
		z_o = 0.01;
	}
	/*--------------------------------------------------------------*/
	/*	make sure the screen is higher than the estimated 	*/
	/*	zero plane displacement.				*/
	/*--------------------------------------------------------------*/
	if ( h < d_o ){
		fprintf(stderr,
			"FATAL ERROR: screen height %d < zero plane of patch highest stratum %d \n", h, d_o);
		exit(EXIT_FAILURE);
	}
	u_z  = u_h * log((z-d_o)/z_o) / log((h-d_o)/z_o);
	return(u_z);
} /*compute_toc_wind*/
