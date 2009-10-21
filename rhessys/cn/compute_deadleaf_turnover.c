/*--------------------------------------------------------------*/
/* 								*/
/*			compute_deadleaf_turnover.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_deadleaf_turnover - computes turnover of standing but dead grass	*/
/*								(so that it enters litter pool		*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_deadleaf_turnover( 				*/
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

int	compute_deadleaf_turnover(
							  struct epconst_struct epc,
							  struct epvar_struct *epv,
							  double cover_fraction,
							  struct cstate_struct *cs,
							  struct nstate_struct *ns,
							  struct litter_c_object *cs_litr,
							  struct litter_n_object *ns_litr,
							  struct cdayflux_patch_struct *cdf,
							  struct ndayflux_patch_struct *ndf,
							  int grow_flag)
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
	double turnover;
	turnover = epv->day_deadleaf_turnover;
	c1 = turnover * epc.leaflitr_flab;
	c2 = turnover * epc.leaflitr_fucel;
	n2 = turnover * epc.leaflitr_fucel/CEL_CN;
	c3 = turnover * epc.leaflitr_fscel;
	n3 = turnover * epc.leaflitr_fscel/CEL_CN;
	c4 = turnover * epc.leaflitr_flig;
	n4 = turnover * epc.leaflitr_flig/LIG_CN;
	n1 = ((c1+c2+c3+c4)/epc.leaflitr_cn)-n2-n3-n4;
	/* set fluxes in daily flux structure */
	cdf->leafc_to_litr1c += c1 * cover_fraction;
	cdf->leafc_to_litr2c += c2 * cover_fraction;
	cdf->leafc_to_litr3c += c3 * cover_fraction;
	cdf->leafc_to_litr4c += c4 * cover_fraction;
	ndf->leafn_to_litr1n += n1 * cover_fraction;
	ndf->leafn_to_litr2n += n2 * cover_fraction;
	ndf->leafn_to_litr3n += n3 * cover_fraction;
	ndf->leafn_to_litr4n += n4 * cover_fraction;
	/* update state variables */
	cs->dead_leafc -= (c1 + c2 + c3 + c4);
	cs_litr->litr1c += c1 * cover_fraction;
	cs_litr->litr2c += c2 * cover_fraction;
	cs_litr->litr3c += c3 * cover_fraction;
	cs_litr->litr4c += c4 * cover_fraction;
	/* nitrogen state variable updates */
	ns->dead_leafn -= (n1 + n2 + n3 + n4);
	ns_litr->litr1n += n1 * cover_fraction;
	ns_litr->litr2n += n2 * cover_fraction;
	ns_litr->litr3n += n3 * cover_fraction;
	ns_litr->litr4n += n4 * cover_fraction;
	return(0);
} /*compute_deadleaf_turnover*/ 
