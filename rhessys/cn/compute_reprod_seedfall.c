/*--------------------------------------------------------------*/
/* 								*/
/*			compute_reprod_seedfall.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_reprod_seedfall - computes reprod litfall and 	*/
/*				updates carbon stores		*/
/*				for patch.			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_reprod_seedfall( 				*/
/*					);			*/	
/*								*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	source from Peter Thornton, 1d_bgc, 1997		*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

int	compute_reprod_seedfall(
						 struct epconst_struct epc,
						 double litfallc,
						 double cover_fraction,
						 struct cstate_struct *cs,
						 struct nstate_struct *ns,
						 struct cdayflux_struct *cdf_stratum,
						 struct ndayflux_struct *ndf_stratum)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok=1;
	double c1,c2,c3,c4;
	double n1,n2,n3,n4;
	double nretrans, nloss;
	double avg_cn;
	
	avg_cn = cs->reprodc/ns->reprodn;

	/*------------------------------------------------------*/
	/*	Don't allow more leaves to fall than exist	*/
	/*------------------------------------------------------*/
	litfallc = min(cs->reprodc, litfallc);

	/*------------------------------------------------------*/
	/* add litter to seed pool				*/
	/*------------------------------------------------------*/
	cdf_stratum->reprodc_to_seedc = litfallc;
	/* update state variables */
	cs->reprodc -= cdf_stratum->reprodc_to_seedc;
	cs->seedc += cdf_stratum->reprodc_to_seedc;

	/* nitrogen state variable updates */
	nloss = litfallc/avg_cn;
	nretrans = max(0, nloss-litfallc/epc.seed_cn);
	/* update state variables */
	ns->retransn += nretrans;
	ns->reprodn -= nloss;
	ndf_stratum->reprodn_to_seedn = nloss - nretrans ;
	ns->seedn += ndf_stratum->reprodn_to_seedn;

	return(0);
} /*compute_reprod_seedfall*/ 
