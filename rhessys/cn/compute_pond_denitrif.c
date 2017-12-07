/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_pond_denitrif				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_pond_denitrif				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  double compute_pond_denitrif(				*/
/*                                                              */
/*			double					*/
/* 			struct  cdayflux_patch_object *,         */
/*			struct  litter_c_object   *               */
/*                      struct  litter_n_object   *               */
/*                              )                               */
/*  OPTIONS                                                     */
/*                                                              */
/*                                                              */
/*  DESCRIPTION                                                 */
/*	as with regular dentrification this builds on		*/
/*	but uses pond water inputs				*/
/*	and assumes that water_scale = 1 (saturated)		*/
/*	and uses pond litter for carbon availability 	*/
/*								*/
/*	Parton et al. 1996. Generalized model of N2 and N20 	*/
/*	production, Global Biogeochemical cycles, 10:3		*/
/*	401-412							*/
/*                                                              */
/*								*/
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include "rhessys.h"
#include <stdio.h>
#include <math.h>

double  compute_pond_denitrif(  double nitrate,
 					struct  cdayflux_patch_struct *cdf,     
					struct  litter_c_object   *cs_litter,
					struct  litter_n_object   *ns_litter)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok,i;
	double denitrify;
	double water_scalar;
	double fnitrate, fCO2;
	double hr, nitrate_ratio;
	double totallitterC, totallitterN;


	/* assume saturated */
	water_scalar = 1.0;

		/*--------------------------------------------------------------*/
		/*	maximum denitrfication (kg/ha) based on available	*/
		/*		N03							*/
		/*--------------------------------------------------------------*/
		totallitterC = cs_litter->litr1c+cs_litter->litr2c+cs_litter->litr3c+cs_litter->litr4c;
		totallitterN = ns_litter->litr1n+ns_litter->litr2n+ns_litter->litr3n+ns_litter->litr4n;

		nitrate_ratio = (nitrate)
			/ (totallitterC+totallitterN) * 1e6;
		fnitrate = atan(PI*0.002*(nitrate_ratio - 180)) * 0.004 / PI + 0.0011;

		/*--------------------------------------------------------------*/
		/*	maximum denitrfication (kg/ha) based on available	*/
		/*	carbon substrate - estimated from heter. respiration    */
		/* using litter here but removing the first labile store 	*/
		/* because it is highly variable and not included in original NGAS formulation 				*/
		/*--------------------------------------------------------------*/
		hr = (cdf->litr2c_hr + cdf->litr3c_hr + cdf->litr4c_hr);
		if (hr > ZERO) 
			fCO2 = 0.0024 / (1+ 200.0/exp(0.35*hr*10000.0)) - 0.00001;
			
		else
			fCO2 = 0.0;
		/*--------------------------------------------------------------*/
		/*	estimate denitrification				*/
		/*--------------------------------------------------------------*/
		denitrify = min(fCO2, fnitrate) * water_scalar;
	/*--------------------------------------------------------------*/
	/*	update state and flux variables				*/
	/*--------------------------------------------------------------*/
	denitrify = min(denitrify, nitrate);
	denitrify = max(0.0, denitrify);
	return(denitrify);
} /* end compute_pond_denitrif */
