
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
					  int thintyp,
					  struct mortality_struct mort)
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
	double m_deadleafc_to_litr1c, m_deadleafc_to_litr2c;
	double m_deadleafc_to_litr3c, m_deadleafc_to_litr4c;
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
	double m_deadleafn_to_litr1n, m_deadleafn_to_litr2n;
	double m_deadleafn_to_litr3n, m_deadleafn_to_litr4n;
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
	m_cpool = mort.mort_cpool * cs->cpool;
	m_npool = mort.mort_cpool * ns->npool;
	
	m_leafc_to_litr1c = mort.mort_leafc * cs->leafc * epc.leaflitr_flab;
	m_leafc_to_litr2c = mort.mort_leafc * cs->leafc * epc.leaflitr_fucel;
	m_leafc_to_litr3c = mort.mort_leafc * cs->leafc * epc.leaflitr_fscel;
	m_leafc_to_litr4c = mort.mort_leafc * cs->leafc * epc.leaflitr_flig;
	m_deadleafc_to_litr1c = mort.mort_deadleafc * cs->dead_leafc * epc.leaflitr_flab;
	m_deadleafc_to_litr2c = mort.mort_deadleafc * cs->dead_leafc * epc.leaflitr_fucel;
	m_deadleafc_to_litr3c = mort.mort_deadleafc * cs->dead_leafc * epc.leaflitr_fscel;
	m_deadleafc_to_litr4c = mort.mort_deadleafc * cs->dead_leafc * epc.leaflitr_flig;	
	m_frootc_to_litr1c = mort.mort_frootc * cs->frootc * epc.frootlitr_flab;
	m_frootc_to_litr2c = mort.mort_frootc * cs->frootc * epc.frootlitr_fucel;
	m_frootc_to_litr3c = mort.mort_frootc * cs->frootc * epc.frootlitr_fscel;
	m_frootc_to_litr4c = mort.mort_frootc * cs->frootc * epc.frootlitr_flig;
	/* mortality fluxes out of storage and transfer pools */
	/* Assumes cpool mortality fraction applies to all non-structural stores and transfers */
	m_leafc_store_to_litr1c  = mort.mort_cpool * cs->leafc_store;
	m_frootc_store_to_litr1c  = mort.mort_cpool * cs->frootc_store;
	m_leafc_transfer_to_litr1c = mort.mort_cpool * cs->leafc_transfer;
	m_frootc_transfer_to_litr1c = mort.mort_cpool * cs->frootc_transfer;
	m_gresp_store_to_litr1c = mort.mort_cpool * cs->gresp_store;
	m_gresp_transfer_to_litr1c = mort.mort_cpool * cs->gresp_transfer;
	/* TREE-specific carbon fluxes */
	if (epc.veg_type==TREE){
		m_livestemc_to_cwdc = mort.mort_livestemc * cs->live_stemc;
		m_deadstemc_to_cwdc = mort.mort_deadstemc * cs->dead_stemc;
		m_livecrootc_to_cwdc = mort.mort_livecrootc * cs->live_crootc;
		m_deadcrootc_to_cwdc = mort.mort_deadcrootc * cs->dead_crootc;
		/* Assumes cpool mortality fraction applies to all non-structural stores and transfers */
		m_livestemc_store_to_litr1c  = mort.mort_cpool * cs->livestemc_store;
		m_deadstemc_store_to_litr1c  = mort.mort_cpool * cs->deadstemc_store;
		m_livecrootc_store_to_litr1c  = mort.mort_cpool * cs->livecrootc_store;
		m_deadcrootc_store_to_litr1c  = mort.mort_cpool * cs->deadcrootc_store;
		m_livestemc_transfer_to_litr1c = mort.mort_cpool * cs->livestemc_transfer;
		m_deadstemc_transfer_to_litr1c = mort.mort_cpool * cs->deadstemc_transfer;
		m_livecrootc_transfer_to_litr1c = mort.mort_cpool * cs->livecrootc_transfer;
		m_deadcrootc_transfer_to_litr1c = mort.mort_cpool * cs->deadcrootc_transfer;
	}
	
	/* daily nitrogen fluxes due to mortality */
	/* mortality fluxes out of leaf and fine root pools */
	if (epc.leaf_cn > ZERO) {
		m_leafn_to_litr2n = m_leafc_to_litr2c / CEL_CN;
		m_leafn_to_litr3n = m_leafc_to_litr3c / CEL_CN;
		m_leafn_to_litr4n = m_leafc_to_litr4c / LIG_CN;
		m_leafn_to_litr1n = mort.mort_leafc*ns->leafn - (m_leafn_to_litr2n+m_leafn_to_litr3n+m_leafn_to_litr4n);
		m_leafn_to_litr1n = max(m_leafn_to_litr1n, 0.0);
		}
	else {
		m_leafn_to_litr1n = 0.0;
		m_leafn_to_litr2n = 0.0;
		m_leafn_to_litr3n = 0.0;
		m_leafn_to_litr4n = 0.0;
		}
	if (epc.leaflitr_cn > ZERO) {
		m_deadleafn_to_litr2n = m_deadleafc_to_litr2c / CEL_CN;
		m_deadleafn_to_litr3n = m_deadleafc_to_litr3c / CEL_CN;
		m_deadleafn_to_litr4n = m_deadleafc_to_litr4c / LIG_CN;
		m_deadleafn_to_litr1n = mort.mort_deadleafc*ns->dead_leafn - (m_deadleafn_to_litr2n+m_deadleafn_to_litr3n+m_deadleafn_to_litr4n);
		m_deadleafn_to_litr1n = max(m_deadleafn_to_litr1n, 0.0);
	}
	else {
		m_deadleafn_to_litr1n = 0.0;
		m_deadleafn_to_litr2n = 0.0;
		m_deadleafn_to_litr3n = 0.0;
		m_deadleafn_to_litr4n = 0.0;
	}	
	if (epc.froot_cn > ZERO) {
		m_frootn_to_litr2n = m_frootc_to_litr2c / CEL_CN;
		m_frootn_to_litr3n = m_frootc_to_litr3c / CEL_CN;
		m_frootn_to_litr4n = m_frootc_to_litr4c / LIG_CN;
		m_frootn_to_litr1n = mort.mort_frootc*ns->frootn - (m_frootn_to_litr2n+m_frootn_to_litr3n+m_frootn_to_litr4n);
		m_frootn_to_litr1n = max(m_frootn_to_litr1n, 0.0);
		}
	else {
		m_frootn_to_litr1n = 0.0;
		m_frootn_to_litr2n = 0.0;
		m_frootn_to_litr3n = 0.0;
		m_frootn_to_litr4n = 0.0;		
		}

	/* mortality fluxes out of storage and transfer pools */
	/* Assumes same mortality fractions as for c pools */
	/* Assumes cpool mortality fraction applies to all non-structural stores and transfers */
	m_leafn_store_to_litr1n  = mort.mort_cpool * ns->leafn_store;
	m_frootn_store_to_litr1n  = mort.mort_cpool * ns->frootn_store;
	m_leafn_transfer_to_litr1n = mort.mort_cpool * ns->leafn_transfer;
	m_frootn_transfer_to_litr1n = mort.mort_cpool * ns->frootn_transfer;
	m_retransn_to_litr1n = mort.mort_cpool * ns->retransn;

	/* TREE-specific nitrogen fluxes */
	if (epc.veg_type==TREE){
		m_livestemn_to_cwdn = m_livestemc_to_cwdc / epc.deadwood_cn;
		m_livecrootn_to_cwdn = m_livecrootc_to_cwdc / epc.deadwood_cn;
		/* Assumes same mortality fractions as for c pools */
		m_livestemn_to_litr1n = (mort.mort_livestemc * ns->live_stemn) - m_livestemn_to_cwdn;
		m_livecrootn_to_litr1n = (mort.mort_livecrootc * ns->live_crootn) - m_livecrootn_to_cwdn;
		m_deadstemn_to_cwdn = mort.mort_deadstemc * ns->dead_stemn;
		m_deadcrootn_to_cwdn = mort.mort_deadcrootc * ns->dead_crootn;
		/* Assumes cpool mortality fraction applies to all non-structural stores and transfers */
		m_livestemn_store_to_litr1n  = mort.mort_cpool * ns->livestemn_store;
		m_deadstemn_store_to_litr1n  = mort.mort_cpool * ns->deadstemn_store;
		m_livecrootn_store_to_litr1n  = mort.mort_cpool * ns->livecrootn_store;
		m_deadcrootn_store_to_litr1n  = mort.mort_cpool * ns->deadcrootn_store;
		m_livestemn_transfer_to_litr1n = mort.mort_cpool * ns->livestemn_transfer;
		m_deadstemn_transfer_to_litr1n = mort.mort_cpool * ns->deadstemn_transfer;
		m_livecrootn_transfer_to_litr1n = mort.mort_cpool * ns->livecrootn_transfer;
		m_deadcrootn_transfer_to_litr1n = mort.mort_cpool * ns->deadcrootn_transfer;		
	}
	
	/* update state variables */
	
	/* ---------------------------------------- */
	/* CARBON mortality state variable update   */
	/* ---------------------------------------- */
	
	/* ABOVEGROUND C POOLS */
	
	/* Only add dead leaf and stem c to litter and cwd pools if thintyp   */
	/* is not 2 (harvest case). If thintyp is 2, harvest aboveground c.   */
	if (thintyp != 2) {
		cs_litr->litr1c    += m_cpool;
		/*    Leaf mortality */
		cs_litr->litr1c    += m_leafc_to_litr1c;
		cs_litr->litr2c    += m_leafc_to_litr2c;
		cs_litr->litr3c    += m_leafc_to_litr3c;
		cs_litr->litr4c    += m_leafc_to_litr4c;
		cs_litr->litr1c    += m_deadleafc_to_litr1c;
		cs_litr->litr2c    += m_deadleafc_to_litr2c;
		cs_litr->litr3c    += m_deadleafc_to_litr3c;
		cs_litr->litr4c    += m_deadleafc_to_litr4c;		
		cs_litr->litr1c    += m_leafc_store_to_litr1c;
		cs_litr->litr1c    += m_leafc_transfer_to_litr1c;
		if (epc.veg_type == TREE) {
			/*    Stem wood mortality */
			/*	  Transfer to DEADWOOD if standing dead */
			if (thintyp == 3) {
				cs->dead_stemc       += m_livestemc_to_cwdc;
				cs->dead_stemc       += m_deadstemc_to_cwdc;
				}
			/*	  Transfer to CWD otherwise */
			else {
				cs->cwdc       += m_livestemc_to_cwdc;
				cs->cwdc       += m_deadstemc_to_cwdc;
				}
			cs_litr->litr1c    += m_livestemc_store_to_litr1c;
			cs_litr->litr1c    += m_deadstemc_store_to_litr1c;
			cs_litr->litr1c    += m_livestemc_transfer_to_litr1c;
			cs_litr->litr1c    += m_deadstemc_transfer_to_litr1c;			
			}
		/* gresp... group in with aboveground? */
		cs_litr->litr1c         += m_gresp_store_to_litr1c;
		cs_litr->litr1c         += m_gresp_transfer_to_litr1c;
		}
	/* Remove aboveground dead c from carbon stores in all cases. */
	cs->cpool -= m_cpool;
	/*    Leaf mortality */
	cs->leafc          -= m_leafc_to_litr1c;
	cs->leafc          -= m_leafc_to_litr2c;
	cs->leafc          -= m_leafc_to_litr3c;
	cs->leafc          -= m_leafc_to_litr4c;
	cs->dead_leafc          -= m_deadleafc_to_litr1c;
	cs->dead_leafc          -= m_deadleafc_to_litr2c;
	cs->dead_leafc          -= m_deadleafc_to_litr3c;
	cs->dead_leafc          -= m_deadleafc_to_litr4c;	
	cs->leafc_store       -= m_leafc_store_to_litr1c;
	cs->leafc_transfer      -= m_leafc_transfer_to_litr1c;
	if (epc.veg_type == TREE){
		/*    Stem wood mortality */
		cs->live_stemc  -= m_livestemc_to_cwdc;
		cs->dead_stemc  -= m_deadstemc_to_cwdc;
		cs->livestemc_store   -= m_livestemc_store_to_litr1c;
		cs->deadstemc_store   -= m_deadstemc_store_to_litr1c;
		cs->livestemc_transfer  -= m_livestemc_transfer_to_litr1c;
		cs->deadstemc_transfer  -= m_deadstemc_transfer_to_litr1c;	
		}
	/* gresp... group in with aboveground? */
	cs->gresp_store       -= m_gresp_store_to_litr1c;
	cs->gresp_transfer      -= m_gresp_transfer_to_litr1c;
	
	/* BELOWGROUND C POOLS */
	/* Belowground dead c goes to litter and cwd in all cases. */
	/*   Fine root mortality */
	cs_litr->litr1c    += m_frootc_to_litr1c;
	cs->frootc         -= m_frootc_to_litr1c;
	cs_litr->litr2c    += m_frootc_to_litr2c;
	cs->frootc         -= m_frootc_to_litr2c;
	cs_litr->litr3c    += m_frootc_to_litr3c;
	cs->frootc         -= m_frootc_to_litr3c;
	cs_litr->litr4c    += m_frootc_to_litr4c;
	cs->frootc         -= m_frootc_to_litr4c;
	cs_litr->litr1c    += m_frootc_store_to_litr1c;
	cs->frootc_store   -= m_frootc_store_to_litr1c;
	cs_litr->litr1c         += m_frootc_transfer_to_litr1c;
	cs->frootc_transfer     -= m_frootc_transfer_to_litr1c;
	if (epc.veg_type == TREE){
		/* Coarse root wood mortality */
		cs->cwdc       += m_livecrootc_to_cwdc;
		cs->live_crootc -= m_livecrootc_to_cwdc;
		cs->cwdc       += m_deadcrootc_to_cwdc;
		cs->dead_crootc -= m_deadcrootc_to_cwdc;		
		cs_litr->litr1c       += m_livecrootc_store_to_litr1c;
		cs->livecrootc_store  -= m_livecrootc_store_to_litr1c;
		cs_litr->litr1c       += m_deadcrootc_store_to_litr1c;
		cs->deadcrootc_store  -= m_deadcrootc_store_to_litr1c;
		cs_litr->litr1c         += m_livecrootc_transfer_to_litr1c;
		cs->livecrootc_transfer -= m_livecrootc_transfer_to_litr1c;
		cs_litr->litr1c         += m_deadcrootc_transfer_to_litr1c;
		cs->deadcrootc_transfer -= m_deadcrootc_transfer_to_litr1c;
		}
	
	/* ---------------------------------------- */
	/* NITROGEN mortality state variable update */
	/* ---------------------------------------- */
	
	/* ABOVEGROUND N POOLS */
	
	/* Only add dead leaf and stem n to litter and cwd pools if thintyp   */
	/* is not 2 (harvest case). If thintyp is 2, harvest aboveground n.   */
	if (thintyp != 2) {
		ns_litr->litr1n         += m_npool;
		/*    Leaf mortality */		
		ns_litr->litr1n    += m_leafn_to_litr1n;
		ns_litr->litr2n    += m_leafn_to_litr2n;
		ns_litr->litr3n    += m_leafn_to_litr3n;
		ns_litr->litr4n    += m_leafn_to_litr4n;
		ns_litr->litr1n    += m_deadleafn_to_litr1n;
		ns_litr->litr2n    += m_deadleafn_to_litr2n;
		ns_litr->litr3n    += m_deadleafn_to_litr3n;
		ns_litr->litr4n    += m_deadleafn_to_litr4n;		
		ns_litr->litr1n    += m_leafn_store_to_litr1n;
		ns_litr->litr1n    += m_leafn_transfer_to_litr1n;
		ns_litr->litr1n    += m_retransn_to_litr1n;
		if (epc.veg_type == TREE){
		/*    Stem wood mortality */
			ns_litr->litr1n     += m_livestemn_to_litr1n;
			/*	  Transfer to CWD if normal thinning */
			if (thintyp != 3) {
				ns->cwdn       += m_livestemn_to_cwdn;
				ns->cwdn       += m_deadstemn_to_cwdn;
				}
			/*	  Transfer to DEADWOOD if standing dead */
			else {
				ns->dead_stemn       += m_livestemn_to_cwdn;
				ns->dead_stemn       += m_deadstemn_to_cwdn;
				}
			ns_litr->litr1n    += m_livestemn_store_to_litr1n;
			ns_litr->litr1n    += m_deadstemn_store_to_litr1n;
			ns_litr->litr1n    += m_livestemn_transfer_to_litr1n;
			ns_litr->litr1n    += m_deadstemn_transfer_to_litr1n;
			}
		}
	/* Remove aboveground dead n from n stores in all cases. */
	ns->npool -= m_npool;
	/*    Leaf mortality */
	ns->leafn          -= m_leafn_to_litr1n;
	ns->leafn          -= m_leafn_to_litr2n;
	ns->leafn          -= m_leafn_to_litr3n;
	ns->leafn          -= m_leafn_to_litr4n;
	ns->dead_leafn          -= m_deadleafn_to_litr1n;
	ns->dead_leafn          -= m_deadleafn_to_litr2n;
	ns->dead_leafn          -= m_deadleafn_to_litr3n;
	ns->dead_leafn          -= m_deadleafn_to_litr4n;	
	ns->leafn_store       -= m_leafn_store_to_litr1n;
	ns->leafn_transfer      -= m_leafn_transfer_to_litr1n;
	ns->retransn            -= m_retransn_to_litr1n;
	if (epc.veg_type == TREE){
		/*    Stem wood mortality */
		ns->live_stemn  -= m_livestemn_to_litr1n;
		ns->live_stemn  -= m_livestemn_to_cwdn;
		ns->dead_stemn  -= m_deadstemn_to_cwdn;
		ns->livestemn_store   -= m_livestemn_store_to_litr1n;
		ns->deadstemn_store   -= m_deadstemn_store_to_litr1n;
		ns->livestemn_transfer  -= m_livestemn_transfer_to_litr1n;
		ns->deadstemn_transfer  -= m_deadstemn_transfer_to_litr1n;
		}

	/* BELOWGROUND N POOLS */
	/* Belowground dead n goes to litter and cwd in all cases. */
	/*   Fine root mortality */
	ns_litr->litr1n    += m_frootn_to_litr1n;
	ns->frootn         -= m_frootn_to_litr1n;
	ns_litr->litr2n    += m_frootn_to_litr2n;
	ns->frootn         -= m_frootn_to_litr2n;
	ns_litr->litr3n    += m_frootn_to_litr3n;
	ns->frootn         -= m_frootn_to_litr3n;
	ns_litr->litr4n    += m_frootn_to_litr4n;
	ns->frootn         -= m_frootn_to_litr4n;
	ns_litr->litr1n         += m_frootn_store_to_litr1n;
	ns->frootn_store      -= m_frootn_store_to_litr1n;
	ns_litr->litr1n         += m_frootn_transfer_to_litr1n;
	ns->frootn_transfer     -= m_frootn_transfer_to_litr1n;
	if (epc.veg_type == TREE){
		/* Coarse root mortality */
		ns_litr->litr1n     += m_livecrootn_to_litr1n;
		ns->live_crootn -= m_livecrootn_to_litr1n;
		ns->cwdn       += m_livecrootn_to_cwdn;
		ns->live_crootn -= m_livecrootn_to_cwdn;
		ns->cwdn       += m_deadcrootn_to_cwdn;
		ns->dead_crootn -= m_deadcrootn_to_cwdn;
		ns_litr->litr1n         += m_livecrootn_store_to_litr1n;
		ns->livecrootn_store  -= m_livecrootn_store_to_litr1n;
		ns_litr->litr1n         += m_deadcrootn_store_to_litr1n;
		ns->deadcrootn_store  -= m_deadcrootn_store_to_litr1n;
		ns_litr->litr1n         += m_livecrootn_transfer_to_litr1n;
		ns->livecrootn_transfer -= m_livecrootn_transfer_to_litr1n;
		ns_litr->litr1n         += m_deadcrootn_transfer_to_litr1n;
		ns->deadcrootn_transfer -= m_deadcrootn_transfer_to_litr1n;		
	}
	return;
}/*end update_mortality*/

