/*--------------------------------------------------------------*/
/* 								*/
/*								*/
/*	compute_growth_resp					*/
/*								*/
/*	NAME							*/
/*		compute_growth_resp				*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_growth_resp(				*/
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*		from Peter Thornton, 1997 - 1d_bgc		*/
/*								*/
/*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

int compute_growth_resp(int alloc, struct epconst_struct epc,
						struct cdayflux_struct* cdf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	double g1;   /* RATIO   C respired for growth : C grown  */
	g1 = epc.gr_perc;
	/* leaf and fine root growth respiration for both trees and grass */
	cdf->cpool_leaf_gr     = cdf->cpool_to_leafc * g1;
	cdf->cpool_froot_gr    = cdf->cpool_to_frootc * g1;
	cdf->transfer_leaf_gr  = cdf->leafc_transfer_to_leafc * g1;
	cdf->transfer_froot_gr = cdf->frootc_transfer_to_frootc * g1;
	cdf->total_gr = cdf->cpool_leaf_gr + cdf->cpool_froot_gr +
		cdf->transfer_leaf_gr + cdf->transfer_froot_gr;
	/* woody tissue growth respiration only for trees */
	if (epc.veg_type == TREE){
		cdf->cpool_livestem_gr     = cdf->cpool_to_livestemc * g1;
		cdf->cpool_deadstem_gr     = cdf->cpool_to_deadstemc * g1;
		cdf->cpool_livecroot_gr    = cdf->cpool_to_livecrootc * g1;
		cdf->cpool_deadcroot_gr    = cdf->cpool_to_deadcrootc * g1;
		cdf->transfer_livestem_gr  = cdf->livestemc_transfer_to_livestemc * g1;
		cdf->transfer_deadstem_gr  = cdf->deadstemc_transfer_to_deadstemc * g1;
		cdf->transfer_livecroot_gr = cdf->livecrootc_transfer_to_livecrootc * g1;
		cdf->transfer_deadcroot_gr = cdf->deadcrootc_transfer_to_deadcrootc * g1;
		cdf->total_gr += cdf->cpool_livestem_gr + cdf->cpool_deadstem_gr
			+ cdf->cpool_livecroot_gr + cdf->cpool_deadcroot_gr
			+ cdf->transfer_livecroot_gr + cdf->transfer_deadcroot_gr
			+ cdf->transfer_livestem_gr + cdf->transfer_deadstem_gr ;
	}
	return (!ok);
}/* end compute_growth_resp.c */
