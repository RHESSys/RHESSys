/*--------------------------------------------------------------*/
/*                                                              */ 
/*		update_mortality									*/
/*                                                              */
/*  NAME                                                        */
/*		update_mortality									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 	void update_mortality( 
/*                      struct epconst_struct,			*/
/*                      struct phenology_struct *,		*/
/*                      struct cstate_struct *,			*/
/*                      struct cdayflux_struct *,		*/
/*                      struct cdayflux_patch_struct *,		*/
/*                      struct nstate_struct *,			*/
/*                      struct ndayflux_struct *,		*/
/*                      struct ndayflux_patch_struct *,		*/
/*                      struct litter_c_object *,		*/
/*                      struct litter_n_object *,		*/
/*                      double);				*/
/*								*/
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*                                                              */
/*	calculated daily mortality losses and updates 		*/
/*	carbon and nitrogen pools				*/
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*	from P.Thornton (1997) version of 1d_bgc		*/
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"
void update_mortality(
					  struct epconst_struct	epc	,
					  struct cstate_struct *cs,
					  struct cdayflux_struct	*cdf,
					  struct cdayflux_patch_struct *cdf_patch,
					  struct nstate_struct *ns,
					  struct ndayflux_struct	*ndf,
					  struct ndayflux_patch_struct *ndf_patch,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  double	cover_fraction,
					  double	mort)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double m_cpool, m_npool;
	double m_leafc_to_litr1c, m_leafc_to_litr2c;
	double m_leafc_to_litr3c, m_leafc_to_litr4c;
	double m_frootc_to_litr1c, m_frootc_to_litr2c;
	double m_frootc_to_litr3c, m_frootc_to_litr4c;
	double m_leafc_store_to_litr1c;
	double m_frootc_store_to_litr1c;
	double m_livestemc_store_to_litr1c;
	double m_deadstemc_store_to_litr1c;
	double m_livecrootc_store_to_litr1c;
	double m_deadcrootc_store_to_litr1c;
	double m_leafc_transfer_to_litr1c;
	double m_frootc_transfer_to_litr1c;
	double m_livestemc_transfer_to_litr1c;
	double m_deadstemc_transfer_to_litr1c;
	double m_livecrootc_transfer_to_litr1c;
	double m_deadcrootc_transfer_to_litr1c;
	double m_gresp_store_to_litr1c;
	double m_gresp_transfer_to_litr1c;
	double m_leafn_to_litr1n, m_leafn_to_litr2n;
	double m_leafn_to_litr3n, m_leafn_to_litr4n;
	double m_frootn_to_litr1n, m_frootn_to_litr2n;
	double m_frootn_to_litr3n, m_frootn_to_litr4n;
	double m_livestemn_to_litr1n, m_livecrootn_to_litr1n;
	double m_leafn_store_to_litr1n;
	double m_frootn_store_to_litr1n;
	double m_livestemn_store_to_litr1n;
	double m_deadstemn_store_to_litr1n;
	double m_livecrootn_store_to_litr1n;
	double m_deadcrootn_store_to_litr1n;
	double m_leafn_transfer_to_litr1n;
	double m_frootn_transfer_to_litr1n;
	double m_livestemn_transfer_to_litr1n;
	double m_deadstemn_transfer_to_litr1n;
	double m_livecrootn_transfer_to_litr1n;
	double m_deadcrootn_transfer_to_litr1n;
	double m_retransn_to_litr1n;
	double m_livestemc_to_cwdc;
	double m_deadstemc_to_cwdc;
	double m_livecrootc_to_cwdc;
	double m_deadcrootc_to_cwdc;
	double m_livestemn_to_cwdn;
	double m_deadstemn_to_cwdn;
	double m_livecrootn_to_cwdn;
	double m_deadcrootn_to_cwdn;
	/******************************************************************/
	/* Non-fire mortality: these fluxes all enter litter or CWD pools */
	/******************************************************************/
	/* daily carbon fluxes due to mortality */
	/* mortality fluxes out of leaf and fine root pools */
	/* carbon depts in cpool have to die with the plant - could result in a carbon balance issue */
	m_cpool = mort* cs->cpool;
	m_npool = mort* ns->npool;
	m_leafc_to_litr1c = mort * cs->leafc * epc.leaflitr_flab;
	m_leafc_to_litr2c = mort * cs->leafc * epc.leaflitr_fucel;
	m_leafc_to_litr3c = mort * cs->leafc * epc.leaflitr_fscel;
	m_leafc_to_litr4c = mort * cs->leafc * epc.leaflitr_flig;
	m_frootc_to_litr1c = mort * cs->frootc * epc.frootlitr_flab;
	m_frootc_to_litr2c = mort * cs->frootc * epc.frootlitr_fucel;
	m_frootc_to_litr3c = mort * cs->frootc * epc.frootlitr_fscel;
	m_frootc_to_litr4c = mort * cs->frootc * epc.frootlitr_flig;
	/* mortality fluxes out of storage and transfer pools */
	m_leafc_store_to_litr1c  = mort * cs->leafc_store;
	m_frootc_store_to_litr1c  = mort * cs->frootc_store;
	m_livestemc_store_to_litr1c  = mort * cs->livestemc_store;
	m_deadstemc_store_to_litr1c  = mort * cs->deadstemc_store;
	m_livecrootc_store_to_litr1c  = mort * cs->livecrootc_store;
	m_deadcrootc_store_to_litr1c  = mort * cs->deadcrootc_store;
	m_leafc_transfer_to_litr1c = mort * cs->leafc_transfer;
	m_frootc_transfer_to_litr1c = mort * cs->frootc_transfer;
	m_livestemc_transfer_to_litr1c = mort * cs->livestemc_transfer;
	m_deadstemc_transfer_to_litr1c = mort * cs->deadstemc_transfer;
	m_livecrootc_transfer_to_litr1c = mort * cs->livecrootc_transfer;
	m_deadcrootc_transfer_to_litr1c = mort * cs->deadcrootc_transfer;
	m_gresp_store_to_litr1c = mort * cs->gresp_store;
	m_gresp_transfer_to_litr1c = mort * cs->gresp_transfer;
	/* TREE-specific carbon fluxes */
	if (epc.veg_type==TREE){
		m_livestemc_to_cwdc = mort * cs->live_stemc;
		m_deadstemc_to_cwdc = mort * cs->dead_stemc;
		m_livecrootc_to_cwdc = mort * cs->live_crootc;
		m_deadcrootc_to_cwdc = mort * cs->dead_crootc;
	}
	/* daily nitrogen fluxes due to mortality */
	/* daily nitrogen fluxes due to mortality */
	/* mortality fluxes out of leaf and fine root pools */
	m_leafn_to_litr1n = m_leafc_to_litr1c / epc.leaf_cn;
	m_leafn_to_litr2n = m_leafc_to_litr2c / epc.leaf_cn;
	m_leafn_to_litr3n = m_leafc_to_litr3c / epc.leaf_cn;
	m_leafn_to_litr4n = m_leafc_to_litr4c / epc.leaf_cn;
	m_frootn_to_litr1n = m_frootc_to_litr1c / epc.froot_cn;
	m_frootn_to_litr2n = m_frootc_to_litr2c / epc.froot_cn;
	m_frootn_to_litr3n = m_frootc_to_litr3c / epc.froot_cn;
	m_frootn_to_litr4n = m_frootc_to_litr4c / epc.froot_cn;

	/* mortality fluxes out of storage and transfer pools */
	m_leafn_store_to_litr1n  = mort * ns->leafn_store;
	m_frootn_store_to_litr1n  = mort * ns->frootn_store;
	m_livestemn_store_to_litr1n  = mort * ns->livestemn_store;
	m_deadstemn_store_to_litr1n  = mort * ns->deadstemn_store;
	m_livecrootn_store_to_litr1n  = mort * ns->livecrootn_store;
	m_deadcrootn_store_to_litr1n  = mort * ns->deadcrootn_store;
	m_leafn_transfer_to_litr1n = mort * ns->leafn_transfer;
	m_frootn_transfer_to_litr1n = mort * ns->frootn_transfer;
	m_livestemn_transfer_to_litr1n = mort * ns->livestemn_transfer;
	m_deadstemn_transfer_to_litr1n = mort * ns->deadstemn_transfer;
	m_livecrootn_transfer_to_litr1n = mort * ns->livecrootn_transfer;
	m_deadcrootn_transfer_to_litr1n = mort * ns->deadcrootn_transfer;
	m_retransn_to_litr1n = mort * ns->retransn;

	/* TREE-specific nitrogen fluxes */
	if (epc.veg_type==TREE){
		m_livestemn_to_cwdn = m_livestemc_to_cwdc / epc.deadwood_cn;
		m_livecrootn_to_cwdn = m_livecrootc_to_cwdc / epc.deadwood_cn;
		m_livestemn_to_litr1n
			= (mort * ns->live_stemn) - m_livestemn_to_cwdn;
		m_livecrootn_to_litr1n
			= (mort * ns->live_crootn) - m_livecrootn_to_cwdn;
		m_deadstemn_to_cwdn = mort * ns->dead_stemn;
		m_deadcrootn_to_cwdn = mort * ns->dead_crootn;

	}
	/* update state variables */
	/* CARBON mortality state variable update */
	/*   Leaf mortality */
	cs_litr->litr1c    += m_cpool * cover_fraction;
	cs->cpool -= m_cpool;
	cs_litr->litr1c    += m_leafc_to_litr1c * cover_fraction;
	cs->leafc          -= m_leafc_to_litr1c;
	cs_litr->litr2c    += m_leafc_to_litr2c * cover_fraction;
	cs->leafc          -= m_leafc_to_litr2c;
	cs_litr->litr3c    += m_leafc_to_litr3c * cover_fraction;
	cs->leafc          -= m_leafc_to_litr3c;
	cs_litr->litr4c    += m_leafc_to_litr4c * cover_fraction;
	cs->leafc          -= m_leafc_to_litr4c;
	/*   Fine root mortality */
	cs_litr->litr1c    += m_frootc_to_litr1c * cover_fraction;
	cs->frootc         -= m_frootc_to_litr1c;
	cs_litr->litr2c    += m_frootc_to_litr2c * cover_fraction;
	cs->frootc         -= m_frootc_to_litr2c;
	cs_litr->litr3c    += m_frootc_to_litr3c * cover_fraction;
	cs->frootc         -= m_frootc_to_litr3c;
	cs_litr->litr4c    += m_frootc_to_litr4c * cover_fraction;
	cs->frootc         -= m_frootc_to_litr4c;
	/*   Storage and transfer mortality */
	cs_litr->litr1c         += m_leafc_store_to_litr1c * cover_fraction;
	cs->leafc_store       -= m_leafc_store_to_litr1c;
	cs_litr->litr1c         += m_frootc_store_to_litr1c * cover_fraction;
	cs->frootc_store      -= m_frootc_store_to_litr1c;
	cs_litr->litr1c         += m_livestemc_store_to_litr1c * cover_fraction;
	cs->livestemc_store   -= m_livestemc_store_to_litr1c;
	cs_litr->litr1c         += m_deadstemc_store_to_litr1c * cover_fraction;
	cs->deadstemc_store   -= m_deadstemc_store_to_litr1c;
	cs_litr->litr1c         += m_livecrootc_store_to_litr1c * cover_fraction;
	cs->livecrootc_store  -= m_livecrootc_store_to_litr1c;
	cs_litr->litr1c         += m_deadcrootc_store_to_litr1c * cover_fraction;
	cs->deadcrootc_store  -= m_deadcrootc_store_to_litr1c;
	cs_litr->litr1c         += m_leafc_transfer_to_litr1c * cover_fraction;
	cs->leafc_transfer      -= m_leafc_transfer_to_litr1c;
	cs_litr->litr1c         += m_frootc_transfer_to_litr1c * cover_fraction;
	cs->frootc_transfer     -= m_frootc_transfer_to_litr1c;
	cs_litr->litr1c         += m_livestemc_transfer_to_litr1c * cover_fraction;
	cs->livestemc_transfer  -= m_livestemc_transfer_to_litr1c;
	cs_litr->litr1c         += m_deadstemc_transfer_to_litr1c * cover_fraction;
	cs->deadstemc_transfer  -= m_deadstemc_transfer_to_litr1c;
	cs_litr->litr1c         += m_livecrootc_transfer_to_litr1c * cover_fraction;
	cs->livecrootc_transfer -= m_livecrootc_transfer_to_litr1c;
	cs_litr->litr1c         += m_deadcrootc_transfer_to_litr1c * cover_fraction;
	cs->deadcrootc_transfer -= m_deadcrootc_transfer_to_litr1c;
	cs_litr->litr1c         += m_gresp_store_to_litr1c * cover_fraction;
	cs->gresp_store       -= m_gresp_store_to_litr1c;
	cs_litr->litr1c         += m_gresp_transfer_to_litr1c * cover_fraction;
	cs->gresp_transfer      -= m_gresp_transfer_to_litr1c;
	if (epc.veg_type == TREE){
		/*    Stem wood mortality */
		cs->cwdc       += m_livestemc_to_cwdc;
		cs->live_stemc  -= m_livestemc_to_cwdc;
		cs->cwdc       += m_deadstemc_to_cwdc;
		cs->dead_stemc  -= m_deadstemc_to_cwdc;
		/* STEP 1e  Coarse root wood mortality */
		cs->cwdc       += m_livecrootc_to_cwdc;
		cs->live_crootc -= m_livecrootc_to_cwdc;
		cs->cwdc       += m_deadcrootc_to_cwdc;
		cs->dead_crootc -= m_deadcrootc_to_cwdc;
	}
	/* NITROGEN mortality state variable update */
	/*    Leaf mortality */
	ns_litr->litr1n         += m_npool * cover_fraction;
	ns->npool -= m_npool;
	ns_litr->litr1n    += m_leafn_to_litr1n * cover_fraction;
	ns->leafn          -= m_leafn_to_litr1n;
	ns_litr->litr2n    += m_leafn_to_litr2n * cover_fraction;
	ns->leafn          -= m_leafn_to_litr2n;
	ns_litr->litr3n    += m_leafn_to_litr3n * cover_fraction;
	ns->leafn          -= m_leafn_to_litr3n;
	ns_litr->litr4n    += m_leafn_to_litr4n * cover_fraction;
	ns->leafn          -= m_leafn_to_litr4n;
	/*    Fine root mortality */
	ns_litr->litr1n    += m_frootn_to_litr1n * cover_fraction;
	ns->frootn         -= m_frootn_to_litr1n;
	ns_litr->litr2n    += m_frootn_to_litr2n * cover_fraction;
	ns->frootn         -= m_frootn_to_litr2n;
	ns_litr->litr3n    += m_frootn_to_litr3n * cover_fraction;
	ns->frootn         -= m_frootn_to_litr3n;
	ns_litr->litr4n    += m_frootn_to_litr4n * cover_fraction;
	ns->frootn         -= m_frootn_to_litr4n;
	/*    Storage, transfer, excess, and npool mortality */
	ns_litr->litr1n         += m_leafn_store_to_litr1n * cover_fraction;
	ns->leafn_store       -= m_leafn_store_to_litr1n;
	ns_litr->litr1n         += m_frootn_store_to_litr1n * cover_fraction;
	ns->frootn_store      -= m_frootn_store_to_litr1n;
	ns_litr->litr1n         += m_livestemn_store_to_litr1n * cover_fraction;
	ns->livestemn_store   -= m_livestemn_store_to_litr1n;
	ns_litr->litr1n         += m_deadstemn_store_to_litr1n * cover_fraction;
	ns->deadstemn_store   -= m_deadstemn_store_to_litr1n;
	ns_litr->litr1n         += m_livecrootn_store_to_litr1n * cover_fraction;
	ns->livecrootn_store  -= m_livecrootn_store_to_litr1n;
	ns_litr->litr1n         += m_deadcrootn_store_to_litr1n * cover_fraction;
	ns->deadcrootn_store  -= m_deadcrootn_store_to_litr1n;
	ns_litr->litr1n         += m_leafn_transfer_to_litr1n * cover_fraction;
	ns->leafn_transfer      -= m_leafn_transfer_to_litr1n;
	ns_litr->litr1n         += m_frootn_transfer_to_litr1n * cover_fraction;
	ns->frootn_transfer     -= m_frootn_transfer_to_litr1n;
	ns_litr->litr1n         += m_livestemn_transfer_to_litr1n * cover_fraction;
	ns->livestemn_transfer  -= m_livestemn_transfer_to_litr1n;
	ns_litr->litr1n         += m_deadstemn_transfer_to_litr1n * cover_fraction;
	ns->deadstemn_transfer  -= m_deadstemn_transfer_to_litr1n;
	ns_litr->litr1n         += m_livecrootn_transfer_to_litr1n * cover_fraction;
	ns->livecrootn_transfer -= m_livecrootn_transfer_to_litr1n;
	ns_litr->litr1n         += m_deadcrootn_transfer_to_litr1n * cover_fraction;
	ns->deadcrootn_transfer -= m_deadcrootn_transfer_to_litr1n;
	ns_litr->litr1n         += m_retransn_to_litr1n * cover_fraction;
	ns->retransn            -= m_retransn_to_litr1n;
	if (epc.veg_type == TREE){
		/*    Stem wood mortality */
		ns_litr->litr1n     += m_livestemn_to_litr1n * cover_fraction;
		ns->live_stemn  -= m_livestemn_to_litr1n;
		ns->cwdn       += m_livestemn_to_cwdn;
		ns->live_stemn  -= m_livestemn_to_cwdn;
		ns->cwdn       += m_deadstemn_to_cwdn;
		ns->dead_stemn  -= m_deadstemn_to_cwdn;
		ns_litr->litr1n     += m_livecrootn_to_litr1n * cover_fraction;
		ns->live_crootn -= m_livecrootn_to_litr1n;
		ns->cwdn       += m_livecrootn_to_cwdn;
		ns->live_crootn -= m_livecrootn_to_cwdn;
		ns->cwdn       += m_deadcrootn_to_cwdn;
		ns->dead_crootn -= m_deadcrootn_to_cwdn;
	}
	return;
}/*end update_mortality*/

