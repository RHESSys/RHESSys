/*--------------------------------------------------------------*/
/* 								*/
/*		compute_z_final					*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_z_final - computes new water table height	*/
/*	due to an gain/loss of water.				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_delta_water(				*/
/*			int	,				*/
/*			double	,				*/
/*			double	,				*/
/*			double	,				*/
/*			double	)				*/
/*								*/
/*	returns:						*/
/*	delta_z (m)	- amout of water lost/gained		*/
/*								*/
/*	OPTIONS							*/
/*	int verbose_flag 					*/
/*	double	p_0 - porosity at z=0				*/
/*	double	p - porosity scaling parameter			*/
/*	double	z_initial - (m) initial water table depth	*/
/*	double	delta_water - (m) final water table depth	*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This routine estimates the new water table height due	*/
/*	to a gain or loss of water when porosity is not		*/
/*	fixed with depth.  It must be the inverse of		*/
/*	compute_delta_water.c for stability.			*/	
/*								*/
/*	Note that the current function relating porosity to 	*/
/*	depth is a negative exponential with the p parameter 	*/
/*	governing its decay.  The math works out so that 	*/
/*	the minmum delta_water is:				*/
/*								*/
/*	delta_water = p_o * p * exp(-z_initial/p)		*/
/*								*/
/*	For values past this a fatal error is reported.		*/
/*								*/
/*	The porosity scaling parameter p may be interpreted	*/
/*	as follows:						*/
/*								*/
/*	i) Assuming the pore size distribution is constant	*/
/*		with depth.					*/
/*								*/
/*	Ksat(z) = n(z) * t(z) * const				*/
/*								*/
/*	where t(z) is the change in tortuosity with depth	*/
/*	and the constant is a function of the pore size		*/
/*	distribution and packing characteristics of grains.	*/
/*								*/
/*	ii) Assuming that we can represent t(z) and Ksat(z) 	*/
/*	by exponential decay functions:				*/
/*								*/
/*	Ksat(z) = K(z=0) exp(-z/m)				*/
/*	T(z) = T(z=0) exp(-z/q)					*/
/*								*/
/*	then							*/
/*								*/
/*	n(z) = [K(z=0)/(const * T(z=0)]*[exp(-z/m)/exp(-z/q)]	*/
/*	     = p_0 exp(-[(1/m) - (1/q)]z)			*/
/*	     = p_0 exp(z/p)					*/
/*								*/
/*	where p = 1 / [(1/m) - (1/q)] 				*/
/*								*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double	compute_z_final(
						int	verbose_flag,
						double	p_0,
						double	p,
						double	soil_depth,
						double	z_initial,
						double	delta_water)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration									*/
	/*--------------------------------------------------------------*/
	double	compute_delta_water(
		int,
		double,
		double,
		double,
		double,
		double);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	double	arguement;
	double	delta_water_surf;
	double	z_final;
	
	/*--------------------------------------------------------------*/
	/*	User defined function relating water table gain/loss    */
	/*	and current water table depth to new water table depth	*/
	/*	(delta_water positive ==> addition of water to water 	*/
	/*					table )			*/
	/*								*/
	/*	Given that the OPMODEL m parameter is proportional to	*/
	/*	the depth to the impermeable layer we use the TOPMODEL	*/
	/*	depth decay parameter for porosity in the soil column:	*/
	/*								*/
	/*	n(z) = p_0 * exp( - z / p )	if z<=0			*/
	/*	     = 1			if z>0			*/
	/*								*/
	/*	Then the addition in water content due to a change in z	*/
	/*	is the integral of n(z) from z_final to z_initial:	*/
	/*								*/
	/*	i) Where both z_initial and z_final >= 0:		*/
	/*								*/
	/*	delta_water = p_0 * +p * 				*/
	/*		(exp(-z_final/p) - exp(-z_initial/p))		*/
	/*								*/
	/*	Solvin for z_final gives:				*/
	/*								*/
	/*	exp(-z_final/p) = [ delta_water / (-p*p_0) ] +	*/
	/*				exp(-z_initial/p)		*/
	/*								*/
	/*	z_final = -p * ln [ exp(-z_initial/p) +			*/
	/*				delta_water / (p*p_0) ]	*/
	/*								*/
	/*	Note that since we dont know z_final a priori 		*/
	/*	recognizing this case is tricky.  To do so we first	*/
	/*	compute delta_water(z_final=0) and see if this is	*/
	/*	less than the existing delta_water - if not then 	*/
	/*	we have case iv).					*/
	/*								*/
	/*	ii) Where z_initial <= 0 and z_final <= 0:		*/
	/*								*/
	/*	delta_water = z_initial - z_final			*/
	/*	z_final = z_initial - delta_water			*/
	/*								*/
	/*	iii) Where z_initial <0 and delta_water < -z_initial:	*/
	/*								*/
	/*	set z_initial'=0 and delta_water'=delta_water-z_initial	*/
	/*	and apply case i)					*/
	/*								*/
	/*	iv) Where z_initial<0 and z_final >0:			*/
	/*								*/
	/*	estimate delta_water'=delta_water-delta_water(z_final=0)*/
	/*	estimate z_final = delta_water'				*/
	/*								*/
	/*	where delta_water(z_final=0) is the amount of water 	*/
	/*	needed to reach the surface.				*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Ensure that p and p_0 is not zero 			*/
	/*--------------------------------------------------------------*/
	p = max(p,0.00000001);
	p_0 = max(p_0,0.00000001);
	
	/*--------------------------------------------------------------*/
	/*	Case ii  water table at or above surface + recharge	*/
	/*--------------------------------------------------------------*/
	if ( (z_initial<=0) && (delta_water>=0) ){
		z_final = z_initial - delta_water;
	}
	/*--------------------------------------------------------------*/
	/*	Case iii water table at or above surface + discharge	*/
	/*--------------------------------------------------------------*/
	else if ( (z_initial<=0) && (delta_water<0) ){
		z_final = z_initial - delta_water;
		/*--------------------------------------------------------------*/
		/*	The drainage brings z_final below the surface	*/
		/*--------------------------------------------------------------*/
		if ( z_final > 0 ){
			/*--------------------------------------------------------------*/
			/*	Make sure we dont drain more than there is.		*/
			/*--------------------------------------------------------------*/
			delta_water = -1 * z_final;
			if (p < 999.9) {
				if ( z_final < (p*p_0) ){
					z_final = -1 * p *
						log( 1 +  delta_water / ( p * p_0) );
				}
				else {
					z_final = soil_depth;
				}
			}
			else
				{if ((-delta_water/p_0)>soil_depth) // should compare with soil_depth here
					z_final=soil_depth;
				else
					z_final =  (0 - delta_water/p_0); // should be 0 because start from surface, z=0
				} 
		}
	}
	/*--------------------------------------------------------------*/
	/*	Determine if we will be in case i or iv			*/
	/*	i.e. will the recharge cause z_final to be above surface*/
	/*--------------------------------------------------------------*/
	else{
		/*--------------------------------------------------------------*/
		/*	Compute water needed to get to surface		*/
		/*--------------------------------------------------------------*/
		delta_water_surf =
			compute_delta_water(
			verbose_flag,
			p_0,
			p,
			soil_depth,
			z_initial,
			0);
		
		/*--------------------------------------------------------------*/
		/*	Determine if we will get to the surface.	*/
		/*--------------------------------------------------------------*/
		if ( delta_water < delta_water_surf ){
			/*--------------------------------------------------------------*/
			/*	Case i - we dont get to surface				*/
			/*	make sure if delta_water is negative we dont blow up	*/
			/*	the logarithm						*/
			/*--------------------------------------------------------------*/
			arguement = ( exp( -1 * z_initial/p) + delta_water / (p*p_0));
			if ( arguement > 0 ){
				z_final = -1 * p * log(arguement);
			}
			else{
				z_final = soil_depth;
			}
		}
		else{
			/*--------------------------------------------------------------*/
			/*	Case iv - we get to surface .				*/
			/*--------------------------------------------------------------*/
			z_final = delta_water_surf  - delta_water;
		}
	}
	
	/* CAN GO SLIGHTLY BELOW SOIL DEPTH AND CAUSING INSTABILITY, SO CAPPING */
	z_final = min(z_final,soil_depth);
	
	return(z_final);
} /*compute_z_final*/

