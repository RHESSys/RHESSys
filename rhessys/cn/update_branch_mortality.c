/*--------------------------------------------------------------*/
/*                                                              */
/*		update_branch_mortality									*/
/*                                                              */
/*  NAME                                                        */
/*		update_branch_mortality									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 	void update_branch_mortality(
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
void update_branch_mortality(
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
	double m_livestemc_store_to_litr1c;
	double m_deadstemc_store_to_litr1c;
	double m_livecrootc_store_to_litr1c_bg; // coarse root store to below ground litter
	double m_deadcrootc_store_to_litr1c_bg; // to below ground
	double m_livestemc_transfer_to_litr1c;
	double m_deadstemc_transfer_to_litr1c;
	double m_livecrootc_transfer_to_litr1c_bg; // to below ground litter pool
	double m_deadcrootc_transfer_to_litr1c_bg; // to below ground litter pool
	double m_livestemn_to_litr1n, m_livecrootn_to_litr1n_bg;
	double m_livestemn_store_to_litr1n;
	double m_deadstemn_store_to_litr1n;
	double m_livecrootn_store_to_litr1n_bg; // to below ground
	double m_deadcrootn_store_to_litr1n_bg; // to below ground
	double m_livestemn_transfer_to_litr1n;
	double m_deadstemn_transfer_to_litr1n;
	double m_livecrootn_transfer_to_litr1n_bg; // to below ground
	double m_deadcrootn_transfer_to_litr1n_bg;
	double m_livestemc_to_cwdc;
	double m_deadstemc_to_cwdc;
	double m_livecrootc_to_cwdc_bg; // to below ground
	double m_deadcrootc_to_cwdc_bg;
	double m_livestemn_to_cwdn;
	double m_deadstemn_to_cwdn;
	double m_livecrootn_to_cwdn_bg; // to below ground
	double m_deadcrootn_to_cwdn_bg;
	/******************************************************************/
	/* Non-fire mortality: these fluxes all enter litter or CWD pools */
	/******************************************************************/
	/* daily carbon fluxes due to mortality */
	/* TREE-specific carbon fluxes */

	if (epc.veg_type==TREE){
		m_livestemc_to_cwdc = mort * cs->live_stemc;
		m_deadstemc_to_cwdc = mort * cs->dead_stemc;
		m_livecrootc_to_cwdc_bg = mort * cs->live_crootc; //croot
		m_deadcrootc_to_cwdc_bg = mort * cs->dead_crootc; //croot
	/* daily nitrogen fluxes due to mortality */
	/*  nitrogen fluxes */
		m_livestemn_to_cwdn = m_livestemc_to_cwdc / epc.deadwood_cn;
		m_livecrootn_to_cwdn_bg = m_livecrootc_to_cwdc_bg / epc.deadwood_cn; //croot
		m_livestemn_to_litr1n
			= (mort * ns->live_stemn) - m_livestemn_to_cwdn;
		m_livecrootn_to_litr1n_bg
			= (mort * ns->live_crootn) - m_livecrootn_to_cwdn_bg; //croot
		m_deadstemn_to_cwdn = mort * ns->dead_stemn;
		m_deadcrootn_to_cwdn_bg = mort * ns->dead_crootn; //croot
	/* update state variables */
	/* CARBON mortality state variable update */
		/*    Stem wood mortality */
		cs->cwdc       += m_livestemc_to_cwdc;
		cs->live_stemc  -= m_livestemc_to_cwdc;
		cs->cwdc       += m_deadstemc_to_cwdc;
		cs->dead_stemc  -= m_deadstemc_to_cwdc;
		/* STEP 1e  Coarse root wood mortality */
		cs->cwdc_bg       += m_livecrootc_to_cwdc_bg; //below ground cwdc
		cs->live_crootc -= m_livecrootc_to_cwdc_bg;
		cs->cwdc_bg       += m_deadcrootc_to_cwdc_bg;
		cs->dead_crootc -= m_deadcrootc_to_cwdc_bg;
	/* NITROGEN mortality state variable update */
		/*    Stem wood mortality */
		ns_litr->litr1n     += m_livestemn_to_litr1n * cover_fraction;
		ns->live_stemn  -= m_livestemn_to_litr1n;
		ns->cwdn       += m_livestemn_to_cwdn;
		ns->live_stemn  -= m_livestemn_to_cwdn;
		ns->cwdn       += m_deadstemn_to_cwdn;
		ns->dead_stemn  -= m_deadstemn_to_cwdn;

    /* below ground root    */
		ns_litr->litr1n_bg     += m_livecrootn_to_litr1n_bg * cover_fraction;
		ns->live_crootn -= m_livecrootn_to_litr1n_bg;
		ns->cwdn_bg       += m_livecrootn_to_cwdn_bg;
		ns->live_crootn -= m_livecrootn_to_cwdn_bg;
		ns->cwdn_bg       += m_deadcrootn_to_cwdn_bg;
		ns->dead_crootn -= m_deadcrootn_to_cwdn_bg;

	} /* end if TREE - if not tree this routine does nothing */

	return;
}/*end update_branch_mortality*/

