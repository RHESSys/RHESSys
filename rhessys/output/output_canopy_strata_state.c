/*--------------------------------------------------------------*/
/* 																*/
/*					output_canopy_strata_state					*/
/*																*/
/*	output_canopy_strata_state - outputs state data			*/
/*																*/
/*	NAME														*/
/*	output_canopy_strata_state - outputs state data 	.		*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_canopy_strata_state(						*/
/*					struct	canopy_strata_object	*canopy_strata,				*/
/*					struct	date	current_date,				*/
/*					FILE	*outfile							*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs current canopy_strata state variable to a make a new		*/
/*	world input file											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_canopy_strata_state(
								   struct	canopy_strata_object	*canopy_strata,
								   struct	date	current_date,
								   struct	command_line_object	*command_line,
								   FILE	*outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int i;
	/*--------------------------------------------------------------*/
	/*	output canopy_strata information									*/
	/*--------------------------------------------------------------*/
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30ld %s",canopy_strata[0].ID, "canopy_strata_ID");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30ld %s",canopy_strata[0].defaults[0][0].ID, "default_ID");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cover_fraction),
		"cover_fraction");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s",(canopy_strata[0].gap_fraction),"gap_fraction");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].rootzone.depth),"root_depth");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].snow_stored), "snow_stored");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].rain_stored), "rain_stored");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.cpool), "cs_cpool");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.leafc), "cs_leafc");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.dead_leafc), "cs_dead_leafc");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.leafc_store),
		"cs_leafc_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.leafc_transfer),
		"cs_leafc_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.live_stemc),
		"cs_live_stemc");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.livestemc_store),
		"cs_livestemc_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.livestemc_transfer),
		"cs_livestemc_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.dead_stemc),
		"cs_dead_stemc");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.deadstemc_store),
		"cs_deadstemc_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.deadstemc_transfer),
		"cs_deadstemc_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.live_crootc),
		"cs_live_crootc");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.livecrootc_store),
		"cs_livecrootc_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.livecrootc_transfer),
		"cs_livecrootc_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.dead_crootc),
		"cs_dead_crootc");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.deadcrootc_store),
		"cs_deadcrootc_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.deadcrootc_transfer),
		"cs_deadcrootc_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.frootc), "cs_frootc");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.frootc_store),
		"cs_frootc_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.frootc_transfer),
		"cs_frootc_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.cwdc), "cs_cwdc");
	fprintf(outfile,"\n            ");
	if (command_line[0].reproduction_flag == 1) {
		fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.reprodc), "cs_reprodc");
		fprintf(outfile,"\n            ");
		fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.reprodc_transfer), "cs_reprodc_transfer");
		fprintf(outfile,"\n            ");
		fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.reprodc_store), "cs_reprodc_store");
		fprintf(outfile,"\n            ");
		fprintf(outfile,"%-30.8f %s", (canopy_strata[0].cs.seedc), "cs_seedc");
		fprintf(outfile,"\n            ");
		}

	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].epv.prev_leafcalloc),
		"epv.prev_leafcalloc");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.npool), "ns_npool");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.leafn), "ns_leafn");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.dead_leafn),
		"ns_dead_leafn");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.leafn_store),
		"ns_leafn_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.leafn_transfer),
		"ns_leafn_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.live_stemn),
		"ns_live_stemn");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.livestemn_store),
		"ns_livestemn_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.livestemn_transfer),
		"ns_livestemn_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.dead_stemn),
		"ns_dead_stemn");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.deadstemn_store),
		"ns_deadstemn_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.deadstemn_transfer),
		"ns_deadstemn_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.live_crootn),
		"ns_live_crootn");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.livecrootn_store),
		"ns_livecrootn_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.livecrootn_transfer),
		"ns_livecrootn_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.dead_crootn),
		"ns_dead_crootn");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.deadcrootn_store),
		"ns_deadcrootn_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.deadcrootn_transfer),
		"ns_deadcrootn_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.frootn), "ns_frootn");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.frootn_store),
		"ns_frootn_store");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.frootn_transfer),
		"ns_frootn_transfer");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.cwdn), "ns_cwdn");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.retransn), "ns_retransn");
	fprintf(outfile,"\n            ");

	if (command_line[0].reproduction_flag == 1) {
		fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.reprodn), "ns_reprodn");
		fprintf(outfile,"\n            ");
		fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.reprodn_transfer), "ns_reprodn_transfer");
		fprintf(outfile,"\n            ");
		fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.reprodn_store), "ns_reprodn_store");
		fprintf(outfile,"\n            ");
		fprintf(outfile,"%-30.8f %s", (canopy_strata[0].ns.seedn), "ns_seedn");
		fprintf(outfile,"\n            ");
		}

	fprintf(outfile,"%-30ld %s", (canopy_strata[0].epv.wstress_days),
		"epv_wstress_days");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].epv.max_fparabs),
		"epv_max_fparabs");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30.8f %s", (canopy_strata[0].epv.min_vwc), "epv_min_vwc");
	fprintf(outfile,"\n            ");
	fprintf(outfile,"%-30ld %s",canopy_strata[0].num_base_stations,
		"n_basestations");
	for (i=0; i < canopy_strata[0].num_base_stations; i++){
		fprintf(outfile,"\n            ");
		fprintf(outfile,"%-30ld %s",canopy_strata[0].base_stations[i][0].ID,
			"base_station_ID");
	}
	return;
} /*end output_canopy_strata_state*/
