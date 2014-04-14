/*--------------------------------------------------------------*/
/*                                                              */ 
/*		compute_annual_seedfall				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_annual_seedfall				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* void compute_annual_seedfall(				*/ 
/*			struct epconst_struct  epc,		*/
/*			struct phenology_struct *phen,		*/
/*			struct cstate_struct	*cs,		*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	computes leaf and froot seedfall			*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

int compute_annual_seedfall( 
						   struct epconst_struct	epc	,
						   struct reproduction_phenology_struct *phen,
						   struct cstate_struct *cs)
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	/*  seedfall */
	if (epc.phenology_type == EVERGREEN) {
		phen->reprodlitfallc = (cs->reprodc) * epc.reprod_turnover;
	}
	else{
		phen->reprodlitfallc = (cs->reprodc); 
	}
	return (!ok);
} /* end compute_annual_seedfall */


