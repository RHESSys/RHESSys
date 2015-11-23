/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_growingseason_index				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_growingseason_index				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  void compute_growingseason_index(    			*/
/*			struct zone *,		*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*                                                              */
/*  DESCRIPTION                                                 */
/*  computation of growing season index to drive phenology     */
/*  leaf onset and offset timing				*/
/*  based on Jolly et al., 2005, Global Change Biology 		*/
/*                                                              */
/*								*/
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

double	compute_growingseason_index(struct zone_object *zone,
				struct epconst_struct epc) 
			
	{ 
		
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

	double gsi, itmin, ivpd, idayl;

	itmin = 0.0;
	if (zone[0].metv.tmin_ravg >= epc.gs_tmax)
		itmin = 1.0;
	else
		itmin = (zone[0].metv.tmin_ravg - epc.gs_tmin)/(epc.gs_trange);
	
	ivpd = 0.0;
	if (zone[0].metv.vpd_ravg <= epc.gs_vpd_min)
		ivpd = 1.0;
	else
		ivpd = 1.0 - ((zone[0].metv.vpd_ravg - epc.gs_vpd_min)/(epc.gs_vpd_range));
	
	idayl = 0.0;
	if (zone[0].metv.dayl_ravg >= epc.gs_dayl_max)
		idayl = 1.0;
	else
		idayl = (zone[0].metv.dayl_ravg - epc.gs_dayl_min)/(epc.gs_dayl_range);
	
	itmin = MAX(itmin, 0.0);
	ivpd = MAX(ivpd, 0.0);
	idayl = MAX(idayl, 0.0);
	
	gsi = idayl*itmin*ivpd;
	/* printf("\n tmin %lf %lf vpd %lf %lf dayl %lf %lf gsi %lf", itmin,
	zone[0].metv.tmin_ravg, ivpd, zone[0].metv.vpd_ravg, idayl, zone[0].metv.dayl_ravg, gsi); */
	return(gsi);
} /* end compute_growingseason_index */

