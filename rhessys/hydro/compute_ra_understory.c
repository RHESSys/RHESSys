/*--------------------------------------------------------------*/
/* 								*/
/*		compute_ra_understory.c		*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_ra_understory - aerodynamic cond	*/
/*		for ovsretory layers in patch.			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_ra_understory(		*/
/*						int,		*/
/*						double,		*/
/*						double,		*/
/*						double,		*/
/*						double,		*/
/*						*double);	*/
/*								*/
/*	returns:						*/
/*	ra - aerodynamic resistance of stratum			*/
/*								*/
/*	OPTIONS							*/
/*	int - verbose flag					*/
/*	u - wind speed measured in representative stratum at 	*/
/*			height z above ground surface. (m/s)	*/
/*	cn - wind attenuation coefficient			*/
/*	h - average height of canopy stratum (m)		*/
/*	z_u - average height of next lower layer (m)		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Computes aerodynamic resistance of what should be	*/
/*	the undersotry layer in a patch. 			*/
/*        The computation   					*/
/*      assumes a log decay profile to the top of the           */
/*      canopy followed by an exponential decay profile to      */
/*      the current layer height.                               */
/*								*/
/*	No attempt has been made to add stability corrections	*/
/*		or to make use of knowledge of other strata or	*/
/*		or to take into account snowpack on/over strata	*/
/*								*/
/*	Reference: Heddeland, I and Lettenmaier, D. (1995)	*/
/*		   "Hydrological Modelling of Boreal Forest	*/
/*			Ecosystems" Water Resource Series #149,	*/
/*			Dept. of Civil Engineering, U of	*/
/*			Washington (pg. 38)			*/
/*								*/
/*	d and zo formula Taken from Xuewen Wang's version 	*/
/*	of rhessys C code.  					*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	A fatal error condition arises if the screen heigh	*/
/*	is lower than the zero plane height.  To avoid this the	*/
/*	calling routine should use data acquired at a higher	*/
/*	screen height or adjusted to a higher screen height.	*/ 
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

double	compute_ra_understory(
							  int	verbose_flag,
							  double  cn,
							  double	*u,
							  double	h_o,
							  double  h_u,
							  double  *ga)
							  
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	d_o, d_u;
	double	zo_o, zo_u;
	double	ra;
	double	ra_u;
	/*--------------------------------------------------------------*/
	/* set current resistance at the top of the layer		*/
	/*--------------------------------------------------------------*/
	ra = 1/(*ga);
	/*--------------------------------------------------------------*/
	/* comput the zero plane displacement d (m)			*/
	/*								*/
	/*	Equation supplied by Xuewen Wang rhessys C code.	*/
	/*--------------------------------------------------------------*/
	d_o =  pow(10.0, (0.979 * log10(h_o+0.001) - 0.154));
	d_u =  max(0.01*h_o, pow(10.0, (0.979 * log10(h_u+001) - 0.154)));
	/*--------------------------------------------------------------*/
	/*	Compute the roughness length zo (m)			*/
	/*								*/
	/*	Equation supplied by Xuewen Wang rhessys C code.	*/
	/*	Richard Fernandes:  This should change with snow pack .	*/
	/*--------------------------------------------------------------*/
	zo_o = pow(10.0, (0.997 * log10(h_o+0.001) - 0.883));
	zo_u = max(0.01*h_o, pow(10.0, (0.997 * log10(h_u+0.001) - 0.883)));
	
	printf("\nUNDER:cn=%lf u_toc=%lf h_o=%lf h_u=%lf ga_toc=%lf ra_toc=%lf",cn,*u,h_o,h_u,*ga,ra);
	
	if ( h_o < d_o ){
		fprintf(stderr,
			"FATAL ERROR: screen height < zero plane stratum\n");
		exit(EXIT_FAILURE);
	}
	/*--------------------------------------------------------------*/
	/*	Compute the resistance to momentum transfer from a sourc*/
	/*	at the reference height but still in the the canopy	*/
	/*	stratum. ga (m/s)					*/
	/*--------------------------------------------------------------*/
	ra_u = ra + log ( (h_o-d_o)/zo_o ) * h_o * exp(-1*cn)
		* ( exp(-1*cn*(d_u+zo_u)/h_o) - exp(-1*cn*(d_o+zo_o)/h_o))
		/ ( *u * 0.41 * 0.41 * cn * (h_o - d_o));
	/*--------------------------------------------------------------*/
	/*	compute exponential decay of wind throught the canopy  */
	/*--------------------------------------------------------------*/
	*u = max((*u * exp(cn*(max(h_u, 0.1*h_o)/h_o - 1))), 0.0); /* FIXED MISSING PARENTHESES */
	printf(" u_bot=%lf ra_bot1=%lf",*u,ra_u);
	/*--------------------------------------------------------------*/
	/*	if, this canopy below extends to 0.1*ho of the surface	*/
	/* 	include a logarithmic profile componenet of the near	*/
	/*	surface resistance					*/
	/*--------------------------------------------------------------*/
	if ( h_u <= 0.1*h_o )
		ra_u += pow(log( (0.1*h_o)/ max(zo_u, 0.01 )),2.0) / (*u * 0.41*0.41);
	/*--------------------------------------------------------------*/
	/*	update conductance below this patch			*/
	/*--------------------------------------------------------------*/
	*ga = 1/ra_u;
	printf(" ra_bot2=%lf ga_bot=%lf",ra_u,*ga);
	return(ra);
} /*compute_ra_understory*/
