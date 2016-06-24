/*--------------------------------------------------------------*/
/*                                                              */ 
/*		compute_annual_litfall				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_annual_litfall				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* void compute_annual_litfall(				*/ 
/*			struct epconst_struct  epc,		*/
/*			struct phenology_struct *phen,		*/
/*			struct cstate_struct	*cs,		*/
/*			int grow_flag				*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	computes leaf and froot litfall			*/
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

int compute_annual_litfall( 
						   struct epconst_struct	epc	,
						   struct phenology_struct *phen,
						   struct cstate_struct *cs,
						   int grow_flag)
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	/* leaf and fine root litfall */
	if (epc.phenology_type == EVERGREEN) {
		phen->leaflitfallc = (cs->leafc) * epc.leaf_turnover;
		if (grow_flag > 0)
			phen->frootlitfallc = (cs->frootc) * epc.froot_turnover;
	}
	else{
		phen->leaflitfallc = (cs->leafc);
		if (grow_flag > 0)
			phen->frootlitfallc = (cs->frootc) * epc.froot_turnover;
	}
	phen->leaflitfallc_wstress = 0.0;
	return (!ok);
} /* end compute_annual_litfall */


