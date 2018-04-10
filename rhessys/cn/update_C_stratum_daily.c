/*--------------------------------------------------------------*/
/* 								*/
/*		update_C_stratum_daily				*/
/*								*/
/*								*/
/*	NAME							*/
/*	update_C_stratum_daily -					*/
/*								*/
/*		updates daily C stores to			*/
/*		account for psn and respiration 		*/
/*	SYNOPSIS						*/
/*	double	update_C_stratum_daily(					*/
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
/*								*/
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int update_C_stratum_daily(struct epconst_struct epc,
						   struct cstate_struct *cs, struct cdayflux_struct *cdf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok=1;
	
	/* Daily photosynthesis */
	cs->gpsn_src	+= cdf->psn_to_cpool;
	cs->cpool	+= cdf->psn_to_cpool;
	/* Daily maintenance respiration */
	cs->cpool	-= cdf->leaf_day_mr;
	cs->cpool	-= cdf->leaf_night_mr;
	cs->leaf_mr_snk += cdf->leaf_day_mr + cdf->leaf_night_mr;
	cs->cpool	-= cdf->froot_mr;
	cs->froot_mr_snk += cdf->froot_mr;
	if (epc.veg_type == TREE) {
		cs->cpool	-= cdf->livestem_mr;
		cs->cpool	-= cdf->livecroot_mr;
		cs->livestem_mr_snk += cdf->livestem_mr;
		cs->livecroot_mr_snk += cdf->livecroot_mr;
	}
	cs->net_psn = cdf->psn_to_cpool - cdf->total_mr - cdf->total_gr;


	/* Daily allocation fluxes */
	/* daily leaf allocation fluxes */
	cs->leafc          += cdf->cpool_to_leafc;
	cs->cpool          -= cdf->cpool_to_leafc;
	cs->leafc_store  += cdf->cpool_to_leafc_store;
	cs->cpool          -= cdf->cpool_to_leafc_store;
	/* Daily fine root allocation fluxes */
	cs->frootc         += cdf->cpool_to_frootc;
	cs->cpool          -= cdf->cpool_to_frootc;
	cs->frootc_store += cdf->cpool_to_frootc_store;
	cs->cpool          -= cdf->cpool_to_frootc_store;
	if (epc.veg_type == TREE){
		/* Daily live stem wood allocation fluxes */
		cs->live_stemc          += cdf->cpool_to_livestemc;
		cs->cpool              -= cdf->cpool_to_livestemc;
		cs->livestemc_store  += cdf->cpool_to_livestemc_store;
		cs->cpool              -= cdf->cpool_to_livestemc_store;
		/* Daily dead stem wood allocation fluxes */
		cs->dead_stemc          += cdf->cpool_to_deadstemc;
		cs->cpool              -= cdf->cpool_to_deadstemc;
		cs->deadstemc_store  += cdf->cpool_to_deadstemc_store;
		cs->cpool              -= cdf->cpool_to_deadstemc_store;
		/* Daily live coarse root wood allocation fluxes */
		cs->live_crootc         += cdf->cpool_to_livecrootc;
		cs->cpool              -= cdf->cpool_to_livecrootc;
		cs->livecrootc_store += cdf->cpool_to_livecrootc_store;
		cs->cpool              -= cdf->cpool_to_livecrootc_store;
		/* Daily dead coarse root wood allocation fluxes */
		cs->dead_crootc         += cdf->cpool_to_deadcrootc;
		cs->cpool              -= cdf->cpool_to_deadcrootc;
		cs->deadcrootc_store += cdf->cpool_to_deadcrootc_store;
		cs->cpool              -= cdf->cpool_to_deadcrootc_store;
	}
	/* Daily allocation for transfer growth respiration */
	cs->gresp_store  += cdf->cpool_to_gresp_store;
	cs->cpool          -= cdf->cpool_to_gresp_store;
	/* Daily growth respiration fluxes */
	/* Leaf growth respiration */
	cs->leaf_gr_snk     += cdf->cpool_leaf_gr;
	cs->cpool           -= cdf->cpool_leaf_gr;
	cs->leaf_gr_snk     += cdf->transfer_leaf_gr;
	cs->gresp_transfer  -= cdf->transfer_leaf_gr;
	/* Fine root growth respiration */
	cs->froot_gr_snk    += cdf->cpool_froot_gr;
	cs->cpool           -= cdf->cpool_froot_gr;
	cs->froot_gr_snk    += cdf->transfer_froot_gr;
	cs->gresp_transfer  -= cdf->transfer_froot_gr;
	if (epc.veg_type == TREE){
		/* Live stem growth respiration */
		cs->livestem_gr_snk  += cdf->cpool_livestem_gr;
		cs->cpool            -= cdf->cpool_livestem_gr;
		cs->livestem_gr_snk  += cdf->transfer_livestem_gr;
		cs->gresp_transfer   -= cdf->transfer_livestem_gr;
		/* Dead stem growth respiration */
		cs->deadstem_gr_snk  += cdf->cpool_deadstem_gr;
		cs->cpool            -= cdf->cpool_deadstem_gr;
		cs->deadstem_gr_snk  += cdf->transfer_deadstem_gr;
		cs->gresp_transfer   -= cdf->transfer_deadstem_gr;
		/* Live coarse root growth respiration */
		cs->livecroot_gr_snk += cdf->cpool_livecroot_gr;
		cs->cpool            -= cdf->cpool_livecroot_gr;
		cs->livecroot_gr_snk += cdf->transfer_livecroot_gr;
		cs->gresp_transfer   -= cdf->transfer_livecroot_gr;
		/* Dead coarse root growth respiration */
		cs->deadcroot_gr_snk += cdf->cpool_deadcroot_gr;
		cs->cpool            -= cdf->cpool_deadcroot_gr;
		cs->deadcroot_gr_snk += cdf->transfer_deadcroot_gr;
		cs->gresp_transfer   -= cdf->transfer_deadcroot_gr;
	}

	return (!ok);
}		

