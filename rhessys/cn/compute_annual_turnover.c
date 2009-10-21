/*--------------------------------------------------------------*/
/*                                                              */ 
/*		compute_annual_turnover				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_annual_turnover				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* void compute_annual_turnover(				*/ 
/*			struct epconst_struct  epc,		*/
/*			struct epvar_struct	*epv,		*/
/*			struct phenology_struct *phen,		*/
/*			struct cstate_struct	*cs,		*/
/*			struct nstate_struct	*ns,		*/
/*			int grow_flag				*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	computes livewood turnover (stem and coarse root)	*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*	P.Thornton (1998) version of BIOME_bgc			*/
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include "rhessys.h"

int compute_annual_turnover( 
							struct epconst_struct	epc,
							struct epvar_struct	*epv,
							struct cstate_struct *cs)
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	if ( (epc.veg_type == TREE) ){
		epv->day_livestem_turnover = (cs->live_stemc
			+ cs->livestemc_transfer + cs->livestemc_store)
			* epc.livewood_turnover / 365;
		epv->day_livecroot_turnover = (cs->live_crootc
			+ cs->livecrootc_transfer + cs->livecrootc_store)
			* epc.livewood_turnover	/ 365;
	}
	if (epc.veg_type == GRASS)
		epv->day_deadleaf_turnover = epc.deadleaf_turnover
		* (cs->dead_leafc ) / 365;

	return (!ok);
} /* end compute_annual_turnover */
