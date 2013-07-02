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
/* 	following the model (and parameters) of 		*/
/*	Kothawala and Moore, Canadian J. For Research, 2009	*/
/*	39:2381-3290						*/
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

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

