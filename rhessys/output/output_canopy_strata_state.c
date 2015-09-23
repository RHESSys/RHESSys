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
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30ld %s"        , canopy_strata[0].ID                       , "canopy_strata_ID");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30ld %s"        , canopy_strata[0].defaults[0][0].ID        , "veg_parm_ID");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cover_fraction)         , "cover_fraction");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].gap_fraction)           , "gap_fraction");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].rootzone.depth)         , "rootzone.depth");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].snow_stored)            , "snow_stored");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].rain_stored)            , "rain_stored");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.cpool)               , "cs.cpool");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.leafc)               , "cs.leafc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.dead_leafc)          , "cs.dead_leafc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.leafc_store)         , "cs.leafc_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.leafc_transfer)      , "cs.leafc_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.live_stemc)          , "cs.live_stemc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.livestemc_store)     , "cs.livestemc_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.livestemc_transfer)  , "cs.livestemc_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.dead_stemc)          , "cs.dead_stemc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.deadstemc_store)     , "cs.deadstemc_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.deadstemc_transfer)  , "cs.deadstemc_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.live_crootc)         , "cs.live_crootc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.livecrootc_store)    , "cs.livecrootc_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.livecrootc_transfer) , "cs.livecrootc_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.dead_crootc)         , "cs.dead_crootc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.deadcrootc_store)    , "cs.deadcrootc_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.deadcrootc_transfer) , "cs.deadcrootc_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.frootc)              , "cs.frootc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.frootc_store)        , "cs.frootc_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.frootc_transfer)     , "cs.frootc_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].cs.cwdc)                , "cs.cwdc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].epv.prev_leafcalloc)    , "epv.prev_leafcalloc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.npool)               , "ns.npool");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.leafn)               , "ns.leafn");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.dead_leafn)          , "ns.dead_leafn");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.leafn_store)         , "ns.leafn_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.leafn_transfer)      , "ns.leafn_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.live_stemn)          , "ns.live_stemn");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.livestemn_store)     , "ns.livestemn_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.livestemn_transfer)  , "ns.livestemn_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.dead_stemn)          , "ns.dead_stemn");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.deadstemn_store)     , "ns.deadstemn_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.deadstemn_transfer)  , "ns.deadstemn_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.live_crootn)         , "ns.live_crootn");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.livecrootn_store)    , "ns.livecrootn_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.livecrootn_transfer) , "ns.livecrootn_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.dead_crootn)         , "ns.dead_crootn");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.deadcrootn_store)    , "ns.deadcrootn_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.deadcrootn_transfer) , "ns.deadcrootn_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.frootn)              , "ns.frootn");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.frootn_store)        , "ns.frootn_store");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.frootn_transfer)     , "ns.frootn_transfer");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.cwdn)                , "ns.cwdn");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].ns.retransn)            , "ns.retransn");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30ld %s"        , (canopy_strata[0].epv.wstress_days)       , "epv.wstress_days");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].epv.max_fparabs)        , "epv.max_fparabs");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30.8f %s"       , (canopy_strata[0].epv.min_vwc)            , "epv.min_vwc");
	fprintf(outfile , "\n            ");
	fprintf(outfile , "%-30ld %s"        , canopy_strata[0].num_base_stations        , "n_basestations");
	for (i=0; i < canopy_strata[0].num_base_stations; i++){
		fprintf(outfile,"\n            ");
		fprintf(outfile,"%-30ld %s",canopy_strata[0].base_stations[i][0].ID,"base_station_ID");
	}
	return;
} /*end output_canopy_strata_state*/
