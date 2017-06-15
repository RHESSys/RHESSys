/*--------------------------------------------------------------*/
/* 																*/
/*					construct_canopy_strata						*/
/*																*/
/*	construct_canopy_strata.c - creates a canopy_strata object	*/
/*																*/
/*	NAME														*/
/*	construct_canopy_strata.c - creates a canopy_strata object	*/
/*																*/
/*	SYNOPSIS													*/
/*	struct canopy_strata_object  construct_canopy_strata( 		*/
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
/*	Sep 2 1997	RAF												*/
/*	Removed all references to grow or extended state variables.	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

struct canopy_strata_object *construct_canopy_strata(
													 struct	command_line_object	*command_line,
													 FILE	*world_file,
													 struct	patch_object	*patch,
													 int		num_world_base_stations,
													 struct base_station_object **world_base_stations,
													 struct	default_object	*defaults)
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
		double);

	void	*alloc(size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	base_stationID;
	int	i;
	double	sai, rootc;
	int	default_object_ID; 
	int     spinup_default_object_ID; 
	char	record[MAXSTR];
	struct	canopy_strata_object	*canopy_strata;
	
	/*--------------------------------------------------------------*/
	/*  Allocate a canopy_strata object.                                */
	/*--------------------------------------------------------------*/
	canopy_strata = (struct canopy_strata_object *) alloc( 1 *
		sizeof( struct canopy_strata_object ),"canopy_strata",
		"construct_canopy_strata" );
	
	/*--------------------------------------------------------------*/
	/*	Read in the next canopy strata record for this patch.	*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(canopy_strata[0].ID));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(default_object_ID));
	read_record(world_file, record);
	if (command_line[0].vegspinup_flag > 0){
	    fscanf(world_file,"%d",&(spinup_default_object_ID));
	    read_record(world_file, record);
        }
  fscanf(world_file,"%lf",&(canopy_strata[0].cover_fraction));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].gap_fraction));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].rootzone.depth));
	read_record(world_file, record);
		if (command_line[0].tmp_value > ZERO)
			canopy_strata[0].rootzone.depth *= command_line[0].tmp_value;

	fscanf(world_file,"%lf",&(canopy_strata[0].snow_stored));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].rain_stored));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.cpool));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.leafc));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.dead_leafc));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.leafc_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.leafc_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.live_stemc));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.livestemc_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.livestemc_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.dead_stemc));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.deadstemc_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.deadstemc_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.live_crootc));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.livecrootc_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.livecrootc_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.dead_crootc));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.deadcrootc_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.deadcrootc_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.frootc));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.frootc_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.frootc_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].cs.cwdc));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].epv.prev_leafcalloc));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.npool));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.leafn));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.dead_leafn));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.leafn_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.leafn_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.live_stemn));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.livestemn_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.livestemn_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.dead_stemn));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.deadstemn_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.deadstemn_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.live_crootn));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.livecrootn_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.livecrootn_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.dead_crootn));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.deadcrootn_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.deadcrootn_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.frootn));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.frootn_store));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.frootn_transfer));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.cwdn));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].ns.retransn));
	read_record(world_file, record);

	if (command_line[0].vegspinup_flag > 0){
     canopy_strata[0].target.lai = NULLVAL;
     canopy_strata[0].target.total_stemc = NULLVAL;
     canopy_strata[0].target.met = 2;
     
   }
	/*--------------------------------------------------------------*/
	/*	intialized annual flux variables			*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(canopy_strata[0].epv.wstress_days));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].epv.max_fparabs));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(canopy_strata[0].epv.min_vwc));
	read_record(world_file, record);
	

	/*--------------------------------------------------------------*/
	/*	Assign	defaults for this canopy_strata								*/
	/*--------------------------------------------------------------*/
	canopy_strata[0].defaults = (struct stratum_default **)
		alloc( sizeof(struct stratum_default *),"defaults",
		"construct_canopy_strata" );
	i = 0;
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

	/*--------------------------------------------------------------*/
	/* if spinup module is called assign spinup defaults            */
	/*--------------------------------------------------------------*/
	if (command_line[0].vegspinup_flag > 0) {
	canopy_strata[0].spinup_defaults = (struct spinup_default **)
		alloc( sizeof(struct spinup_default *),"defaults",
		"construct_stratum" );
	i = 0;
	while (defaults[0].spinup[i].ID != spinup_default_object_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this patch.						    */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_spinup_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_stratum, spinup default ID %d not found for patch %d\n" ,
				spinup_default_object_ID, patch[0].ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	canopy_strata[0].spinup_defaults[0] = &defaults[0].spinup[i];
	}

	/*--------------------------------------------------------------*/
	/* zero all non tree stem and wood variables			*/
	/*--------------------------------------------------------------*/
	if (canopy_strata[0].defaults[0][0].epc.veg_type != TREE) {
		canopy_strata[0].cs.live_stemc = 0.0;
		canopy_strata[0].cs.dead_stemc = 0.0;
		canopy_strata[0].cs.live_crootc = 0.0;
		canopy_strata[0].cs.dead_crootc = 0.0;
		canopy_strata[0].cs.livestemc_store = 0.0;
		canopy_strata[0].cs.deadstemc_store = 0.0;
		canopy_strata[0].cs.livestemc_transfer = 0.0;
		canopy_strata[0].cs.deadstemc_transfer = 0.0;
		canopy_strata[0].cs.livecrootc_store = 0.0;
		canopy_strata[0].cs.deadcrootc_store = 0.0;
		canopy_strata[0].cs.livecrootc_transfer = 0.0;
		canopy_strata[0].cs.deadcrootc_transfer = 0.0;
		canopy_strata[0].cs.cwdc = 0.0;

		canopy_strata[0].ns.live_stemn = 0.0;
		canopy_strata[0].ns.dead_stemn = 0.0;
		canopy_strata[0].ns.live_crootn = 0.0;
		canopy_strata[0].ns.dead_crootn = 0.0;
		canopy_strata[0].ns.livestemn_store = 0.0;
		canopy_strata[0].ns.deadstemn_store = 0.0;
		canopy_strata[0].ns.livestemn_transfer = 0.0;
		canopy_strata[0].ns.deadstemn_transfer = 0.0;
		canopy_strata[0].ns.livecrootn_store = 0.0;
		canopy_strata[0].ns.deadcrootn_store = 0.0;
		canopy_strata[0].ns.livecrootn_transfer = 0.0;
		canopy_strata[0].ns.deadcrootn_transfer = 0.0;
		canopy_strata[0].ns.cwdn = 0.0;
	}
	/*--------------------------------------------------------------*/
	/* 	zero other carbon stores for non veg			*/
	/*--------------------------------------------------------------*/
	if (canopy_strata[0].defaults[0][0].epc.veg_type == NON_VEG) {
		canopy_strata[0].cs.cpool = 0.0;
		canopy_strata[0].cs.leafc = 0.0;
		canopy_strata[0].cs.dead_leafc = 0.0;
		canopy_strata[0].cs.leafc_store = 0.0;
		canopy_strata[0].cs.leafc_transfer = 0.0;
		canopy_strata[0].cs.frootc = 0.0;
		canopy_strata[0].cs.frootc_store = 0.0;
		canopy_strata[0].cs.frootc_transfer = 0.0;

		canopy_strata[0].ns.npool = 0.0;
		canopy_strata[0].ns.leafn = 0.0;
		canopy_strata[0].ns.dead_leafn = 0.0;
		canopy_strata[0].ns.leafn_store = 0.0;
		canopy_strata[0].ns.leafn_transfer = 0.0;
		canopy_strata[0].ns.frootn = 0.0;
		canopy_strata[0].ns.frootn_store = 0.0;
		canopy_strata[0].ns.frootn_transfer = 0.0;
	}

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
	canopy_strata[0].NO3_stored = 0.0; // this is for the NO3 deposition on leaves
	
	/*--------------------------------------------------------------*/
	/*      initialize accumulator variables                        */
	/*--------------------------------------------------------------*/
	canopy_strata[0].acc_year.lai = 0.0;
	canopy_strata[0].acc_year.psn = 0.0;
	canopy_strata[0].acc_year.lwp = 0.0;
	canopy_strata[0].acc_year.minNSC = -999;
	canopy_strata[0].acc_year.length = 0;
	canopy_strata[0].acc_month.lai = 0.0;
	canopy_strata[0].acc_month.psn = 0.0;
	canopy_strata[0].acc_month.lwp = 0.0;
	canopy_strata[0].acc_month.length = 0;

        canopy_strata[0].cs.Tacc = 20.0;	
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
	
	if (canopy_strata[0].defaults[0][0].epc.veg_type == TREE)
		canopy_strata[0].epv.height =
		canopy_strata[0].defaults[0][0].epc.height_to_stem_coef
		* pow((canopy_strata[0].cs.live_stemc+canopy_strata[0].cs.dead_stemc),
		canopy_strata[0].defaults[0][0].epc.height_to_stem_exp);
	else
		canopy_strata[0].epv.height =
		canopy_strata[0].defaults[0][0].epc.height_to_stem_coef
		* pow((canopy_strata[0].cs.leafc + canopy_strata[0].cs.dead_leafc),
		canopy_strata[0].defaults[0][0].epc.height_to_stem_exp);

	/*--------------------------------------------------------------*/
	/*	calculate all sided  and project pai from max projected lai	*/
	/*--------------------------------------------------------------*/

	if (canopy_strata[0].defaults[0][0].epc.veg_type == TREE) {
		sai = 0.55*(1.0-exp(-0.175*(canopy_strata[0].cs.live_stemc+canopy_strata[0].cs.dead_stemc)));
		canopy_strata[0].epv.proj_pai = max(canopy_strata[0].epv.proj_lai + sai, 0.0);
		canopy_strata[0].epv.all_pai = max(canopy_strata[0].epv.all_lai + sai, 0.0);
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
	/* 	for static non-grow version use the worldfile rooting depth */
	/*	as read in above					*/
	/*--------------------------------------------------------------*/
	if (( command_line[0].grow_flag != 0) )  {
		rootc = canopy_strata[0].cs.frootc+canopy_strata[0].cs.live_crootc+canopy_strata[0].cs.dead_crootc;
		if (rootc > ZERO){
			if (update_rooting_depth(
				&(canopy_strata[0].rootzone), 
				rootc, 
				canopy_strata[0].defaults[0][0].epc.root_growth_direction, 
				canopy_strata[0].defaults[0][0].epc.root_distrib_parm,
				patch[0].soil_defaults[0][0].effective_soil_depth)){
				fprintf(stderr,
					"FATAL ERROR: in compute_rooting_depth() from construct_canopy_strata()\n");
				exit(EXIT_FAILURE);
				}
		}
	}

	/*--------------------------------------------------------------*/
	/*	initialize leaf out for non-grow version		*/
	/*--------------------------------------------------------------*/
	if (( command_line[0].grow_flag == 0) && 
		(canopy_strata[0].defaults[0][0].epc.veg_type != NON_VEG) ){
		/*
		canopy_strata[0].cs.leafc_transfer = canopy_strata[0].phen.leaflitfallc;
		canopy_strata[0].ns.leafn_transfer = canopy_strata[0].phen.leaflitfallc
			* canopy_strata[0].ns.leafn
			/ canopy_strata[0].cs.leafc;
		canopy_strata[0].cs.leafc_store = 0.0;
		canopy_strata[0].ns.leafn_store = 0.0;
		*/
	}
	/*--------------------------------------------------------------*/
	/*	set phenology timing if static allocation		*/
	/*  and initialize for dynamic runs				*/
	/*--------------------------------------------------------------*/
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
	/*--------------------------------------------------------------*/
	/*	set critical soil moisture (at stomatal closure)	*/
	/*      psi_close is converted to m water tension from MPa using     */
	/*      1m water tension = 10000 Pa                             */
	/*                      = 0.01 Mpa                              */
	/*--------------------------------------------------------------*/
	canopy_strata[0].epv.crit_vwc =  pow(
		((-1.0 * 100.0 * canopy_strata[0].defaults[0][0].epc.psi_close)
		/ patch[0].soil_defaults[0][0].psi_air_entry),
		patch[0].soil_defaults[0][0].pore_size_index );

	
	/*--------------------------------------------------------------*/
	/* initialize runnning average of psi **** should actually  calc */
	/* current day psi						*/
	/*--------------------------------------------------------------*/
	canopy_strata[0].epv.psi_ravg = canopy_strata[0].defaults[0][0].epc.psi_open;

	/*--------------------------------------------------------------*/
	/*	for now initialize these accumuling variables		*/
	/*	note that age really should be a state variable 	*/
	/*	and initialized in the worldfile			*/
	/*--------------------------------------------------------------*/
	canopy_strata[0].cs.num_resprout = 0;
	canopy_strata[0].cs.age = 0;
	canopy_strata[0].epv.wstress_days = 0;
	canopy_strata[0].epv.max_fparabs = 0.0;
	canopy_strata[0].epv.min_vwc = 1.0;
	/*--------------------------------------------------------------*/
	/*	Read in the number of  strata base stations 					*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(canopy_strata[0].num_base_stations));
	read_record(world_file, record);
	/*--------------------------------------------------------------*/
	/*    Allocate a list of base stations for this strata.			*/
	/*--------------------------------------------------------------*/
	canopy_strata[0].base_stations = (struct base_station_object **)
		alloc(canopy_strata[0].num_base_stations *
		sizeof(struct base_station_object *),"base_stations",
		"construct_canopy_strata");
	/*--------------------------------------------------------------*/
	/* Read each base_station ID and then point to that base_station*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<canopy_strata[0].num_base_stations; i++){
		fscanf(world_file,"%d",&(base_stationID));
		read_record(world_file, record);
		/*--------------------------------------------------------------*/
		/*	Point to the appropriate base station in the base       	*/
		/*              station list for this world.					*/
		/*--------------------------------------------------------------*/
		canopy_strata[0].base_stations[i] = assign_base_station(
			base_stationID,
			num_world_base_stations,
			world_base_stations);
	} /*end for*/
	return(canopy_strata);
} /*end construct_canopy_strata.c*/
