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
/*  z1, z2, available_water and the decay exponentials are	*/
/*  memoized per thread (exact-key cache).			*/
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
		/* Unused parameters retained for interface compatibility: m, gamma, transmissivity. */
		(void)m;
		(void)gamma;
		(void)transmissivity;
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
	/* Cleanup note: removed unused locals didx_bot/didx_top, theta/sat_deficit, and Q. */
	double navail, nleached,nabsorbed;
	double Qtotal;
	double z1, z2;
	double	available_water,septic_depth;

	nleached = 0.0;
	Qtotal = 0.0;
	/* Cleanup note: Qtotal is retained for diagnostics and intentionally unused. */
	(void)Qtotal;
	nabsorbed = 0.0;
	navail = 0.0;

	/*------------------------------------------------------*/
	/* nitrate export only occurs when Qout > 0.0		*/ 
	/*------------------------------------------------------*/
	if (Qout > ZERO && total_nitrate > ZERO) {
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
		if (z2 > z2_N)
			z2 = z2_N;

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
		if (navail > total_nitrate) navail=total_nitrate;
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
	static _Thread_local int z_valid = 0, e_valid = 0;
	static _Thread_local double zk_n0, zk_p, zk_z2w, zk_s1, zk_s2, zc_z1, zc_z2, zc_aw;
	static _Thread_local double ek_d, ek_zN, ek_z1, ek_z2, ec_zN, ec_z1, ec_z2;
	if (z_valid && n_0 == zk_n0 && p == zk_p && z2_water == zk_z2w && s1 == zk_s1 && s2 == zk_s2) {
		z1 = zc_z1;
		z2 = zc_z2;
	}
	else {
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
		zc_aw = compute_delta_water(
			verbose_flag,
			n_0,p,z2_water,
			z2,
			z1);
		zk_n0 = n_0; zk_p = p; zk_z2w = z2_water; zk_s1 = s1; zk_s2 = s2;
		zc_z1 = z1; zc_z2 = z2; z_valid = 1;
	}
	if (N_decay_rate > 0.0) {	
		if (!(e_valid && N_decay_rate == ek_d && z2_N == ek_zN && z1 == ek_z1 && z2 == ek_z2)) {
			ec_zN = exp(-1.0 * N_decay_rate * z2_N);
			ec_z1 = exp(-1.0 * N_decay_rate * z1);
			ec_z2 = exp(-1.0 * N_decay_rate * (z2));
			ek_d = N_decay_rate; ek_zN = z2_N; ek_z1 = z1; ek_z2 = z2; e_valid = 1;
		}
		navail = total_nitrate
			/ (1.0 - ec_zN )
			* (ec_z1
			- ec_z2);

		}

	else {
			septic_depth = -1.0*N_decay_rate;
			if (z1 > septic_depth)
				navail = 0.0;
			else
				navail = total_nitrate * (z2-z1)/(z2_N -  septic_depth);
		}
	
		if (navail > total_nitrate) navail=total_nitrate;
				
	/*------------------------------------------------------*/
	/* N-leached is mass flux of soluble nitrate	*/
	/* i.e n_avail / theta * outflow			*/
	/*------------------------------------------------------*/

		available_water = zc_aw;

		
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

