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
#include "rhessys.h"
#define ONE 1.0
void skip_strata(
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
	fscanf(world_file,"%d",&(ltmp));
	read_record(world_file, record);

 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cover_fraction = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].gap_fraction = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].rootzone.depth = ltmp;*/
	    fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].snow_stored = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].rain_stored = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.cpool = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.leafc = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.dead_leafc = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.leafc_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.leafc_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.live_stemc = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.livestemc_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.livestemc_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.dead_stemc = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.deadstemc_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.deadstemc_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.live_crootc = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.livecrootc_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.livecrootc_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.dead_crootc = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.deadcrootc_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.deadcrootc_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.frootc = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.frootc_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.frootc_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].cs.cwdc = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].epv.prev_leafcalloc = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.npool = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.leafn = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.dead_leafn = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.leafn_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.leafn_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.live_stemn = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.livestemn_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.livestemn_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.dead_stemn = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.deadstemn_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.deadstemn_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.live_crootn = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.livecrootn_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.livecrootn_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.dead_crootn = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.deadcrootn_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.deadcrootn_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.frootn = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.frootn_store = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.frootn_transfer = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.cwdn = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].ns.retransn = ltmp;*/
		

		/*--------------------------------------------------------------*/
		/*	intialized annual flux variables			*/
		/*--------------------------------------------------------------*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].epv.wstress_days = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].epv.max_fparabs = ltmp;*/
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE) canopy_strata[0].epv.min_vwc = ltmp;*/

		/*--------------------------------------------------------------*/
		/*	Read in the number of  strata base stations 					*/
		/*--------------------------------------------------------------*/
 		fscanf(world_file,"%d",&(dtmp));
		read_record(world_file, record);
		if (dtmp > 0) {
			/*canopy_strata[0].num_base_stations = dtmp;*/
			/*--------------------------------------------------------------*/
			/*    Allocate a list of base stations for this strata.			*/
			/*--------------------------------------------------------------*/
			/*canopy_strata[0].base_stations = (struct base_station_object **)
				alloc(canopy_strata[0].num_base_stations *
				sizeof(struct base_station_object *),"base_stations",
				"skip_strata");*/
			/*--------------------------------------------------------------*/
			/*      Read each base_station ID and then point to that base_statio*/
			/*--------------------------------------------------------------*/
			for (i=0 ; i<dtmp; i++){
				fscanf(world_file,"%d",&(ltmp));
				read_record(world_file, record);
				/*--------------------------------------------------------------*/
				/*	Point to the appropriate base station in the base       	*/
				/*              station list for this world.					*/
				/*																*/
				/*--------------------------------------------------------------*/
		/*		canopy_strata[0].base_stations[i] = assign_base_station(
					base_stationID,
					num_world_base_stations,
					world_base_stations);*/
			} /*end for*/
		}
			 
	return;
} /*end input_new_strata.c*/

