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
			double N_absorption_rate,
			double *transmissivity) 
			
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


	double  compute_z_final(
		int,
		double,
		double,
		double,
		double,
		double);
	double  compute_N_absorbed(
		int,
		double,
		double,
		double,
		double,
		double);
		
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int didx_bot, didx_top;
	double navail, nleached,nabsorbed;
	double theta, sat_deficit;
	double Q, Qtotal;
	double z1, z2;
	double	available_water,septic_depth;

	nleached = 0.0;
	Qtotal = 0.0;
	nabsorbed=0.0;

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
				- exp(-1.0 * N_decay_rate * (z2)));
		}
		else {
			navail = total_nitrate * (z2-z1)/z2_N;
		}
		nabsorbed=compute_N_absorbed(verbose_flag,
						z1,
						z2,
						N_absorption_rate,
						p,
						n_0); 
	/*------------------------------------------------------*/
	/* in return flow Qout/theta = 1 so			*/
	/*------------------------------------------------------*/
		if (nabsorbed > navail) {
			navail=0;
			}
		else 
		  nleached = navail-nabsorbed;
		if (nleached > navail) nleached=navail;	
	}
	
	else {
	/*------------------------------------------------------*/
	/*	now for regular subsurface flow			*/
	/*	integrate through the saturated zone		*/
	/*------------------------------------------------------*/
	z2 = compute_z_final(
			verbose_flag,
			n_0,
			p,
			z2_water,		
			0.0,
			-s2);
	z1 = compute_z_final(
			verbose_flag,
			n_0,
			p,
			z2_water,		
			0.0,
			-s1);
	if (N_decay_rate > 0.0) {	
		navail = total_nitrate
			/ (1.0 - exp(-1.0 * N_decay_rate * z2_N) )
			* (exp(-1.0 * N_decay_rate * z1)
			- exp(-1.0 * N_decay_rate * (z2)));

		}

	else {
			septic_depth = -1.0*N_decay_rate;
			if (z1 > septic_depth)
				navail = 0.0;
			else
				navail = total_nitrate * (z2-z1)/(z2_N -  septic_depth);
		}
	
				
	/*------------------------------------------------------*/
	/* N-leached is mass flux of soluble nitrate	*/
	/* i.e n_avail / theta * outflow			*/
	/*------------------------------------------------------*/

		available_water = compute_delta_water(
			verbose_flag,
			n_0,p,z2_water,
			z2,
			z1);

		
		nabsorbed=compute_N_absorbed(verbose_flag,
			z1,
			z2,
			N_absorption_rate,
			p,
			n_0); 

		if (nabsorbed > navail) {
				navail=0;
		}
		else navail = navail-nabsorbed;
		
		if (available_water > ZERO) {
			nleached = navail * Qout/available_water;
		}
		else nleached = 0.0;
	    }
	}
	if (nleached > navail) nleached=navail;
	/*------------------------------------------------------*/
	/* there may be enough flow to leach out more than 	*/
	/*	availabe nitrate, so limit export by available	*/
	/*------------------------------------------------------*/

	/*if (nleached > total_nitrate)*/
		/*nleached = total_nitrate;*/

	nleached = max(nleached, 0.0);
	
	return(nleached);
} /* end compute_N_leached */

