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
void input_new_strata(
											  struct	command_line_object	*command_line,
											  FILE	*world_file,
											  int		num_world_base_stations,
											  struct base_station_object **world_base_stations,
											  struct	default_object	*defaults,
											  struct	patch_object *patch,
											  struct canopy_strata_object     *canopy_strata)
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

	void	*alloc(	size_t,
		char	*,
		char	*);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	base_stationID;
	int	i, dtmp, num_lines;
	int	default_object_ID;
	char	record[MAXSTR];
	double 	rootc, ltmp;

	/*--------------------------------------------------------------*/
	/*	Read in the next canopy strata record for this patch.		*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(default_object_ID));
	read_record(world_file, record);

 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cover_fraction = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].gap_fraction = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].rootzone.depth = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].snow_stored = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].rain_stored = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.cpool = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.leafc = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.dead_leafc = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.leafc_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.leafc_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.live_stemc = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.livestemc_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.livestemc_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.dead_stemc = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.deadstemc_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.deadstemc_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.live_crootc = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.livecrootc_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.livecrootc_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.dead_crootc = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.deadcrootc_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.deadcrootc_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.frootc = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.frootc_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.frootc_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.cwdc = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].epv.prev_leafcalloc = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.npool = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.leafn = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.dead_leafn = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.leafn_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.leafn_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.live_stemn = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.livestemn_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.livestemn_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.dead_stemn = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.deadstemn_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.deadstemn_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.live_crootn = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.livecrootn_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.livecrootn_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.dead_crootn = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.deadcrootn_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.deadcrootn_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.frootn = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.frootn_store = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.frootn_transfer = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.cwdn = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.retransn = ltmp;
		

		/*--------------------------------------------------------------*/
		/*	intialized annual flux variables			*/
		/*--------------------------------------------------------------*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].epv.wstress_days = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].epv.max_fparabs = ltmp;
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].epv.min_vwc = ltmp;

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
			patch[0].soil_defaults[0][0].effective_soil_depth)){
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
	}
	else {
		fprintf(stderr,"\nFATAL ERROR - construct_canopy_stratum.c");
		fprintf(stderr,"\n phenology flag must be set to 0 for STATIC");
		fprintf(stderr,"\n since dynamic phenology timing not yet implemented");
		exit(EXIT_FAILURE);
	}
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
 		fscanf(world_file,"%d",&(dtmp));
		read_record(world_file, record);
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
			 
	return;
} /*end input_new_strata.c*/

