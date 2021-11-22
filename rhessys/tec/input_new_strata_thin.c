/*--------------------------------------------------------------*/
/* 																*/
/*					input_new_strata						*/
/*																*/
/*	input_new_strata.c - creates a canopy_strata object	*/
/*																*/
/*	NAME														*/
/*	input_new_strata.c - creates a canopy_strata object	*/
/*																*/
/*	SYNOPSIS													*/
/*	void input_new_strata( 		*/
/*				struct	command_line_object	*command_line,		*/
/*				FILE	*world_file)						*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Allocates memory for a canopy strata object and reads in 	*/
/*	parameters for the object from an open hillslopes file.		*/
/*																*/
/*	Refer to construct_basin.c for a specification of the 		*/
/*	hillslopes file.											*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We assume that the FILE pointers to the 					*/
/*	hillslope file are positioned properly.						*/
/*	 															*/
/*	We assume that the basin and hillslope files have correct	*/
/*	syntax.														*/
/*																*/
/*	Original code, January 16, 1996.							*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#define ONE 1.0
#include "params.h"

void input_new_strata_thin(
											  struct	command_line_object	*command_line,
											  FILE	*world_file,
											  int		num_world_base_stations,
											  struct base_station_object **world_base_stations,
											  struct	default_object	*defaults,
											  struct	patch_object *patch,
											  struct canopy_strata_object     *canopy_strata,
											  int thintyp)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
		int ,
		int ,
		struct base_station_object **);

	int compute_annual_turnover(struct epconst_struct,
		struct epvar_struct *,
		struct cstate_struct *);


	int	compute_annual_litfall(
		struct epconst_struct,
		struct phenology_struct *,
		struct cstate_struct *,
		int);


	int	update_rooting_depth(
		struct rooting_zone_object *,
		double,
		double,
		double,
		double,
		double,
		double);

	double compute_delta_water(
		int, 
		double, 
		double,	
		double, 
		double, 
		double);

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

	void	*alloc(	size_t,
		char	*,
		char	*);

	param	*readtag_worldfile(int *,
				FILE *,
				char *);	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	base_stationID;
	int	i, dtmp, num_lines;
	int	default_object_ID;
	char	record[MAXSTR];
	double 	rootc, ltmp;
	double age_initial, height_initial;
	double cpool_loss, leafc_loss, deadleafc_loss;
	double livestemc_loss, deadstemc_loss, livecrootc_loss;
	double deadcrootc_loss, frootc_loss;
	struct mortality_struct mort;
	int	paramCnt=0;
	param	*paramPtr=NULL;
	
	mort.mort_cpool = 0.0;
	mort.mort_leafc = 0.0;
	mort.mort_deadleafc = 0.0;
	mort.mort_livestemc = 0.0;
	mort.mort_deadstemc = 0.0;
	mort.mort_livecrootc = 0.0;
	mort.mort_deadcrootc = 0.0;
	mort.mort_frootc = 0.0;
	
	/*--------------------------------------------------------------*/
	/*	Read in the next canopy strata record for this patch.		*/
	/*--------------------------------------------------------------*/
	paramPtr = readtag_worldfile(&paramCnt,world_file,"Canopy_Strata");

	// Variables changable by replacement
	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"veg_parm_ID","%d",canopy_strata[0].veg_parm_ID,1);
	 if (dtmp > 0)  canopy_strata[0].veg_parm_ID = dtmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cover_fraction","%lf",canopy_strata[0].cover_fraction,1);	
	  if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cover_fraction = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"gap_fraction","%lf",canopy_strata[0].gap_fraction,1);
	  if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].gap_fraction = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.stem_density","%lf",canopy_strata[0].cs.stem_density,1);
	  if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.stem_density = ltmp;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.age","%lf",canopy_strata[0].cs.age,1);
	  if (fabs(ltmp - NULLVAL) >= ONE){
		age_initial = canopy_strata[0].cs.age;
		canopy_strata[0].cs.age = ltmp;
	  }

	// thinning vars
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.cpool","%lf",NULLVAL,1);
	  	if (fabs(ltmp - NULLVAL) >= ONE){
			cpool_loss = ltmp * (canopy_strata[0].cs.cpool
				+ canopy_strata[0].cs.leafc_store + canopy_strata[0].cs.leafc_transfer
				+ canopy_strata[0].cs.gresp_transfer + canopy_strata[0].cs.gresp_store
				+ canopy_strata[0].cs.frootc_store + canopy_strata[0].cs.frootc_transfer
				+ canopy_strata[0].cs.livestemc_store + canopy_strata[0].cs.livestemc_transfer
				+ canopy_strata[0].cs.deadstemc_store + canopy_strata[0].cs.deadstemc_transfer
				+ canopy_strata[0].cs.livecrootc_store + canopy_strata[0].cs.livecrootc_transfer
				+ canopy_strata[0].cs.deadcrootc_store + canopy_strata[0].cs.deadcrootc_transfer);
			mort.mort_cpool = ltmp;
		}
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.leafc","%lf",NULLVAL,1);
		if (fabs(ltmp - NULLVAL) >= ONE){
			leafc_loss = ltmp * canopy_strata[0].cs.leafc;
			mort.mort_leafc = ltmp;
		}
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.dead_leafc","%lf",NULLVAL,1);
		if (fabs(ltmp - NULLVAL) >= ONE){
			deadleafc_loss = ltmp * canopy_strata[0].cs.dead_leafc;
			mort.mort_deadleafc = ltmp;
		}		
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.live_stemc","%lf",NULLVAL,1);
		if (fabs(ltmp - NULLVAL) >= ONE){
			livestemc_loss = ltmp * canopy_strata[0].cs.live_stemc;
			mort.mort_livestemc = ltmp;
		}	
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.dead_stemc","%lf",NULLVAL,1);
		if (fabs(ltmp - NULLVAL) >= ONE){
			deadstemc_loss = ltmp * canopy_strata[0].cs.dead_stemc;
			mort.mort_deadstemc = ltmp;
		}	
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.live_crootc","%lf",NULLVAL,1);
		if (fabs(ltmp - NULLVAL) >= ONE){
			livecrootc_loss = ltmp * canopy_strata[0].cs.live_crootc;
			mort.mort_livecrootc = ltmp;
		}	
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.dead_crootc","%lf",NULLVAL,1);
		if (fabs(ltmp - NULLVAL) >= ONE){
			deadcrootc_loss = ltmp * canopy_strata[0].cs.dead_crootc;
			mort.mort_deadcrootc = ltmp;
		}	
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"cs.frootc","%lf",NULLVAL,1);
		if (fabs(ltmp - NULLVAL) >= ONE){
			frootc_loss = ltmp * canopy_strata[0].cs.frootc;
			mort.mort_frootc = ltmp;
		}	

	// For use later when looking at (or changing) basestations
	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"canopy_strata_n_basestations","%d",canopy_strata[0].num_base_stations,0);

		/*--------------------------------------------------------------*/
		/*	Assign	defaults for this canopy_strata								*/
		/*--------------------------------------------------------------*/
		if (default_object_ID > 0) {
			i=0;
			while (defaults[0].stratum[i].ID != default_object_ID) {
				i++;
				/*--------------------------------------------------------------*/
				/*  Report an error if no match was found.  Otherwise assign    */
				/*  the default to point to this canopy_strata.						    */
				/*--------------------------------------------------------------*/
				if ( i>= defaults[0].num_stratum_default_files ){
					fprintf(stderr,
						"\nFATAL ERROR: in construct_canopy_strata, canopy_strata default ID %d not found.\n" ,
						default_object_ID);
					exit(EXIT_FAILURE);
				}
			} /* end-while */
			canopy_strata[0].defaults[0] = &defaults[0].stratum[i];
		}
	
	/*--------------------------------------------------------------*/
	/* Add removed C and N to CWD and litter pools */
	/*--------------------------------------------------------------*/
		update_mortality(canopy_strata[0].defaults[0][0].epc,
						 &(canopy_strata[0].cs),
						 &(canopy_strata[0].cdf),
						 &(patch[0].cdf),
						 &(canopy_strata[0].ns),
						 &(canopy_strata[0].ndf),
						 &(patch[0].ndf),
						 &(patch[0].litter_cs),
						 &(patch[0].litter_ns),
						 thintyp,
						 mort);
	
		/*--------------------------------------------------------------*/
		/*	zero all long term sinks				*/
		/*--------------------------------------------------------------*/
		canopy_strata[0].cs.gpsn_src = 0.0;
		canopy_strata[0].cs.leaf_mr_snk = 0.0;
		canopy_strata[0].cs.leaf_gr_snk = 0.0;
		canopy_strata[0].cs.livestem_mr_snk = 0.0;
		canopy_strata[0].cs.livestem_gr_snk = 0.0;
		canopy_strata[0].cs.deadstem_gr_snk = 0.0;
		canopy_strata[0].cs.livecroot_mr_snk = 0.0;
		canopy_strata[0].cs.livecroot_gr_snk = 0.0;
		canopy_strata[0].cs.deadcroot_gr_snk = 0.0;
		canopy_strata[0].cs.froot_mr_snk = 0.0;
		canopy_strata[0].cs.froot_gr_snk = 0.0;

	/*--------------------------------------------------------------*/
	/*	determine current lai and height  based on current leaf carbon	*/
	/* 	we need to initialize the sunlit/shaded proportions of LAI here */
	/*	(these will later be updated in update_phenology	*/
	/*	using Chen;s method					*/
	/*--------------------------------------------------------------*/
	canopy_strata[0].epv.proj_sla_sunlit = canopy_strata[0].defaults[0][0].epc.proj_sla;
	canopy_strata[0].epv.proj_sla_shade = canopy_strata[0].defaults[0][0].epc.proj_sla *
			canopy_strata[0].defaults[0][0].epc.shade_sla_mult;

	if ( canopy_strata[0].cs.leafc <= 1.0/canopy_strata[0].epv.proj_sla_sunlit) {
		canopy_strata[0].epv.proj_lai = canopy_strata[0].cs.leafc *
				canopy_strata[0].epv.proj_sla_sunlit;
		canopy_strata[0].epv.proj_lai_sunlit = canopy_strata[0].epv.proj_lai;
		canopy_strata[0].epv.proj_lai_shade = 0.0;
		}
	else  {
		canopy_strata[0].epv.proj_lai = 1.0 + ( canopy_strata[0].cs.leafc -
				1.0/canopy_strata[0].epv.proj_sla_sunlit) *
				canopy_strata[0].epv.proj_sla_shade;
		canopy_strata[0].epv.proj_lai_sunlit = 1.0;
		canopy_strata[0].epv.proj_lai_shade = canopy_strata[0].epv.proj_lai - 1.0;
		}

	canopy_strata[0].epv.all_lai = canopy_strata[0].epv.proj_lai *
		canopy_strata[0].defaults[0][0].epc.lai_ratio;
	canopy_strata[0].epv.max_proj_lai =  canopy_strata[0].epv.proj_lai;
	
	if (canopy_strata[0].defaults[0][0].epc.veg_type == TREE) {
	/* use stem density if included otherwise default to simply stem carbon */
		height_initial = canopy_strata[0].epv.height;
		if (canopy_strata[0].cs.stem_density < ZERO) {
		canopy_strata[0].epv.height =
		(canopy_strata[0].defaults[0][0].epc.height_to_stem_coef)
		* pow((canopy_strata[0].cs.live_stemc+canopy_strata[0].cs.dead_stemc),
		canopy_strata[0].defaults[0][0].epc.height_to_stem_exp);
		}
		else {
		canopy_strata[0].cs.stem_density = min(canopy_strata[0].cs.stem_density,
			canopy_strata[0].defaults[0][0].epc.max_stem_density);
		canopy_strata[0].epv.height =
                canopy_strata[0].defaults[0][0].epc.height_to_stem_coef
                * pow(((canopy_strata[0].cs.live_stemc+canopy_strata[0].cs.dead_stemc)/
                        canopy_strata[0].cs.stem_density),
                canopy_strata[0].defaults[0][0].epc.height_to_stem_exp);
		}
	}
	/* grass */
	else
		canopy_strata[0].epv.height =
		(canopy_strata[0].defaults[0][0].epc.height_to_stem_coef)
		* pow((canopy_strata[0].cs.leafc + canopy_strata[0].cs.dead_leafc),
		canopy_strata[0].defaults[0][0].epc.height_to_stem_exp);

	/*--------------------------------------------------------------*/
	/*	calculate all sided  and project pai from max projected lai	*/
	/*--------------------------------------------------------------*/
	if (canopy_strata[0].defaults[0][0].epc.veg_type == TREE) {
		canopy_strata[0].epv.proj_pai = canopy_strata[0].epv.proj_lai
			+ canopy_strata[0].defaults[0][0].epc.proj_swa
			* (canopy_strata[0].cs.live_stemc
			+ canopy_strata[0].cs.dead_stemc);
		canopy_strata[0].epv.all_pai = canopy_strata[0].epv.all_lai
			+ canopy_strata[0].defaults[0][0].epc.proj_swa
			* (canopy_strata[0].cs.live_stemc
			+ canopy_strata[0].cs.dead_stemc);
	}
	else {
		canopy_strata[0].epv.proj_pai =  canopy_strata[0].epv.proj_lai;
		canopy_strata[0].epv.all_pai =  canopy_strata[0].epv.all_lai;
	}
		/*--------------------------------------------------------------*/
		/*	initializae turnovers and litterfall 			*/
		/*--------------------------------------------------------------*/

		if (compute_annual_turnover(canopy_strata[0].defaults[0][0].epc,
			&(canopy_strata[0].epv),
			&(canopy_strata[0].cs)) ){
			fprintf(stderr,"FATAL ERROR: in compute_annual_turnover() ... Exiting\n");
			exit(EXIT_FAILURE);
		}

		if (compute_annual_litfall(canopy_strata[0].defaults[0][0].epc,
			&(canopy_strata[0].phen),
			&(canopy_strata[0].cs), command_line[0].grow_flag) ){
			fprintf(stderr,"FATAL ERROR: in compute_annual_litfall() ... Exiting\n");
			exit(EXIT_FAILURE);
		}


	/*--------------------------------------------------------------*/
	/*	compute new rooting depth based on current root carbon  */
	/*--------------------------------------------------------------*/
	if ( command_line[0].grow_flag != 0) {
	rootc = canopy_strata[0].cs.frootc+canopy_strata[0].cs.live_crootc+canopy_strata[0].cs.dead_crootc;
	if (rootc > ZERO){
		if (update_rooting_depth(
			&(canopy_strata[0].rootzone), 
			rootc, 
			canopy_strata[0].defaults[0][0].epc.root_growth_direction, 
			canopy_strata[0].defaults[0][0].epc.root_distrib_parm,
			canopy_strata[0].defaults[0][0].epc.max_root_depth,
			patch[0].soil_defaults[0][0].effective_soil_depth,
			canopy_strata[0].cs.stem_density)){
			fprintf(stderr,
				"FATAL ERROR: in compute_rooting_depth() from construct_canopy_strata()\n");
			exit(EXIT_FAILURE);
		}
	}
	}
	patch[0].rootzone.depth = max(patch[0].rootzone.depth, canopy_strata[0].rootzone.depth);
	
		/*--------------------------------------------------------------*/
		/*	set phenology timing if static allocation		*/
		/*--------------------------------------------------------------*/
	if (canopy_strata[0].defaults[0][0].epc.phenology_flag == STATIC ) {
		canopy_strata[0].phen.expand_startday =
			canopy_strata[0].defaults[0][0].epc.day_leafon;
		canopy_strata[0].phen.expand_stopday =
			canopy_strata[0].phen.expand_startday
			+ canopy_strata[0].defaults[0][0].epc.ndays_expand;
		canopy_strata[0].phen.litfall_startday =
			canopy_strata[0].defaults[0][0].epc.day_leafoff;
		canopy_strata[0].phen.litfall_stopday =
			canopy_strata[0].phen.litfall_startday
			+ canopy_strata[0].defaults[0][0].epc.ndays_litfall;
		if (canopy_strata[0].phen.expand_stopday > 365)
			canopy_strata[0].phen.expand_stopday -= 365;
		if (canopy_strata[0].phen.litfall_stopday > 365)
			canopy_strata[0].phen.litfall_stopday -= 365;
		/*---------------------------------------------------------------*/
		/* assume this is 365 for now since we don't know when next      */
		/* year's growing season will start                              */
		/*---------------------------------------------------------------*/
		canopy_strata[0].phen.nretdays = 365;
		canopy_strata[0].phen.gwseasonday = -1;
		canopy_strata[0].phen.lfseasonday = -1;
		canopy_strata[0].phen.pheno_flag = 0;
	}
	else {
		fprintf(stderr,"\nFATAL ERROR - construct_canopy_stratum.c");
		fprintf(stderr,"\n phenology flag must be set to 0 for STATIC");
		fprintf(stderr,"\n since dynamic phenology timing not yet implemented");
		exit(EXIT_FAILURE);
	}


	/*--------------------------------------------------------------*/
	/* initialize runnning average of psi using current day psi     */
	/*--------------------------------------------------------------*/

	if (canopy_strata[0].rootzone.depth > ZERO)
		canopy_strata[0].rootzone.potential_sat = compute_delta_water(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		canopy_strata[0].rootzone.depth, 
		0.0);			

	canopy_strata[0].rootzone.S = min(patch[0].rz_storage / canopy_strata[0].rootzone.potential_sat, 1.0);

	canopy_strata[0].epv.psi =	compute_lwp_predawn(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].theta_psi_curve,
		patch[0].Tsoil,
		canopy_strata[0].defaults[0][0].epc.psi_open,
		canopy_strata[0].defaults[0][0].epc.psi_close,
		patch[0].soil_defaults[0][0].psi_air_entry,
		patch[0].soil_defaults[0][0].pore_size_index,
		patch[0].soil_defaults[0][0].p3,
		patch[0].soil_defaults[0][0].p4,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		canopy_strata[0].rootzone.S);

	canopy_strata[0].epv.psi_ravg = canopy_strata[0].epv.psi;


		/*--------------------------------------------------------------*/
		/*	for now initialize these accumuling variables		*/
		/*--------------------------------------------------------------*/
		if (fabs(ltmp - NULLVAL) >= ONE)  {
			canopy_strata[0].epv.wstress_days = 0;
			canopy_strata[0].epv.max_fparabs = 0.0;
			canopy_strata[0].epv.min_vwc = 1.0;
			canopy_strata[0].cs.age = 0;
			canopy_strata[0].cs.num_resprout = 0;
		}
		/*--------------------------------------------------------------*/
		/*	Read in the number of  strata base stations 					*/
		/*--------------------------------------------------------------*/
		if (dtmp > 0) {
			canopy_strata[0].num_base_stations = dtmp;
			/*--------------------------------------------------------------*/
			/*    Allocate a list of base stations for this strata.			*/
			/*--------------------------------------------------------------*/
			canopy_strata[0].base_stations = (struct base_station_object **)
				alloc(canopy_strata[0].num_base_stations *
				sizeof(struct base_station_object *),"base_stations",
				"construct_canopy_strata");
			/*--------------------------------------------------------------*/
			/*      Read each base_station ID and then point to that base_statio*/
			/*--------------------------------------------------------------*/
			for (i=0 ; i<canopy_strata[0].num_base_stations; i++){
				fscanf(world_file,"%d",&(base_stationID));
				read_record(world_file, record);
				/*--------------------------------------------------------------*/
				/*	Point to the appropriate base station in the base       	*/
				/*              station list for this world.					*/
				/*																*/
				/*--------------------------------------------------------------*/
				canopy_strata[0].base_stations[i] = assign_base_station(
					base_stationID,
					num_world_base_stations,
					world_base_stations);
			} /*end for*/
		}

	/*----------------------------------------------------------------------------------------*/
    /* accumulate the redefine losses for output filter                           */
    /*----------------------------------------------------------------------------------------*/
	if (thintyp == 2){ /* Harvest */
		if ((command_line[0].output_flags.monthly == 1) &&
				(command_line[0].output_filter_strata_accum_monthly || command_line[0].c != NULL)) {
			canopy_strata[0].acc_month.redefine_totalc_harvest += cpool_loss + leafc_loss + deadleafc_loss + 
					livestemc_loss + deadstemc_loss + livecrootc_loss + deadcrootc_loss + frootc_loss;
			canopy_strata[0].acc_month.redefine_age = age_initial;
			canopy_strata[0].acc_month.redefine_height = height_initial;
		}
		if ((command_line[0].output_flags.yearly == 1) &&
				(command_line[0].output_filter_strata_accum_yearly || command_line[0].c != NULL || command_line[0].f != NULL)){	
			canopy_strata[0].acc_year.redefine_totalc_harvest += cpool_loss + leafc_loss + deadleafc_loss + 
					livestemc_loss + deadstemc_loss + livecrootc_loss + deadcrootc_loss + frootc_loss;
			canopy_strata[0].acc_year.redefine_age = age_initial;
			canopy_strata[0].acc_year.redefine_height = height_initial;
		}
	}
	if (thintyp != 2){ /* Remain on patch */
		if ((command_line[0].output_flags.monthly == 1) &&
				(command_line[0].output_filter_strata_accum_monthly || command_line[0].c != NULL)) {
			canopy_strata[0].acc_month.redefine_totalc_remain += cpool_loss + leafc_loss + deadleafc_loss + 
					livestemc_loss + deadstemc_loss + livecrootc_loss + deadcrootc_loss + frootc_loss;
		}
		if ((command_line[0].output_flags.yearly == 1) &&
				(command_line[0].output_filter_strata_accum_yearly || command_line[0].c != NULL || command_line[0].f != NULL)){	
			canopy_strata[0].acc_year.redefine_totalc_remain += cpool_loss + leafc_loss + deadleafc_loss + 
					livestemc_loss + deadstemc_loss + livecrootc_loss + deadcrootc_loss + frootc_loss;
		}
	}

	return;
} /*end input_new_strata.c*/

