/*--------------------------------------------------------------*/
/*                                                              */ 
/*		update_rooting_depth				*/
/*                                                              */
/*  NAME                                                        */
/*		update_rooting_depth				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* int update_rooting_depth(				*/ 
/*			struct rootzone_struct *rootzone,		*/
/*			double rootc, 				*/
/*		  double root_growth_direction,			*/
/*		   double root_distrib_parm,			*/
/*		   double soil _depth)				*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	changes rooting depth based on root carbon		*/
/*	following eqn (11) in Arora and Boer, 			*/
/*	A representation of variable root distribution		*/
/*	in dynamic vegetation models, Earth Interactions	*/
/*	V7:6, 2003						*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

int update_rooting_depth( struct rooting_zone_object *rootzone,
						   double rootc,
						   double root_growth_direction,
						   double root_distrib_parm,
						   double max_root_depth,
						   double soil_depth,
						   double stem_density)
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	double biomass_carbon_ratio;

	biomass_carbon_ratio = 2.0;
	if (stem_density > ZERO) {
	rootzone->depth = 3.0 * pow((biomass_carbon_ratio*rootc/stem_density), root_growth_direction) 
				/ root_distrib_parm;
	}
	else {
	rootzone->depth = 3.0 * pow((biomass_carbon_ratio*rootc), root_growth_direction) 
				/ root_distrib_parm;
	}

	rootzone->depth = min(rootzone->depth, max_root_depth);
	rootzone->depth = min(rootzone->depth, soil_depth-0.0001);
	rootzone->depth = max(rootzone->depth, 0.0);

	return (!ok);
} /* end update_rooting_depth */


