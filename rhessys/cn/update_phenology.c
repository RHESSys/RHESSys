/*--------------------------------------------------------------*/
/*                                                              */ 
/*		update_phenology				*/
/*                                                              */
/*  NAME                                                        */
/*		update_phenology				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 	void update_phenology(										*/
/*			struct zone *										*/
/*			struct epv_struct *,		                        */
/*			struct epconst_struct,   		*/
/*                      struct phenology_struct *,              */
/*                      struct cstate_struct *,                 */
/*                      struct cdayflux_struct *,               */
/*                      struct cdayflux_patch_struct *,         */
/*                      struct nstate_struct *,                 */
/*                      struct ndayflux_struct *,               */
/*                      struct ndayflux_patch_struct *,         */
/*                      struct litter_c_object *,               */
/*                      struct litter_n_object *,               */
/*                      struct soil_c_object *,                 */
/*                      struct soil_n_object *,                 */
/*                      double,                                 */
/*			struct date current_date,		*/
/*		 	int	grow_flag);			*/
/*                                  				*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	performs seasonal leaf drop and budding   		*/
/*	updates annnual allocates during leaf out		*/
/*	computes leaf and fine root litfall			*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*	modifed from phenology and prephenology in		*/
/*	P.Thornton (1997) version of 1d_bgc			*/
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void update_phenology(struct zone_object  *zone,
					  struct epvar_struct	*epv ,
					  struct epconst_struct	epc	,
					  struct phenology_struct *phen,	
					  struct cstate_struct *cs,
					  struct cdayflux_struct	*cdf,
					  struct cdayflux_patch_struct *cdf_patch,
					  struct nstate_struct *ns,
					  struct ndayflux_struct	*ndf,
					  struct ndayflux_patch_struct *ndf_patch,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  struct litter_object *litter,
					  struct soil_c_object *cs_soil,
					  struct soil_n_object *ns_soil,
					  struct rooting_zone_object *rootzone,
					  double	effective_soil_depth,
					  double	cover_fraction,
					  double	gap_fraction,
					  double	theta_noon,
					  struct date current_date,
					  int	grow_flag)
{
	/*--------------------------------------------------------------*/
	/*  Local function declaration                                  */
	/*--------------------------------------------------------------*/
	long	yearday( struct date);
	int	update_rooting_depth(
		struct rooting_zone_object *,
		double,
		double,
		double,
		double);
	void	update_litter_interception_capacity (double, 
		double,
		struct litter_c_object *,
		struct litter_object *);
	int	compute_annual_litfall(
		struct epconst_struct,
		struct phenology_struct *,
		struct cstate_struct *,
		int);
	int     compute_cwd_decay(	struct epconst_struct *,
		double,
		struct cstate_struct *,
		struct nstate_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *,
		struct ndayflux_struct *);
	int	compute_deadleaf_turnover(
		struct epconst_struct,
		struct epvar_struct *,
		double,
		struct cstate_struct *,
		struct nstate_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *,
		int);
	int	compute_leaf_litfall(
		struct epconst_struct,
		double ,
		double ,
		struct cstate_struct *,
		struct nstate_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *,
		struct cdayflux_struct *,
		struct ndayflux_struct *,
		int);
	int	compute_froot_litfall(
		struct epconst_struct,
		double ,
		double ,
		struct cstate_struct *,
		struct nstate_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *);
	
	double compute_growingseason_index(
		struct zone_object *,
	 	struct epvar_struct	*epv ,
		struct epconst_struct
		);
		
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/

	int ok=1;
	long day;
	double perc_sunlit, leaflitfallc, frootlitfallc;
	double	rootc, sai, new_proj_lai_sunlit;
	double excess_n;
	int remdays_transfer;
	int expand_flag, litfall_flag;


	expand_flag = 0; 
	leaflitfallc = 0.0;
	frootlitfallc = 0.0;
	litfall_flag = 0;
	day = yearday(current_date);


 /*--------------------------------------------------------------*/
 /* static phenology                      */  
 /*--------------------------------------------------------------*/

	
  if (epc.phenology_flag == STATIC) {


	if (day == phen->expand_startday){ 
		expand_flag = 1;
		phen->gwseasonday = 1;
		phen->lfseasonday = -1;
	}

	else if (day == phen->litfall_startday){ 
		litfall_flag = 1;
		phen->lfseasonday = 1;
		phen->gwseasonday = -1;
	}

	else if (phen->gwseasonday > -1 && phen->gwseasonday <= epc.ndays_expand){ 
		expand_flag = 1;
	}

	else if (phen->lfseasonday > -1 && phen->lfseasonday <= epc.ndays_litfall){ 
		litfall_flag = 1;
	}

  }
	
 /*--------------------------------------------------------------*/
  /* dynamic phenology                      */  
  /*--------------------------------------------------------------*/

  else {

  phen->gsi = compute_growingseason_index(zone, epv, epc);
  
  


  /* first are we before last possible date for leaf onset */
    /* are we already in a leaf onset condition */
      if (phen->gwseasonday > -1 ) { 
          if  (phen->gwseasonday <= epc.ndays_expand)
              expand_flag=1;
          }   
      else if (phen->gsi > 0.5) {
              phen->gwseasonday = 1;
          phen->lfseasonday = -1; 
          expand_flag=1;
          phen->expand_startday = day;
          phen->expand_stopday = day + epc.ndays_expand;
          }   

/* now determine if we are before the last possible date of leaf drop */
     
      /* are we already in a leaf offset */
      if (phen->lfseasonday > -1 ) { 

          phen->gwseasonday = -1; 
     
          if  (phen->lfseasonday <= epc.ndays_litfall)
              litfall_flag=1;
          }
 
	else if ((phen->gsi < 0.5) && (phen->gwseasonday > epc.ndays_expand)){
                phen->lfseasonday = 1;
          phen->gwseasonday = -1; 
          litfall_flag=1;
          phen->litfall_startday = day;
          phen->litfall_stopday = day + epc.ndays_litfall;
	  }   



  } /* end dynamic phenology set up */
    

	
	phen->daily_allocation = epc.alloc_prop_day_growth;
	phen->annual_allocation = 0;
	if (((cs->frootc + cs->frootc_transfer + cs->frootc_store) < ZERO) && (epc.phenology_flag != STATIC)) {
		printf("\n calling annual allocation because fine roots are zero %lf %lf %lf", cs->frootc, cs->frootc_transfer, cs->frootc_store);
		phen->annual_allocation=1;} 
	/*--------------------------------------------------------------*/
	/*	Leaf expansion - spring leaf out			*/
	/*--------------------------------------------------------------*/

	if (expand_flag == 1) {

		remdays_transfer = max(1.0,(epc.ndays_expand + 1 - phen->gwseasonday));

	
		cdf->leafc_transfer_to_leafc = 2.0*cs->leafc_transfer / remdays_transfer;
		ndf->leafn_transfer_to_leafn = 2.0*ns->leafn_transfer / remdays_transfer;
		cdf->frootc_transfer_to_frootc=2.0*cs->frootc_transfer / remdays_transfer;
		ndf->frootn_transfer_to_frootn=2.0*ns->frootn_transfer / remdays_transfer;
		if (epc.veg_type == TREE) {
			cdf->livestemc_transfer_to_livestemc
				= 2.0*cs->livestemc_transfer / remdays_transfer;
			ndf->livestemn_transfer_to_livestemn
				= 2.0*ns->livestemn_transfer / remdays_transfer;
			cdf->deadstemc_transfer_to_deadstemc
				= 2.0*cs->deadstemc_transfer / remdays_transfer;
			ndf->deadstemn_transfer_to_deadstemn
				= 2.0*ns->deadstemn_transfer / remdays_transfer;
			cdf->livecrootc_transfer_to_livecrootc
				= 2.0*cs->livecrootc_transfer / remdays_transfer;
			ndf->livecrootn_transfer_to_livecrootn
				= 2.0*ns->livecrootn_transfer / remdays_transfer;
			cdf->deadcrootc_transfer_to_deadcrootc
				= 2.0*cs->deadcrootc_transfer / remdays_transfer;
			ndf->deadcrootn_transfer_to_deadcrootn
				= 2.0*ns->deadcrootn_transfer / remdays_transfer;

		}
	}
	/*--------------------------------------------------------------*/
	/*	Leaf drop - fall litter fall				*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/* at beginning of litter fall figure out how much to drop */
	/*--------------------------------------------------------------*/
	if (phen->lfseasonday == 1)  {
		ok = compute_annual_litfall(epc, phen, cs, grow_flag);
	}

	/*--------------------------------------------------------------*/
	/*	compute daily litter fall				*/
	/*--------------------------------------------------------------*/
	if (litfall_flag == 1) {
		remdays_transfer = max(1.0,(epc.ndays_litfall + 1 - phen->lfseasonday));

		leaflitfallc = 2.0*phen->leaflitfallc / remdays_transfer;
		if (leaflitfallc > cs->leafc)
			leaflitfallc = cs->leafc;
		frootlitfallc = 2.0*phen->frootlitfallc / remdays_transfer;
		if (frootlitfallc > cs->frootc)
			frootlitfallc = cs->frootc;
	}
	/*--------------------------------------------------------------*/
	/*	on the last day of litterfall make sure that deciduous no longer */
	/*	have any leaves left					*/
	/*	this is also considered to be the end of the growing season */
	/*	so do annual allcation					*/
	/*--------------------------------------------------------------*/
	if (phen->lfseasonday == epc.ndays_litfall){
		if (epc.phenology_type == DECID) {
			leaflitfallc = cs->leafc;
			phen->daily_allocation = 0;
			}
		phen->annual_allocation = 1;
	}


	/*--------------------------------------------------------------*/
	/*	update growth variables					*/
	/*		this is expression of allocation from 		*/
	/*		last seasons stored photosynthesis		*/
	/*	note all cdf and ndf variables are zero'd at the 	*/
	/*	start of the day, so only values set above are used	*/
	/*--------------------------------------------------------------*/
	 
	/* Leaf carbon transfer growth */
	cs->leafc            += cdf->leafc_transfer_to_leafc;
	cs->leafc_transfer   -= cdf->leafc_transfer_to_leafc;
	/* Leaf nitrogen transfer growth */
	ns->leafn           += ndf->leafn_transfer_to_leafn;
	ns->leafn_transfer  -= ndf->leafn_transfer_to_leafn;
	if (grow_flag > 0) {
		cs->frootc           += cdf->frootc_transfer_to_frootc;
		cs->frootc_transfer  -= cdf->frootc_transfer_to_frootc;
		ns->frootn          += ndf->frootn_transfer_to_frootn;
		ns->frootn_transfer -= ndf->frootn_transfer_to_frootn;
		if (epc.veg_type == TREE){
			/* Stem and coarse root transfer growth */
			cs->live_stemc             += cdf->livestemc_transfer_to_livestemc;
			cs->livestemc_transfer    -= cdf->livestemc_transfer_to_livestemc;
			cs->dead_stemc             += cdf->deadstemc_transfer_to_deadstemc;
			cs->deadstemc_transfer    -= cdf->deadstemc_transfer_to_deadstemc;
			cs->live_crootc            += cdf->livecrootc_transfer_to_livecrootc;
			cs->livecrootc_transfer   -= cdf->livecrootc_transfer_to_livecrootc;
			cs->dead_crootc            += cdf->deadcrootc_transfer_to_deadcrootc;
			cs->deadcrootc_transfer   -= cdf->deadcrootc_transfer_to_deadcrootc;
			/* nitrogen transfer */
			ns->live_stemn           += ndf->livestemn_transfer_to_livestemn;
			ns->livestemn_transfer  -= ndf->livestemn_transfer_to_livestemn;
			ns->dead_stemn           += ndf->deadstemn_transfer_to_deadstemn;
			ns->deadstemn_transfer  -= ndf->deadstemn_transfer_to_deadstemn;
			ns->live_crootn          += ndf->livecrootn_transfer_to_livecrootn;
			ns->livecrootn_transfer -= ndf->livecrootn_transfer_to_livecrootn;
			ns->dead_crootn          += ndf->deadcrootn_transfer_to_deadcrootn;
			ns->deadcrootn_transfer -= ndf->deadcrootn_transfer_to_deadcrootn;

		}
	}	/* end of grow processing */
	/*--------------------------------------------------------------*/
	/* check for leaf and fine root litfall for this day */
	/*--------------------------------------------------------------*/
	if ((leaflitfallc > ZERO ) && (cs->leafc > ZERO) && (ns->leafn > ZERO)){
		/*--------------------------------------------------------------*/
		/* set daily flux variables */
		/*--------------------------------------------------------------*/
		/*	compute leaf litter fall				*/
		/*--------------------------------------------------------------*/
		if (ok && compute_leaf_litfall(epc,
			leaflitfallc,cover_fraction,
			cs,ns, cs_litr,ns_litr,cdf_patch,ndf_patch, cdf,ndf, grow_flag)){
			fprintf(stderr,
				"FATAL ERROR: in leaf_litfall() from update_phenology()\n");
			exit(EXIT_FAILURE);
		}
		phen->leaflitfallc -= leaflitfallc;
		if (phen->leaflitfallc < 0)
			phen->leaflitfallc = 0;
	}

	/*--------------------------------------------------------------*/
	/*	add additional leaf litterfall if water stress conditions */
	/*	occur							*/ 
	/*	only drop when accumulated litterfall due to water stress */
	/*	is greater than usual litterfall			*/
	/*--------------------------------------------------------------*/
	/*
	 if (epv->psi < epc.psi_open)	{
		perc_leaflitfall = (1.0 / (epc.psi_close - epc.psi_open) *
				epv->psi + (-epc.psi_open) /
				(epc.psi_close - epc.psi_open)) / 100.0;
		leaflitfallc = (perc_leaflitfall * cs->leafc);
		phen->leaflitfallc_wstress += leaflitfallc;
		if ((phen->leaflitfallc_wstress > phen->leaflitfallc) && 
			(phen->leaflitfallc_wstress < 1.5 * phen->leaflitfallc)) {
			if (ok && compute_leaf_litfall(epc,
				leaflitfallc,cover_fraction,
				cs,ns, cs_litr,ns_litr,cdf_patch,ndf_patch, cdf,ndf, grow_flag)){
				fprintf(stderr,
					"FATAL ERROR: in leaf_litfall() from update_phenology()\n");
				exit(EXIT_FAILURE);
				}
		}
	}
	*/

	if ((frootlitfallc > 0.0)  && (grow_flag > 0) && (cs->frootc > ZERO) && (ns->frootn > ZERO)){
		/*--------------------------------------------------------------*/
		/*	compute fine root turnover				*/
		/*--------------------------------------------------------------*/
		if (ok && compute_froot_litfall(epc,frootlitfallc,
			cover_fraction, cs,ns,cs_litr,
			ns_litr,cdf_patch,ndf_patch)){
			fprintf(stderr,
				"FATAL ERROR: in froot_litfall() from update_phenology()\n");
			exit(EXIT_FAILURE);
		}
		phen->frootlitfallc -= frootlitfallc;
		if (phen->frootlitfallc < 0)
			phen->frootlitfallc = 0;
	}
	/*--------------------------------------------------------------*/
	/*	tree wood turnovers and dead standing grass turnovers	*/
	/*	- note this is not mortality but turnover rates		*/
	/*--------------------------------------------------------------*/

	if (((epc.veg_type == GRASS) || (epc.veg_type == C4GRASS)) && (grow_flag > 0) && (cs->dead_leafc > ZERO)) {
		if (ok && compute_deadleaf_turnover(epc,epv, cover_fraction, cs,ns,
			cs_litr,ns_litr,cdf_patch,ndf_patch,grow_flag)){
			fprintf(stderr,"FATAL ERROR: in compute_deadleaf_turnover() from update_phenology()\n");
			exit(EXIT_FAILURE);
		}
	}

	if ((epc.veg_type == TREE) && (grow_flag > 0)) {
		/*--------------------------------------------------------------*/
		/*	compute coarse woody debris fragmentation		*/
		/*--------------------------------------------------------------*/
		if ((cs->cwdc > ZERO) && (cover_fraction > ZERO)) {
			if (ok && compute_cwd_decay(&(epc),cover_fraction, cs,ns,cs_litr,
				ns_litr,cdf_patch,ndf_patch, ndf)){
				fprintf(stderr,
					"FATAL ERROR: in cwd_decay() from update_phenology()\n");
				exit(EXIT_FAILURE);
			}
		}
		/*--------------------------------------------------------------*/
		/*	compute live steam and coarse root turnover		*/
		/* 	excess n from live stem turnover is added to retranslocated N */
		/*    nambiar et al., (1991) tree physiology			*/
		/*--------------------------------------------------------------*/

		if (cs->live_stemc > ZERO) {
			cdf->livestemc_to_deadstemc = min(epv->day_livestem_turnover, cs->live_stemc);
			
			ndf->livestemn_to_deadstemn= min(cdf->livestemc_to_deadstemc 
						/ epc.livewood_cn, ns->live_stemn);
		
			excess_n = max(0.0, ndf->livestemn_to_deadstemn -
						(cdf->livestemc_to_deadstemc / epc.deadwood_cn ) );
			ns->retransn += excess_n;
			cs->live_stemc -= cdf->livestemc_to_deadstemc;
			cs->dead_stemc += cdf->livestemc_to_deadstemc;
			ns->live_stemn -= ndf->livestemn_to_deadstemn;
			ns->dead_stemn += (ndf->livestemn_to_deadstemn - excess_n);
		}
		if (cs->live_crootc > ZERO) {
			cdf->livecrootc_to_deadcrootc = min(epv->day_livecroot_turnover, cs->live_crootc);
			ndf->livecrootn_to_deadcrootn= min(cdf->livecrootc_to_deadcrootc 
						/ epc.livewood_cn, ns->live_crootn);
		
			excess_n = max(0.0, ndf->livecrootn_to_deadcrootn -
						(cdf->livecrootc_to_deadcrootc / epc.deadwood_cn ) );
			ns->retransn += excess_n;
			cs->live_crootc -= cdf->livecrootc_to_deadcrootc;
			cs->dead_crootc += cdf->livecrootc_to_deadcrootc;
			ns->live_crootn -= ndf->livecrootn_to_deadcrootn;
			ns->dead_crootn += (ndf->livecrootn_to_deadcrootn - excess_n);
		}

	} /* end tree processing */
	if (grow_flag == 0) { /* non-grow processing */
		/*--------------------------------------------------------------*/
		/* update state variables assumes no retranslocation */
		/*	this is done in literfall routine			*/
		/*--------------------------------------------------------------*/
		/*
		cs->leafc        -= leaflitfallc;
		cs->leafc_store  += leaflitfallc;
		ns->leafn        -= leaflitfallc / epc.leaf_cn;
		ns->leafn_store  += leaflitfallc / epc.leaf_cn;
		*/
	}
	/*--------------------------------------------------------------*/
	/* check for rounding errors on end of litfall season */
	/*--------------------------------------------------------------*/
	if (fabs(cs->leafc) <= 1e-13){
		cs->leafc = 0.0;
		ns->leafn = 0.0;
	}
	if (fabs(cs->frootc) <= 1e-13){
		cs->frootc = 0.0;
		ns->frootn = 0.0;
	}



	/*--------------------------------------------------------------*/
	/*	compute new rooting depth based on current root carbon  */
	/*--------------------------------------------------------------*/
	rootc = cs->frootc+cs->live_crootc+cs->dead_crootc;
	if ((grow_flag > 0) && (rootc > ZERO)){
		if (ok && update_rooting_depth(
			rootzone, rootc, epc.root_growth_direction, epc.root_distrib_parm,
			effective_soil_depth)){
			fprintf(stderr,
				"FATAL ERROR: in compute_rooting_depth() from update_phenology()\n");
			exit(EXIT_FAILURE);
		}
	}

	/*--------------------------------------------------------------*/
	/* now figure out a sunlit and shaded flux density		*/
	/* use Chen et al. 1999 Ecological Modelling 124: 99-119	*/
	/* to estimate shaded and sunlit fractions			*/
	/* then update lai based on sunlit/shaded sla			*/
	/* we need to do a predictor-corrector type convergence here	*/
	/*	since sunlit/shaded fraction depend on total proj_lai	*/
	/*--------------------------------------------------------------*/

	perc_sunlit = 0.0;
	if ((cs->leafc > ZERO) && (epc.veg_type != NON_VEG)) {
		epv->proj_lai = max((cs->leafc * (epv->proj_sla_sunlit * perc_sunlit + 
			epv->proj_sla_shade * (1-perc_sunlit))), 0.0);
		new_proj_lai_sunlit = 2.0 * cos(theta_noon) *
				(1.0 - exp(-0.5*(1-gap_fraction)*
				epv->proj_lai / cos(theta_noon)));
		while (fabs(epv->proj_lai_sunlit - new_proj_lai_sunlit) > 0.00001*new_proj_lai_sunlit )  {
			epv->proj_lai_sunlit = new_proj_lai_sunlit;
			epv->proj_lai_shade = epv->proj_lai - epv->proj_lai_sunlit;
			if ((epv->proj_lai_sunlit + epv->proj_lai_shade) > ZERO)
				perc_sunlit = (epv->proj_lai_sunlit) / (epv->proj_lai_sunlit + epv->proj_lai_shade);
			else
				perc_sunlit = 1.0;
			epv->proj_lai = max((cs->leafc * (epv->proj_sla_sunlit * perc_sunlit + 
				epv->proj_sla_shade * (1-perc_sunlit))), 0.0);
			new_proj_lai_sunlit = 2.0 * cos(theta_noon) *
					(1.0 - exp(-0.5*(1-gap_fraction)*
					epv->proj_lai / cos(theta_noon)));
			}
	}
	else {
		epv->proj_lai = 0.0;
		epv->proj_lai_sunlit = 0.0;
		epv->proj_lai_shade = 0.0;
	}


	/*--------------------------------------------------------------*/
	/* update lai based on sla			*/
	/* use sunlit sla for lai up to 1 and shaded sla for lai above that */
	/*--------------------------------------------------------------*/
	if ((epv->proj_lai_sunlit + epv->proj_lai_shade) > ZERO)
		perc_sunlit = (epv->proj_lai_sunlit) / (epv->proj_lai_sunlit + epv->proj_lai_shade);
	else
		perc_sunlit = 1.0;

	epv->all_lai = epv->proj_lai * epc.lai_ratio;

	if (epc.veg_type == TREE)  {
		sai = epc.proj_swa*(1.0-exp(-0.175*(cs->live_stemc+cs->dead_stemc)));
		epv->proj_pai = max(epv->proj_lai + sai, 0.0);
		epv->all_pai = max(epv->all_lai + sai, 0.0);
	}
	else {
		epv->proj_pai = epv->proj_lai;
		epv->all_pai = epv->all_lai;
	}
	/*--------------------------------------------------------------*/
	/*	update height						*/
	/*--------------------------------------------------------------*/
	if (epc.veg_type == TREE)
		if ( (cs->live_stemc + cs->dead_stemc) > ZERO)
			epv->height = epc.height_to_stem_coef
				* pow ( (cs->live_stemc + cs->dead_stemc), epc.height_to_stem_exp);
		else
			epv->height = 0.0;
	else
		if (epc.veg_type == NON_VEG) {
			epv->height = 0.0;
			}
		else {	if (cs->leafc > ZERO)
				epv->height = epc.height_to_stem_coef
					* pow ( (cs->leafc), epc.height_to_stem_exp);
			else
				epv->height = 0.0;
			}
	/*--------------------------------------------------------------*/
	/*	keep a seasonal max_lai for outputing purposes		*/
	/*--------------------------------------------------------------*/
	if (phen->gwseasonday == 1){
		epv->max_proj_lai = 0.0;
	}
	if (phen->gwseasonday > 1){
		if (epv->proj_lai > epv->max_proj_lai){
			epv->max_proj_lai = epv->proj_lai;
		}
	}
	/*--------------------------------------------------------------*/
	/*	update litter interception capacity			*/
	/*--------------------------------------------------------------*/
	update_litter_interception_capacity(
		litter->moist_coef,
		litter->density,	
		cs_litr,
		litter);



	/* Advances seasonday variables */
	  if (phen->gwseasonday > 0)
	      phen->gwseasonday += 1;
	  if (phen->lfseasonday > 0)
	      phen->lfseasonday += 1;
	

	return;

}/*end update phenology*/
