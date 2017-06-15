/*--------------------------------------------------------------*/
/* 								*/
/*			canopy_stratum_growth		*/
/*								*/
/*	NAME							*/
/*	canopy_statrum_daily_growth 				*/
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	void canopy_stratum_growth 			*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	performs final end of day processing for the 		*/
/*	strata including CN allocations, growth respiration	*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void	canopy_stratum_growth(
							  struct	world_object		*world,
							  struct	basin_object		*basin,
							  struct	hillslope_object	*hillslope, 
							  struct	zone_object		*zone,
							  struct	patch_object		*patch,
							  struct 	canopy_strata_object 	*stratum,
							  struct 	command_line_object	*command_line,
							  struct	tec_entry		*event,
							  struct 	date 			current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	int	compute_growth_resp(	 int,
		struct epconst_struct,
		struct cdayflux_struct *);
	int 	update_C_stratum_daily(
		struct epconst_struct,
		struct cstate_struct *,
		struct cdayflux_struct *);
	int 	update_N_stratum_daily(
		struct epconst_struct,
		struct nstate_struct *,
		struct ndayflux_struct *,
		struct soil_n_object *);
	int	allocate_daily_growth(
		int,
		double,
		double,
		double,
		double,
		struct cdayflux_struct *,
		struct cstate_struct *,
		struct ndayflux_struct *,
		struct nstate_struct *,
		struct ndayflux_patch_struct *,
		struct	epvar_struct *,
		struct	epconst_struct,
		struct	date);
	int	allocate_annual_growth(
		int,
		int,
		int,
		double,
		double,
		struct epvar_struct *,
		struct cdayflux_struct *,
		struct cstate_struct *,
		struct ndayflux_struct *,
		struct nstate_struct *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *,
		struct	litter_c_object *,
		struct	litter_n_object *,
		struct	epconst_struct,
		struct	command_line_object *);

	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	struct cstate_struct *cs;
	struct nstate_struct *ns;
	double pnow;
	/*--------------------------------------------------------------*/
	/*	perform daily carbon and nitrogen allocations		*/
	/*--------------------------------------------------------------*/

	

	if (command_line[0].grow_flag > 0) {

		if (stratum[0].defaults[0][0].epc.dynamic_alloc_prop_day_growth == 1) 
			pnow = compute_prop_alloc_daily(
				stratum[0].phen.daily_allocation * stratum[0].defaults[0][0].epc.storage_transfer_prop,
				&(stratum[0].cs),
				stratum[0].defaults[0][0].epc);
		else
			pnow = stratum[0].phen.daily_allocation * stratum[0].defaults[0][0].epc.storage_transfer_prop;

		if (allocate_daily_growth(
			patch[0].soil_ns.nlimit,
			pnow,
			patch[0].Tsoil,
			patch[0].soil_cs.frootc,
			stratum[0].cover_fraction,
			&(stratum[0].cdf),
			&(stratum[0].cs),
			&(stratum[0].ndf),
			&(stratum[0].ns),
			&(patch[0].ndf),
			&(stratum[0].epv),
			stratum[0].defaults[0][0].epc,
			current_date) != 0){
			fprintf(stderr,"FATAL ERROR: in allocate_daily_growth");
			exit(EXIT_FAILURE);
			}
	}
	/*--------------------------------------------------------------*/
	/*	compute growth respiration (if grow option is on_)	*/
	/*--------------------------------------------------------------*/
	if (compute_growth_resp(
		stratum[0].phen.annual_allocation,
		stratum[0].defaults[0][0].epc,
		&(stratum[0].cdf)
		) != 0){
		fprintf(stderr,"FATAL ERROR: in compute_growth_resp");
		exit(EXIT_FAILURE);
	}

	/*--------------------------------------------------------------*/
	/*	allocate annual growth - once per year that will then 	*/
	/*	be expressed during leaf out				*/
	/*--------------------------------------------------------------*/
	if (command_line[0].grow_flag > 0) {
	if ( (stratum[0].phen.annual_allocation == 1) ){
		if (allocate_annual_growth(
			stratum[0].ID,
			stratum[0].defaults[0][0].ID,
			command_line[0].vmort_flag,
			stratum[0].cover_fraction,
			command_line[0].cpool_mort_fract,
			&(stratum[0].epv),
			&(stratum[0].cdf),
			&(stratum[0].cs),
			&(stratum[0].ndf),
			&(stratum[0].ns),
			&(patch[0].cdf),
			&(patch[0].ndf),
			&(patch[0].litter_cs),
			&(patch[0].litter_ns),
			stratum[0].defaults[0][0].epc,
			command_line) != 0){
			fprintf(stderr,"FATAL ERROR: in allocate_annual_growth");
			exit(EXIT_FAILURE);
		}
	}

	}
	/*--------------------------------------------------------------*/
	/*	update carbon state variables 				*/
	/*--------------------------------------------------------------*/
	if (command_line[0].grow_flag > 0) {
	if (update_C_stratum_daily(
		stratum[0].defaults[0][0].epc,
		&(stratum[0].cs),
		&(stratum[0].cdf))!= 0){
		fprintf(stderr,"FATAL ERROR: in update_C_stratum_daily");
		exit(EXIT_FAILURE);
	}

	/*--------------------------------------------------------------*/
	/*	update nitrogen state variables 				*/
	/*--------------------------------------------------------------*/
	if (update_N_stratum_daily(
		stratum[0].defaults[0][0].epc,
		&(stratum[0].ns),
		&(stratum[0].ndf),
		&(patch[0].soil_ns))!= 0){
		fprintf(stderr,"FATAL ERROR: in update_N_stratum_daily");
		exit(EXIT_FAILURE);
	}
	}
	else  {
		stratum[0].cs.net_psn = stratum[0].cdf.psn_to_cpool - stratum[0].cdf.total_mr - stratum[0].cdf.total_gr;
		
		if ( stratum[0].phen.annual_allocation == 1){
		stratum[0].cdf.leafc_store_to_leafc_transfer = stratum[0].cs.leafc_store;
		stratum[0].cs.leafc_transfer += stratum[0].cdf.leafc_store_to_leafc_transfer;
		stratum[0].cs.leafc_store -= stratum[0].cdf.leafc_store_to_leafc_transfer;
		stratum[0].ndf.leafn_store_to_leafn_transfer = stratum[0].ns.leafn_store;
		stratum[0].ns.leafn_transfer += stratum[0].ndf.leafn_store_to_leafn_transfer;
		stratum[0].ns.leafn_store -= stratum[0].ndf.leafn_store_to_leafn_transfer;
		}
	}


	cs = &(stratum[0].cs);
	stratum[0].cs.totalc = (cs->cpool + cs->cwdc + cs->dead_leafc
		+ cs->leafc + cs->leafc_store +  cs->leafc_transfer
		+ cs->gresp_transfer + cs->gresp_store
		+ cs->frootc + cs->frootc_store +  cs->frootc_transfer
		+ cs->live_stemc + cs->livestemc_store +  cs->livestemc_transfer
		+ cs->dead_stemc + cs->deadstemc_store +  cs->deadstemc_transfer
		+ cs->live_crootc + cs->livecrootc_store +  cs->livecrootc_transfer
		+ cs->dead_crootc + cs->deadcrootc_store +  cs->deadcrootc_transfer);
	ns = &(stratum[0].ns);
	stratum[0].ns.totaln = (ns->npool + ns->cwdn + ns->retransn + ns->dead_leafn
		+ ns->leafn + ns->leafn_store +  ns->leafn_transfer
		+ ns->frootn + ns->frootn_store +  ns->frootn_transfer
		+ ns->live_stemn + ns->livestemn_store +  ns->livestemn_transfer
		+ ns->dead_stemn + ns->deadstemn_store +  ns->deadstemn_transfer
		+ ns->live_crootn + ns->livecrootn_store +  ns->livecrootn_transfer
		+ ns->dead_crootn + ns->deadcrootn_store +  ns->deadcrootn_transfer);
	stratum[0].acc_month.lai += stratum[0].epv.proj_lai;
	return;
} /*end canopy_stratum_daily_growth.c*/
