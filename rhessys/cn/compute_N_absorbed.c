/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_N_absorbed				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_N_absorbed				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  void compute_N_absorbed(int					*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double);		*/
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

#define  PARTICLE_DENSITY	2.65	/* soil particle density g/cm3 (Dingman) */
double	compute_N_absorbed(int verbose_flag, 
			double z1, 
			double z2, 
			double N_absorption_rate, 
			double p, 
			double n_0) 
			
	{ 
	/*------------------------------------------------------*/ 
	/*	Local Function Declarations.						*/ 
	/*------------------------------------------------------*/
   		
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double nabsorbed;       /*kg*/
	double bulk_density;
	
	bulk_density = PARTICLE_DENSITY * (1.0 - n_0) * 1000;
	nabsorbed=n_0*(z2-z1)*N_absorption_rate*bulk_density;
	
	nabsorbed=max(nabsorbed, 0.0);
	
	return(nabsorbed);
} /* end compute_N_absorbed */

