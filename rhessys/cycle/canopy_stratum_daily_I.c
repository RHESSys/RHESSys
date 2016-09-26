/*--------------------------------------------------------------*/
/*								*/
/*		canopy_stratum.I				*/
/*								*/
/*	NAME							*/
/*								*/
/*	SYNOPSIS						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/* March 12, 1997	C. Tague 	*/
/* added seasonal leafon/off adjustment routine  */
/*								*/
/*	Sep 15 97 RAF						*/
/*	Took out call to compute lwp predawn and set it		*/
/*	to default at LWP_min_spring for now.			*/
/*	Due to modification of porosity with depth.		*/ 
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void	canopy_stratum_daily_I(
							   struct	world_object	*world,
							   struct	basin_object	*basin,
							   struct	hillslope_object	*hillslope, 
							   struct	zone_object		*zone,
							   struct	patch_object	*patch,
							   struct 	canopy_strata_object 	*stratum,
							   struct 	command_line_object	*command_line,
							   struct	tec_entry		*event,
							   struct 	date 			current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	double	compute_lwp_predawn(
		int,
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);
	
	int 	compute_annual_turnover(
		struct epconst_struct,
		struct epvar_struct *,
		struct cstate_struct *);
	
	int	update_rootzone_moist(
		struct patch_object	*,
		struct	rooting_zone_object	*,
		struct command_line_object *);
	

	void	update_branch_mortality(
		struct epconst_struct,
		struct cstate_struct *,
		struct cdayflux_struct *,
		struct cdayflux_patch_struct *,
		struct nstate_struct *,
		struct ndayflux_struct *,
		struct ndayflux_patch_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		double, double);
	
	
	void	update_phenology(
		struct zone_object *,
		struct epvar_struct *,
		struct epconst_struct,
		struct phenology_struct *,
		struct cstate_struct *,
		struct cdayflux_struct *,
		struct cdayflux_patch_struct *,
		struct nstate_struct *,
		struct ndayflux_struct *,
		struct ndayflux_patch_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		struct litter_object *,
		struct soil_c_object *,
		struct soil_n_object *,
		struct rooting_zone_object *,
		double soil_depth,
		double,
		double,
		double,
		struct date,
		int);

	void	update_mortality(
		struct epconst_struct,
		struct cstate_struct *,
		struct cdayflux_struct *,
		struct cdayflux_patch_struct *,
		struct nstate_struct *,
		struct ndayflux_struct *,
		struct ndayflux_patch_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		int,
		struct mortality_struct);
	
	int	zero_stratum_daily_flux(struct cdayflux_struct *,
		struct ndayflux_struct *);
	
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	struct cstate_struct *cs;
	struct nstate_struct *ns;
	double wilting_point;
	struct mortality_struct mort;
	double leafcloss_perc, daily_mortality;

	/*--------------------------------------------------------------*/
	/* no processing at present for non-veg types			*/
	/*--------------------------------------------------------------*/
	if (stratum[0].defaults[0][0].epc.veg_type != NON_VEG) {
		
	/*--------------------------------------------------------------*/
	/*  zero all of the carbon daily flux variables.		*/
	/*--------------------------------------------------------------*/
	if (zero_stratum_daily_flux(&(stratum[0].cdf), &(stratum[0].ndf) )){
		fprintf(stderr,"fATAL ERROR: in zero_day_flux() ... Exiting\n");
		exit(EXIT_FAILURE);
	}


/*	stratum[0].Kup_direct = 0.0;
	stratum[0].Kup_diffuse = 0.0;*/

	if (patch[0].sat_deficit < ZERO)
		stratum[0].rootzone.S = 1.0;

	else if (patch[0].sat_deficit_z > patch[0].rootzone.depth)  	
		stratum[0].rootzone.S = min(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);	
	
	else  
		stratum[0].rootzone.S = min((patch[0].rz_storage + patch[0].rootzone.potential_sat 
			- patch[0].sat_deficit)
			/ patch[0].rootzone.potential_sat, 1.0);							
	 /*--------------------------------------------------------------*/
	/*      Compute canopy predawn LWP        			*/
	/*	Currently defaulted at non-stressed value.		*/
	/*--------------------------------------------------------------*/
	stratum[0].epv.psi =	compute_lwp_predawn(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].theta_psi_curve,
		patch[0].Tsoil,
		stratum[0].defaults[0][0].epc.psi_open,
		stratum[0].defaults[0][0].epc.psi_close,
		patch[0].soil_defaults[0][0].psi_air_entry,
		patch[0].soil_defaults[0][0].pore_size_index,
		patch[0].soil_defaults[0][0].p3,
		patch[0].soil_defaults[0][0].p4,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		stratum[0].rootzone.S);

	stratum[0].epv.psi_ravg = (stratum[0].defaults[0][0].epc.gs_ravg_days-1)/(stratum[0].defaults[0][0].epc.gs_ravg_days)* stratum[0].epv.psi_ravg + 
	 			1.0/(stratum[0].defaults[0][0].epc.gs_ravg_days) * stratum[0].epv.psi;

	if ( command_line[0].verbose_flag > 1 )
		printf(" %8f", stratum[0].epv.psi);

	/*--------------------------------------------------------------*/
	/*	keep track of water stress days for annual allocation   */
	/*--------------------------------------------------------------*/
	if ( (command_line[0].grow_flag > 0) &&
		(stratum[0].epv.psi <= stratum[0].defaults[0][0].epc.psi_close )) {
		if (command_line[0].verbose_flag == -2)
			printf("\n%4ld %4ld %4ld -111.1 ws day %lf %lf",
			current_date.day, current_date.month, current_date.year,
			stratum[0].epv.psi ,stratum[0].defaults[0][0].epc.psi_close);
		stratum[0].epv.wstress_days += 1;
	}


	/*--------------------------------------------------------------*/
        /*  compute temperature for acclimation - currently just a running mean of air temperature */
	/*--------------------------------------------------------------*/
	if (stratum[0].defaults[0][0].epc.Tacclim_days > 0)
       	 stratum[0].cs.Tacc = stratum[0].cs.Tacc*(stratum[0].defaults[0][0].epc.Tacclim_days-1.0)/
				(stratum[0].defaults[0][0].epc.Tacclim_days) + 
                              zone[0].metv.tavg * 1.0/stratum[0].defaults[0][0].epc.Tacclim_days;
	else
		stratum[0].cs.Tacc = zone[0].metv.tavg;


	if (command_line[0].grow_flag > 0)  {
		cs = &(stratum[0].cs);
		ns = &(stratum[0].ns);
	
	 	
		stratum[0].cs.preday_totalc = (cs->cpool + cs->cwdc
			+ cs->leafc + cs->leafc_store + cs->leafc_transfer
			+ cs->dead_leafc + cs->gresp_transfer + cs->gresp_store
			+ cs->frootc + cs->frootc_store + cs->frootc_transfer
			+ cs->live_stemc + cs->livestemc_store + cs->livestemc_transfer
			+ cs->dead_stemc + cs->deadstemc_store + cs->deadstemc_transfer
			+ cs->live_crootc + cs->livecrootc_store + cs->livecrootc_transfer
			+ cs->dead_crootc + cs->deadcrootc_store + cs->deadcrootc_transfer);

	
		daily_mortality = stratum[0].defaults[0][0].epc.max_daily_mortality;

		if (cs->age > stratum[0].defaults[0][0].epc.daily_mortality_threshold) 
			daily_mortality = daily_mortality - daily_mortality*min(1.0, 
				(cs->age-stratum[0].defaults[0][0].epc.daily_mortality_threshold)/100.0);

		daily_mortality = max(daily_mortality, stratum[0].defaults[0][0].epc.min_daily_mortality);

		mort.mort_cpool = daily_mortality;
		mort.mort_leafc = daily_mortality;
		mort.mort_deadleafc = daily_mortality;
		mort.mort_livestemc = daily_mortality;
		mort.mort_deadstemc = daily_mortality;
		mort.mort_livecrootc = daily_mortality;
		mort.mort_deadcrootc = daily_mortality;
		mort.mort_frootc = daily_mortality;
		update_mortality(stratum[0].defaults[0][0].epc,
			&(stratum[0].cs),
			&(stratum[0].cdf),
			&(patch[0].cdf),
			&(stratum[0].ns),
			&(stratum[0].ndf),
			&(patch[0].ndf),
			&(patch[0].litter_cs),
			&(patch[0].litter_ns),
			1,
			mort);

		if  ((stratum[0].defaults[0][0].epc.veg_type==TREE) && (stratum[0].defaults[0][0].epc.branch_turnover > ZERO)){
		update_branch_mortality(stratum[0].defaults[0][0].epc,
			&(stratum[0].cs),
			&(stratum[0].cdf),
			&(patch[0].cdf),
			&(stratum[0].ns),
			&(stratum[0].ndf),
			&(patch[0].ndf),
			&(patch[0].litter_cs),
			&(patch[0].litter_ns),
			stratum[0].cover_fraction,
			stratum[0].defaults[0][0].epc.branch_turnover);
		}
	}
	/*--------------------------------------------------------------*/
	/*  perform seasonal leaf sens. and budding						*/
	/*--------------------------------------------------------------*/
	update_phenology( zone, &(stratum[0].epv),
		stratum[0].defaults[0][0].epc,
		&(stratum[0].phen),
		&(stratum[0].cs),
		&(stratum[0].cdf),
		&(patch[0].cdf),
		&(stratum[0].ns),
		&(stratum[0].ndf),
		&(patch[0].ndf),
		&(patch[0].litter_cs),
		&(patch[0].litter_ns),
		&(patch[0].litter),
		&(patch[0].soil_cs),
		&(patch[0].soil_ns),
		&(stratum[0].rootzone),
		patch[0].soil_defaults[0][0].effective_soil_depth,
		stratum[0].cover_fraction,
		stratum[0].gap_fraction,
		basin[0].theta_noon,
		current_date,
		command_line[0].grow_flag);

	/*--------------------------------------------------------------*/
	/* if it is the last day of litterfall, perform carbon/nitrogen */
	/* 	allocations						*/
	/*--------------------------------------------------------------*/
	if (command_line[0].grow_flag > 0) {
		if ( command_line[0].verbose_flag == -2 )
			printf("\n%4ld %4ld %4ld -111.0 ",
			current_date.day, current_date.month, current_date.year);
		if ( stratum[0].phen.annual_allocation == 1) {
			/*--------------------------------------------------------------*/
			/*	livewood and leaf turnover				*/
			/*--------------------------------------------------------------*/
			if (compute_annual_turnover(stratum[0].defaults[0][0].epc,
				&(stratum[0].epv),
				&(stratum[0].cs) ) ){
				fprintf(stderr,
					"FATAL ERROR: in compute_annual_turnover() ... Exiting\n");
				exit(EXIT_FAILURE);
			}
			/*--------------------------------------------------------------*/
			/*	zero annual (season) accumulation variables			*/
			/*--------------------------------------------------------------*/
			stratum[0].epv.wstress_days = 0;
			stratum[0].epv.max_fparabs = 0.0;
			stratum[0].epv.min_vwc = 1.0;
		} /* end litterfall end of season calculations */
	} /* end grow flag */
	}	/* end NON_VEG conditional */
	return;
} /*end canopy_stratum_I.c*/

