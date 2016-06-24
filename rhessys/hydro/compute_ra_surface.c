/*--------------------------------------------------------------*/
/* 								*/
/*		compute_ra_surface.c		*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_ra_surface - aerodynamic cond	*/
/*	for surface patch layer			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_ra_surface(		*/
/*						int,		*/
/*						*double,	*/
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
/*	z - screen height					*/
/*	h - average height of canopy stratum (m)		*/
/*	z_u - average height of next lower layer (m)		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Computes aerodynamic resistance of what should be	*/
/*	the surface layer in a patch. 			*/
/*        The computation   					*/
/*	a logarithmic profile is assumed here			*/
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
#include <math.h>
#include "rhessys.h"

double	compute_ra_surface(
						   int	verbose_flag,
						   double  cn,
						   double	*u,
						   double	h_o,
						   double  h_u,
						   double  *ga)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration			*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	zo_u;
	double	ra;
	double	ra_u;
	/*--------------------------------------------------------------*/
	/* set current resistance at the top of the layer		*/
	/*--------------------------------------------------------------*/
	ra = 1.0/(*ga);
	printf("\nSURF:cn=%lf u_top=%lf h_o=%lf h_u=%lf ga_top=%lf ra_top=%lf",cn,*u,h_o,h_u,*ga,ra);

	/*--------------------------------------------------------------*/
	/*	Compute the roughness length zo (m)			*/
	/*								*/
	/*	Equation supplied by Xuewen Wang rhessys C code.	*/
	/*	Richard Fernandes:  This should change with snow pack .	*/
	/*--------------------------------------------------------------*/
	zo_u =  pow(10.0, (0.997 * log10(h_u+0.001) - 0.883));
	/*--------------------------------------------------------------*/
	/*	compute exponential decay of wind throught the canopy  */
	/*--------------------------------------------------------------*/
	*u = max((*u * exp(cn*(max(h_u, 0.1*h_o)/h_o - 1))), 0.0);	
	/*--------------------------------------------------------------*/
	/*	this canopy below is below  0.1*ho of the surface	*/
	/* 	therefore resistence is  a logarithmic profile 		*/
	/*--------------------------------------------------------------*/
	ra_u = pow(log( (h_o)/ max(zo_u, 0.01 ) )/0.41,2.0) / *u;
	/*--------------------------------------------------------------*/
	/*	update conductance below this patch			*/
	/*--------------------------------------------------------------*/
	*ga = 1/ra_u;
	printf(" u_bot=%lf ga_bot=%lf ra_bot=%lf",*u,*ga,ra_u);
	return(ra);
} /*compute_ra_surface*/ 
