/*--------------------------------------------------------------*/
/* 								*/
/*		zero_stratum_daily_flux				*/
/*								*/
/*								*/
/*	NAME							*/
/*	zero_stratum_daily_flux					*/
/*								*/
/*	SYNOPSIS						*/
/*	int zero_stratum_daily_flux(				*/
/*		struct cdayflux_struct *,			*/
/*		struct ndayflux_struct *)			*/
/*								*/
/*	returns flag (0 for okay):				*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	zeros daily flux (carbon and nitrogen) variables	*/
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

int zero_stratum_daily_flux(
							struct cdayflux_struct *cdf,
							struct ndayflux_struct *ndf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok=1;

		
	/* sets all the daily flux variables to 0.0 at the beginning of the day */
	/* carbon fluxes */
	cdf->psn_to_cpool = 0.0;    /* (kgC/m2/d) gross photosynthesis */
	/* daily phenology fluxes */
	cdf->leafc_transfer_to_leafc = 0.0;
	cdf->frootc_transfer_to_frootc = 0.0;
	cdf->livestemc_transfer_to_livestemc = 0.0;
	cdf->deadstemc_transfer_to_deadstemc = 0.0;
	cdf->livecrootc_transfer_to_livecrootc = 0.0;
	cdf->deadcrootc_transfer_to_deadcrootc = 0.0;
	cdf->livestemc_to_deadstemc = 0.0;
	cdf->livecrootc_to_deadcrootc = 0.0;
	/* daily growth fluxes */
	cdf->cpool_to_leafc = 0.0;
	cdf->cpool_to_leafc_store = 0.0;
	cdf->cpool_to_frootc = 0.0;
	cdf->cpool_to_frootc_store = 0.0;
	cdf->cpool_to_livestemc = 0.0;
	cdf->cpool_to_livestemc_store = 0.0;
	cdf->cpool_to_deadstemc = 0.0;
	cdf->cpool_to_deadstemc_store = 0.0;
	cdf->cpool_to_livecrootc = 0.0;
	cdf->cpool_to_livecrootc_store = 0.0;
	cdf->cpool_to_deadcrootc = 0.0;
	cdf->cpool_to_deadcrootc_store = 0.0;
	cdf->cpool_to_gresp_store = 0.0;
	/* annual growth fluxes */
	cdf->leafc_store_to_leafc_transfer = 0.0;
	cdf->frootc_store_to_frootc_transfer = 0.0;
	cdf->livestemc_store_to_livestemc_transfer = 0.0;
	cdf->deadstemc_store_to_deadstemc_transfer = 0.0;
	cdf->livecrootc_store_to_livecrootc_transfer = 0.0;
	cdf->deadcrootc_store_to_deadcrootc_transfer = 0.0;
	cdf->gresp_store_to_gresp_transfer = 0.0;

	/* autotroph  maintenance respiration fluxes */
	cdf->leaf_day_mr = 0.0;
	cdf->leaf_night_mr = 0.0;
	cdf->froot_mr = 0.0;
	cdf->livestem_mr = 0.0;
	cdf->livecroot_mr = 0.0;
  cdf->total_mr = 0.0;
	/* growth respiration fluxes */
	cdf->transfer_leaf_gr = 0.0;
	cdf->cpool_froot_gr = 0.0;
	cdf->transfer_froot_gr = 0.0;
	cdf->cpool_livestem_gr = 0.0;
	cdf->transfer_livestem_gr = 0.0;
	cdf->cpool_deadstem_gr = 0.0;
	cdf->transfer_deadstem_gr = 0.0;
	cdf->cpool_livecroot_gr = 0.0;
	cdf->transfer_livecroot_gr = 0.0;
	cdf->cpool_deadcroot_gr = 0.0;
	cdf->transfer_deadcroot_gr = 0.0;
	/* daily nitrogen fluxes */
	/* daily phenology fluxes */
	ndf->leafn_transfer_to_leafn = 0.0;
	ndf->frootn_transfer_to_frootn = 0.0;
	ndf->livestemn_transfer_to_livestemn = 0.0;
	ndf->deadstemn_transfer_to_deadstemn = 0.0;
	ndf->livecrootn_transfer_to_livecrootn = 0.0;
	ndf->deadcrootn_transfer_to_deadcrootn = 0.0;
	ndf->livestemn_to_deadstemn = 0.0;
	ndf->livestemn_to_retransn = 0.0;
	ndf->livecrootn_to_deadcrootn = 0.0;
	ndf->livecrootn_to_retransn = 0.0;
	/* daily growth fluxes */
	ndf->potential_N_uptake = 0.0;
	ndf->sminn_to_npool = 0.0;
	ndf->retransn_to_npool = 0.0;
	ndf->npool_to_leafn = 0.0;
	ndf->npool_to_leafn_store = 0.0;
	ndf->npool_to_frootn = 0.0;
	ndf->npool_to_frootn_store = 0.0;
	ndf->npool_to_livestemn = 0.0;
	ndf->npool_to_livestemn_store = 0.0;
	ndf->npool_to_deadstemn = 0.0;
	ndf->npool_to_deadstemn_store = 0.0;
	ndf->npool_to_livecrootn = 0.0;
	ndf->npool_to_livecrootn_store = 0.0;
	ndf->npool_to_deadcrootn = 0.0;
	ndf->npool_to_deadcrootn_store = 0.0;
	/* annual growth fluxes */
	ndf->leafn_store_to_leafn_transfer = 0.0;
	ndf->frootn_store_to_frootn_transfer = 0.0;
	ndf->livestemn_store_to_livestemn_transfer = 0.0;
	ndf->deadstemn_store_to_deadstemn_transfer = 0.0;
	ndf->livecrootn_store_to_livecrootn_transfer = 0.0;
	ndf->deadcrootn_store_to_deadcrootn_transfer = 0.0;
	return (!ok);
} /* end zero_stratum_daily_flux */





