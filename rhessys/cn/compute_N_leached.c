/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_N_leached				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_N_leached				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  void compute_N_leached(int					*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	);		*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*								*/
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"


double	compute_N_leached(int verbose_flag, 
			double total_nitrate, 
			double Qout, 
			double s1, 
			double s2, 
			double m,
			double gamma,
			double n_0, 
			double p, 
			double N_decay_rate, 
			double z2_N, 
			double z2_water, 
			double mobile_N_proportion) 
			
	{ 
	/*------------------------------------------------------*/ 
	/*	Local Function Declarations.						*/ 
	/*------------------------------------------------------*/
    	double  compute_delta_water(
                int,
                double,
                double,
                double,
                double,
                double);

	double compute_transmissivity_curve(
		double,
		double,
		double,
		double);
		
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double navail, nleached;
	double theta, sat_deficit;
	double Q, Qtotal;
	double z1, z2;
	double	septic_depth;

	nleached = 0.0;
	Qtotal = 0.0;
	/*------------------------------------------------------*/
	/* nitrate export only occurs when Qout > 0.0		*/ 
	/*------------------------------------------------------*/
	if (Qout > ZERO) {
	if (s1 < 0.0) s1 = 0.0;
	if (s2 < s1) s2 = s1;
	/*------------------------------------------------------*/
	/*	first look at the case of return flow		*/
	/*	for return flow we must estimate the sat_deficit */
	/*	that would account for the flow			*/
	/*	(assuming all water leaves, so Qout/theta here */
	/*	is 1)						*/
	/*------------------------------------------------------*/
	if ((s1 == 0.0) && (s2 == 0.0)) {
		z2 = -1.0 * p * log (1 - (Qout) / (p * n_0));
		z1 = 0.0;
		if (N_decay_rate > ZERO) {	
			navail = total_nitrate
				/ (1.0 - exp(-1.0 * N_decay_rate * z2_N) )
				* (exp(-1.0 * N_decay_rate * z1)
				- exp(-1.0 * N_decay_rate * (z2)))
				* mobile_N_proportion;
		}
		else {
			navail = total_nitrate * (z2-z1)/z2_N * mobile_N_proportion;
		}
	/*------------------------------------------------------*/
	/* in return flow Qout/theta = 1 so			*/
	/*------------------------------------------------------*/
		nleached = navail;
	}
	else {
	/*------------------------------------------------------*/
	/*	now for regular subsurface flow			*/
	/*	integrate through the saturated zone		*/
	/*------------------------------------------------------*/
	for (sat_deficit = s1; sat_deficit < s2; sat_deficit += INTERVAL_SIZE) {

		/*------------------------------------------------------*/
		/*	determine actual depth at this point		*/
		/*------------------------------------------------------*/
		z1 = -1.0 * p * log ( 1 - sat_deficit / (p * n_0));
		z2 = -1.0 * p * log (1 - (min(s2,sat_deficit + INTERVAL_SIZE)) / (p * n_0));
			
		/*------------------------------------------------------*/
		/* calculate mobile nitrate at this depth 	*/
		/*------------------------------------------------------*/
		if (z1 > z2_N)
			z1 = z2_N;
		if (z2 > z2_N)
			z2 = z2_N;
		if (z1 < 0.0)
			z1 = 0.0;
		if (N_decay_rate > 0.0) {	
			navail = total_nitrate
				/ (1.0 - exp(-1.0 * N_decay_rate * z2_N) )
				* (exp(-1.0 * N_decay_rate * z1)
				- exp(-1.0 * N_decay_rate * (z2)))
				* mobile_N_proportion;
		}
		else {
			septic_depth = -1.0*N_decay_rate;
			if (z1 > septic_depth)
				navail = 0.0;
			else
				navail = total_nitrate * (z2-z1)/(z2_N -  septic_depth) * mobile_N_proportion;
		}
				
		/*------------------------------------------------------*/
		/*	calculate total water in transport zone		*/ 
		/*------------------------------------------------------*/
		theta = min(INTERVAL_SIZE, s2-sat_deficit) ; 
		/*------------------------------------------------------*/
		/*	calculte lateral subsurface flow contributions  */      
		/*	from this saturation depth			*/
		/*	note gamma is gamma per unit area, per time interval */ 
		/*------------------------------------------------------*/

		Q = gamma * compute_transmissivity_curve( m, 
				z2_water,
				sat_deficit,
				min(sat_deficit + INTERVAL_SIZE,s2)); 

		if (Q > theta) {
			Q = theta;
			}

		Qtotal += Q;
		/*------------------------------------------------------*/
		/* N-leached is mass flux of soluble nitrate	*/
		/* i.e n_avail / theta * outflow			*/
		/*------------------------------------------------------*/
		if ((theta > ZERO) && (navail > ZERO) && (Q > ZERO))
			nleached += navail * Q/theta;
	} /* end for */
		/*
		if ((Qtotal - Qout) > 0.00000001) 
			printf("\n Estimated flow is creater by %lf", Qtotal - Qout);
			*/
	} /* end else */ 
	} /* end if Qout > 0 */

	/*------------------------------------------------------*/
	/* there may be enough flow to leach out more than 	*/
	/*	availabe nitrate, so limit export by available	*/
	/*------------------------------------------------------*/

	if (nleached > total_nitrate)
		nleached = total_nitrate;

	nleached = max(nleached, 0.0);
	
	return(nleached);
} /* end compute_N_leached */

