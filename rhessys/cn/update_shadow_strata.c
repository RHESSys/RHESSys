/*--------------------------------------------------------------*/
/* 							                        	                      */
/*			update_shadow_strata		                              	*/
/*							                                              	*/
/*	NAME						                                          	*/
/*	update_shadow_statrum                             					*/
/*								                                              */
/*							                                              	*/
/*	SYNOPSIS			                                        			*/
/*						                                               		*/
/*	OPTIONS						                                         	*/
/*							                                              	*/
/*	DESCRIPTION					                                       	*/
/*							                                              	*/
/*	PROGRAMMER NOTES				                                   	*/
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "phys_constants.h"
#include "rhessys.h"

void	update_shadow_strata(
							   struct	world_object		      *world,
							   struct canopy_strata_object 	*stratum,
                 struct canopy_strata_object  *shadow_strata,
							   struct command_line_object	  *command_line,
							   struct date 			            current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration		                          		*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	   
	/*------------------------------------------------------------------------*/
	/*	Check whether the target.met flag is set                              */
	/*	if it is do nothing, if not copy strata values to shadow              */
	/*	then check if targets have been met. If so, set target.met flag to 1 	*/
	/*------------------------------------------------------------------------*/
  
 // printf("\ntarget met: %d", stratum[0].target.met);

  if(stratum[0].target.met != 1){
    shadow_strata[0].cover_fraction = stratum[0].cover_fraction;
    shadow_strata[0].gap_fraction = stratum[0].gap_fraction;
    shadow_strata[0].rootzone.depth = stratum[0].rootzone.depth;
    shadow_strata[0].snow_stored = stratum[0].snow_stored;
    shadow_strata[0].rain_stored = stratum[0].rain_stored;
    shadow_strata[0].cs.cpool = stratum[0].cs.cpool;
    shadow_strata[0].cs.leafc = stratum[0].cs.leafc;
    shadow_strata[0].cs.dead_leafc = stratum[0].cs.dead_leafc;
    shadow_strata[0].cs.leafc_store = stratum[0].cs.leafc_store;
    shadow_strata[0].cs.leafc_transfer = stratum[0].cs.leafc_transfer;
    shadow_strata[0].cs.live_stemc = stratum[0].cs.live_stemc;
    shadow_strata[0].cs.livestemc_store = stratum[0].cs.livestemc_store;
    shadow_strata[0].cs.livestemc_transfer = stratum[0].cs.livestemc_transfer;
    shadow_strata[0].cs.dead_stemc = stratum[0].cs.dead_stemc;
    shadow_strata[0].cs.deadstemc_store = stratum[0].cs.deadstemc_store;
    shadow_strata[0].cs.deadstemc_transfer = stratum[0].cs.deadstemc_transfer;
    shadow_strata[0].cs.live_crootc = stratum[0].cs.live_crootc;
    shadow_strata[0].cs.livecrootc_store = stratum[0].cs.livecrootc_store;
    shadow_strata[0].cs.livecrootc_transfer = stratum[0].cs.livecrootc_transfer;
    shadow_strata[0].cs.dead_crootc = stratum[0].cs.dead_crootc;
    shadow_strata[0].cs.deadcrootc_store = stratum[0].cs.deadcrootc_store;
    shadow_strata[0].cs.deadcrootc_transfer = stratum[0].cs.deadcrootc_transfer;
    shadow_strata[0].cs.frootc = stratum[0].cs.frootc;
    shadow_strata[0].cs.frootc_store = stratum[0].cs.frootc_store;
    shadow_strata[0].cs.frootc_transfer = stratum[0].cs.frootc_transfer;
    shadow_strata[0].cs.cwdc = stratum[0].cs.cwdc;
    shadow_strata[0].epv.prev_leafcalloc = stratum[0].epv.prev_leafcalloc;
    shadow_strata[0].ns.npool = stratum[0].ns.npool;
    shadow_strata[0].ns.leafn = stratum[0].ns.leafn;
    shadow_strata[0].ns.dead_leafn = stratum[0].ns.dead_leafn;
    shadow_strata[0].ns.leafn_store = stratum[0].ns.leafn_store;
    shadow_strata[0].ns.leafn_transfer = stratum[0].ns.leafn_transfer;
    shadow_strata[0].ns.live_stemn = stratum[0].ns.live_stemn;
    shadow_strata[0].ns.livestemn_store = stratum[0].ns.livestemn_store;
    shadow_strata[0].ns.livestemn_transfer = stratum[0].ns.livestemn_transfer;
    shadow_strata[0].ns.dead_stemn = stratum[0].ns.dead_stemn;
    shadow_strata[0].ns.deadstemn_store = stratum[0].ns.deadstemn_store;
    shadow_strata[0].ns.deadstemn_transfer = stratum[0].ns.deadstemn_transfer;
    shadow_strata[0].ns.live_crootn = stratum[0].ns.live_crootn;
    shadow_strata[0].ns.livecrootn_store = stratum[0].ns.livecrootn_store;
    shadow_strata[0].ns.livecrootn_transfer = stratum[0].ns.livecrootn_transfer;
    shadow_strata[0].ns.dead_crootn = stratum[0].ns.dead_crootn;
    shadow_strata[0].ns.deadcrootn_store = stratum[0].ns.deadcrootn_store;
    shadow_strata[0].ns.deadcrootn_transfer = stratum[0].ns.deadcrootn_transfer;
    shadow_strata[0].ns.frootn = stratum[0].ns.frootn;
    shadow_strata[0].ns.frootn_store = stratum[0].ns.frootn_store;
    shadow_strata[0].ns.frootn_transfer = stratum[0].ns.frootn_transfer;
    shadow_strata[0].ns.cwdn = stratum[0].ns.cwdn;
    shadow_strata[0].ns.retransn = stratum[0].ns.retransn;
    shadow_strata[0].epv.wstress_days = stratum[0].epv.wstress_days;
    shadow_strata[0].epv.max_fparabs = stratum[0].epv.max_fparabs;
    shadow_strata[0].epv.min_vwc = stratum[0].epv.min_vwc;
  }
 
 /*if((((stratum[0].cs.leafc + stratum[0].cs.leafc_store + stratum[0].cs.leafc_transfer)*stratum[0].defaults[0][0].epc.proj_sla) >= (stratum[0].target.lai -	world[0].defaults[0].spinup[0].tolerance * stratum[0].target.lai))
    && ((stratum[0].cs.live_stemc + stratum[0].cs.dead_stemc) >= (stratum[0].target.total_stemc - world[0].defaults[0].spinup[0].tolerance * stratum[0].target.total_stemc))) {
    // TODO: add && for all the possible targets, add age
    stratum[0].target.met = 1;*/
     
  if (stratum[0].epv.proj_lai >= (stratum[0].target.lai - world[0].defaults[0].spinup[0].tolerance * stratum[0].target.lai)) {
    if ((stratum[0].cs.live_stemc + stratum[0].cs.dead_stemc) >= (stratum[0].target.total_stemc - world[0].defaults[0].spinup[0].tolerance * stratum[0].target.total_stemc)) {
      if (stratum[0].epv.height >= (stratum[0].target.height - world[0].defaults[0].spinup[0].tolerance * stratum[0].target.height)) {
        if(((current_date.year - command_line[0].start_date.year) > stratum[0].target.age) && current_date.month==9 && current_date.day==30){
          stratum[0].target.met = 1;
        }
      }
    }
  }

  if((current_date.year - command_line[0].start_date.year > world[0].defaults[0].spinup[0].max_years) && current_date.month==9 && current_date.day==30){
    stratum[0].target.met = 1;
    printf("\nexceeded max years for patch:%d", stratum[0].patch_ID);
  }	

	return;
} /*end update_shadow_strata.c*/
