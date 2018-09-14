/*--------------------------------------------------------------*/
/*								*/
/*		penman_monteith.c				*/
/*								*/
/*	penman_monteith - Penman monteith E.T. equation		*/ 
/*								*/
/*	NAME							*/
/*	penman_monteith						*/
/*								*/
/*	SYNOPSIS						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Inputs:							*/
/*								*/
/*	Tair 	(deg C)	air temperature				*/
/*	Pair	(Pa)	air pressure				*/
/*	vpd	(Pa)	vapour pressure deficit			*/
/*	Rnet	(W/m2)	net radiation				*/
/*	rs	(s/m)	surface resistance  			*/
/*	ra	(s/m)	aerodynamic resistance 			*/
/*	output_flag	0 = units kg/m2/s			*/
/*			1 = units W/m2				*/
/*			2 = units mH20/s			*/
/*								*/
/*	Internal Variables					*/
/*								*/
/*	rho	(kg/m3) 	density of air			*/
/*	CP	(K/jg/degC)	specific heat of air		*/
/*	lhvap	(J/kg)		latent heat of vapourization H20*/
/*	s	(Pa/degC)	slope of sat vpd vs T curve	*/
/*								*/
/*	Output:							*/
/*								*/
/*	et 	(kg/m2/s) 	water vapour mass flux density	*/
/*	et	(W/m2)		latent heat flux density	*/
/*	et	(m/s)		water equiv. depth flux		*/
/*								*/
/*	The output fluxes are reported over a unit area which	*/
/*	should correspond to the resistance over that unit	*/
/*	area that is input as rs and rh.  It is up to the 	*/
/*	calling program to properly scale rs and rh to reflect	*/
/*	variations in LAI or stomatal fraction.			*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Take from bbgc (Peter Thorton).				*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "phys_constants.h"
#include "rhessys.h"

double	penman_monteith(
						int	verbose_flag,
						double	Tair,
						double	Pair,
						double	vpd,
						double	Rnet,
						double	rs,
						double	ra,
						int	output_flag)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double	dt;
	double	rho;
	double	lhvap;
	double	s;
	double	t1;
	double	t2;
	double	pvs1;
	double	pvs2;
	double	e;
	double	tk;
	double	rr;
	double	rhr;
	double	gamma;
	/*--------------------------------------------------------------*/
	/*	Assign tk (Kelvins)					*/
	/*--------------------------------------------------------------*/
	tk = Tair + 273.15;
	/*--------------------------------------------------------------*/
	/*	Density of air (rho) as a fn. of air temp.		*/
	/*--------------------------------------------------------------*/
	rho = 1.292 - ( 0.00428 * Tair );
	/*--------------------------------------------------------------*/
	/*	Resistance to radiative heat transfer through air 	*/
	/*--------------------------------------------------------------*/
	/* rr = rho * CP / ( 4.0 * SBC * ( tk*tk*tk));			*/
	/*--------------------------------------------------------------*/
	/*	Combined resistance to convective and radiative heat	*/
	/*	transfer, parallel resistances: 			*/
	/*--------------------------------------------------------------*/
	/*	rhr = ( ra * rr ) / ( ra + rr ); */


	/*--------------------------------------------------------------*/
	/*	Latent heat of vapourization as a fn. of Tair.		*/
	/*--------------------------------------------------------------*/
	lhvap = 2.5023e6 - 2430.54 * Tair; /* J/kg H2O */
	/*--------------------------------------------------------------*/
	/*	Temperature offsets for slope estimates			*/
	/*--------------------------------------------------------------*/
	dt = 0.2;
	t1 = Tair + dt;
	t2 = Tair - dt;
	/*--------------------------------------------------------------*/
	/*	Saturation vapour pressures at t1 and t2(Pa)		*/
	/*--------------------------------------------------------------*/
	pvs1 = 610.7 * exp(17.38 * t1 / ( 239.0 + t1));
	pvs2 = 610.7 * exp(17.38 * t2 / ( 239.0 + t2));
	/*--------------------------------------------------------------*/
	/*	Slope of pvs vs T curve at Tair		(Pa/deg C)	*/
	/*--------------------------------------------------------------*/
	s = ( pvs1 - pvs2 ) / ( t1 - t2 );
	/*--------------------------------------------------------------*/
	/*	Calculate gamma						*/
	/*--------------------------------------------------------------*/
	gamma = CP * Pair / ( lhvap );
	/*--------------------------------------------------------------*/
	/*	Evaporation in W/m2					*/
	/*--------------------------------------------------------------*/

	e = ((s*Rnet) + (rho*CP*vpd/ra)) / (gamma*(1.0 + rs/ra) +s);

	if ( verbose_flag > 2)
		printf("%8.4f %8.4f %8.4f %8.4f %8.1f %8.1f ",s , ra, rs,
		Rnet,gamma,vpd);
	if ( verbose_flag > 2)
		printf("%8.2f %8.4f ",Rnet, vpd);
	
	if ( verbose_flag == -5) {
		printf("\n          PENMAN: s=%8.4f ra=%8.4f rs=%8.4f Rnet=%8.4f gamma=%8.4f vpd=%8.4f rho=%8.4f CP=%8.4f Tair=%lf Pair=%lf LE=%8.4f e=%8.4f",
			   s , ra, rs,
			   Rnet,gamma,vpd,
			   rho,CP,
			   Tair,Pair,
			   e,
			   e / ( lhvap * 1000 ) * 1000.0);
	}
	
	
	/*--------------------------------------------------------------*/
	/*	Perform conversion if needed and return			*/
	/*--------------------------------------------------------------*/
	if ( output_flag == 0 ){
		/*--------------------------------------------------------------*/
		/*	kgH20/m2*s = W/m2 * 1kgH20/lhvap J			*/
		/*--------------------------------------------------------------*/
		return ( e / lhvap );
	}
	else if ( output_flag == 1 ){
		/*--------------------------------------------------------------*/
		/*	W/m2 = W/m2						*/
		/*--------------------------------------------------------------*/
		return(e);
	}
	else if ( output_flag == 2 ){
		/*--------------------------------------------------------------*/
		/*	mH20/s = W/m2 * 1kgH20/lhvap J * 1m3H20/1000kGH20	*/
		/*--------------------------------------------------------------*/
		if ( verbose_flag > 2)
			printf("%8.4f",e );
		return( e / ( lhvap * 1000 ));
	}
	else{
		fprintf(stderr,"FATAL ERROR: in penman_monteith - invalid output flag");
		exit(EXIT_FAILURE);
	}
} /*end penman_monteith*/
