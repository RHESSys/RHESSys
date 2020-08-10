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
	double leaf_turnover, froot_turnover;
	/*------------------------------------------------------*/
	/* include an option for resprouts to have no turn over */
	/*------------------------------------------------------*/


	leaf_turnover = epc.leaf_turnover;	
	if ((epc.leaf_turnover > ZERO) && (epc.zero_turnover_sprouts==1)) { 
		if (cs->age <= 1/epc.leaf_turnover)
			leaf_turnover = 0.0;
		else
			leaf_turnover = epc.leaf_turnover;

	}

	froot_turnover = epc.froot_turnover;	
	if ((epc.froot_turnover > ZERO) && (epc.zero_turnover_sprouts==1)) { 
		if (cs->age <= 1/epc.froot_turnover)
			froot_turnover = 0.0;
		else
			froot_turnover = epc.froot_turnover;

	}


	
	/* leaf and fine root litfall */
	if (epc.phenology_type == EVERGREEN) {
		phen->leaflitfallc = (cs->leafc) * leaf_turnover;
		if (grow_flag > 0)
			phen->frootlitfallc = (cs->frootc) * froot_turnover;
	}
	else{
		phen->leaflitfallc = (cs->leafc);
		if (grow_flag > 0)
			phen->frootlitfallc = (cs->frootc) * froot_turnover;
	}

		
	phen->leaflitfallc_wstress = 0.0;
	return (!ok);
} /* end compute_annual_litfall */


