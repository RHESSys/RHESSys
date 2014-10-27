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
				struct epvar_struct	*epv ,
				struct epconst_struct epc) 
			
	{ 
		
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

	double gsi, itmin, ivpd, idayl, ipsi;

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
	ipsi = 1.0;
	if (epv->psi_ravg  >= epc.gs_psi_max)
		ipsi = 1.0;
	else
		ipsi = (epv->psi_ravg - epc.gs_psi_min)/(epc.gs_psi_range);

	
	itmin = max(itmin, 0.0);
	ivpd = max(ivpd, 0.0);
	idayl = max(idayl, 0.0);
	ipsi = max(ipsi, 0.0);
	
	gsi = idayl*itmin*ivpd*ipsi;
 /* printf("\n tmin %lf %lf vpd %lf %lf dayl %lf %lf psi %lf %lf gsi %lf", itmin,
	zone[0].metv.tmin_ravg, ivpd, zone[0].metv.vpd_ravg, idayl, zone[0].metv.dayl_ravg, ipsi, epv->psi,gsi);     */
	return(gsi);
} /* end compute_growingseason_index */

