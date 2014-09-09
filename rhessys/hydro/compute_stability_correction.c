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
									 double	z1,
									 double	t1,
									 double	u1,
									 double	z2,
									 double	t2,
									 double	u2)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	ri;
	double	correction;
	/*--------------------------------------------------------------*/
	/*	Compute richardson number.				*/
	/*--------------------------------------------------------------*/
	ri = (z1-z2) * 9.8 * (t2-t1)/ ( ((t2+t1)/2) * (u2-u1) * (u2-u1) );
	/*--------------------------------------------------------------*/
	/*	Determine if we are in stable or unstable case.		*/
	/*--------------------------------------------------------------*/
	if ( ri > 0 ){
		correction = pow(1/pow(1-5*ri,2.0),0.5);
		correction = 0.2;
	}
	else{
		correction = pow(1/pow(1-16*ri,.75),0.33);
	}
	return(correction);
} /*end compute_stability_correction*/
