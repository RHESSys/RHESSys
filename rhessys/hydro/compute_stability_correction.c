/*--------------------------------------------------------------*/
/*								*/
/*	compute_stability_correction.c				*/
/*								*/
/*	NAME							*/
/*	compute_stability_correction				*/
/*								*/
/*	SYNOPSIS						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	COmpute richardson number and then stability correction	*/
/*	factor for ra for latent heat.				*/
/*								*/
/*	See :							*/
/*		p. 381- p. 383 of Oke, Boundary Layer CLimates.	*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	compute_stability_correction(
									 int verbose_flag,
									 double	zsurf,
									 double	Tsurf,
									 double	usurf,
									 double	zair,
									 double	Tair,
									 double	uair)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	ri;
	double	correction;
	double Z0, ri_cr, ri_lim;
	
	Z0 = 0.01; /* Roughness length */
	ri_cr = 0.2; /* Critical Richardson's number */
	
	/*--------------------------------------------------------------*/
	/*	Compute richardson number.				*/
	/*--------------------------------------------------------------*/
	/* Updated formulation based on DHSVM */
	/* Resistances should be divided by these correction factors */
	
	correction = 1.0;
	if (Tsurf != Tair) {
		ri = 9.8 * (Tair - Tsurf) * (zair - zsurf) / (((Tair + 273.15) + (Tsurf + 273.15)) / 2.0 * uair * uair);
		ri_lim = (Tair + 273.15) / (((Tair + 273.15) + (Tsurf + 273.15)) / 2.0 * (log((zair - zsurf) / Z0) + 5));
	
		if (ri > ri_lim)
			ri = ri_lim;
		if (ri > 0.0) /* stable case */
			correction = (1 - ri / ri_cr) * (1 - ri / ri_cr);
		else { /* unstable case */
			if (ri < -0.5)
				ri = -0.5;
			correction = sqrt(1 - 16 * ri);
			}
	}
	
	/* OLD CODE */
	/*ri = (z1-z2) * 9.8 * (t2-t1)/ ( ((t2+t1)/2) * (u2-u1) * (u2-u1) );*/
	/*--------------------------------------------------------------*/
	/*	Determine if we are in stable or unstable case.		*/
	/*--------------------------------------------------------------*/
	/*if ( ri > 0 ){
		correction = pow(1/pow(1-5*ri,2.0),0.5);
		correction = 0.2;
	}
	else{
		correction = pow(1/pow(1-16*ri,.75),0.33);
	}*/
	
	return(correction);
} /*end compute_stability_correction*/
