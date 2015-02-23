/*--------------------------------------------------------------*/
/* 								*/
/*			compute_resprout_from_seed.c			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_resprout_from_seed - 				*/
/*		computes resprouting from seed		 	*/
/*			 and	updates carbon and nitrogen stores		*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_resprout_from_seed( 				*/
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

int	compute_resprout_from_seed(
						 struct epconst_struct epc,
						 struct cstate_struct *cs,
						 struct nstate_struct *ns,
						 struct cdayflux_patch_struct *cdf_patch,
						 struct cdayflux_struct *cdf,
						 struct ndayflux_struct *ndf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	double availc, availn, releasedc, releasedn, 
	double newplantn, newplantc;
	double litrc, litrn, excessn, excessc;
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*  Compute seed resprout carbon */
	/* environmental controls on resprouting should go here */
	/*  							*/
	/*------------------------------------------------------*/
	availc = epc.prop_seed_resprout * cs->seedc;


	releasedc = availc * (1.0-epc.alloc_seed_to_leafc-epc.alloc_seedc_to_frootc - epc.alloc_seedc_to_cpool);
	releasedc = max(0, releasedc);
	newplantc = availc-releasedc;

	/*------------------------------------------------------*/
	/* allocate new carbon to existing strata		*/
	/*------------------------------------------------------*/


	cdf->seedc_to_cpool = epc.alloc_seedc_to_cpool * newplantc;
	cdf->seedc_to_leafc_transfer = epc.alloc_seedc_to_leafc * newplantc; 
	cdf->seedc_to_frootc_transfer = epc.alloc_seedc_to_frootc * newplantc; 

	cs->cpool += cdf->seedc_to_cpool;
	cs->leafc_transfer += cdf->seedc_to_leafc_transfer;
	cs->frootc_transfer += cdf->seedc_to_frootc_transfer;

	cs->seedc -= availc;
	
	/*------------------------------------------------------*/
	/* allocated corresponding N 				*/
	/*------------------------------------------------------*/
	if (ns->npool > ZERO)
		pool_cn = cs->cpool/ns->npool;
	else
		pool_cn = epc.leaf_cn;

	ndf->seedn_to_npool = cdf->seedc_to_cpool/pool_cn;
	ndf->seedn_to_leafn_transfer = cdf->seedc_to_leafc_transfer/epc.leaf_cn;
	ndf->seedn_to_frootn_transfer = cdf->seedc_to_frootc_transfer/epc.froot_cn;

	newplantn = ndf->seedn_to_npool + ndf->seedn_to_leafc_transfer + ndf->seedn_to_frootc_transfer;
	ns->npool += ndf->seedn_to_npool;
	ns->leafn_transfer += ndf->seedn_to_leafn_transfer;
	ns->frootn_transfer += ndf->seedn_to_frootn_transfer;

	availn  = availc / epc.seed_cn;	
	ns->seedn -= availn;


	/*------------------------------------------------------*/
	/* return excess C and N to litter 			*/
	/*------------------------------------------------------*/
	releasedn = availn - newplantn;
	if (releasedn > ZERO) {
		released_cn = releasedc/releasedn;
		if (released_cn < LIG_CN) {
			if (released_cn < CEL_CN) {
				if (released_cn < LIVELAB_CN) {
				   litrc = releasedn*LIVELAB_CN;
				   cs_litr->litr1c += litrc;
				   ns_litr->litr1n += releasedn;
				   cdf_patch->litr1c_hr = releasedc-litrc;
				}	
				else {
					excessn = releasedn - releasedc/LIVEAB_CN;
					litrn = releasedn-excessn;
					ns_litr->litr1n += litrn;
					cs_litr->litr1c += releasedc;
					ns->sminn += excessn;
				}
			}
			else {
				excessn = releasedn - releasedc/CEL_CN;
				litrn = releasedn-excessn;
				ns_litr->litr2n += litrn;
				cs_litr->litr2c += releasedc;
				ns->sminn += excessn;
			}
		}
		else {
			excessn = releasedn - releasedc/LIG_CN;
			litrn = releasedn-excessn;
			ns_litr->litr4n += litrn;
			cs_litr->litr4c += releasedc;
			ns->sminn += excessn;
		}
	} 


	return(0);
} /*compute_resprout_from_seed*/ 
