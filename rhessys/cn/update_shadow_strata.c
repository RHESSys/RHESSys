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
                 struct canopy_strata_object  *empty_shadow_strata,
							   struct command_line_object	  *command_line,
                 struct default_object        *defaults, 
                 struct target_object         *target,
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

  if(target->met == 0){
    empty_shadow_strata[0].ID = stratum[0].ID;
   //  default_object_ID = stratum[0].default_object_ID; // Do I need this line? It's a NULLVAL in construct_empty - EJH
    empty_shadow_strata[0].cover_fraction = stratum[0].cover_fraction;
    empty_shadow_strata[0].gap_fraction = stratum[0].gap_fraction;
    empty_shadow_strata[0].rootzone.depth = stratum[0].rootzone.depth;
    empty_shadow_strata[0].snow_stored = stratum[0].snow_stored;
    empty_shadow_strata[0].rain_stored = stratum[0].rain_stored;
    empty_shadow_strata[0].cs.cpool = stratum[0].cs.cpool;
    empty_shadow_strata[0].cs.leafc = stratum[0].cs.leafc;
    empty_shadow_strata[0].cs.dead_leafc = stratum[0].cs.dead_leafc;
    empty_shadow_strata[0].cs.leafc_store = stratum[0].cs.leafc_store;
    empty_shadow_strata[0].cs.leafc_transfer = stratum[0].cs.leafc_transfer;
    empty_shadow_strata[0].cs.live_stemc = stratum[0].cs.live_stemc;
    empty_shadow_strata[0].cs.livestemc_store = stratum[0].cs.livestemc_store;
    empty_shadow_strata[0].cs.livestemc_transfer = stratum[0].cs.livestemc_transfer;
    empty_shadow_strata[0].cs.dead_stemc = stratum[0].cs.dead_stemc;
    empty_shadow_strata[0].cs.deadstemc_store = stratum[0].cs.deadstemc_store;
    empty_shadow_strata[0].cs.deadstemc_transfer = stratum[0].cs.deadstemc_transfer;
    empty_shadow_strata[0].cs.live_crootc = stratum[0].cs.live_crootc;
    empty_shadow_strata[0].cs.livecrootc_store = stratum[0].cs.livecrootc_store;
    empty_shadow_strata[0].cs.livecrootc_transfer = stratum[0].cs.livecrootc_transfer;
    empty_shadow_strata[0].cs.dead_crootc = stratum[0].cs.dead_crootc;
    empty_shadow_strata[0].cs.deadcrootc_store = stratum[0].cs.deadcrootc_store;
    empty_shadow_strata[0].cs.deadcrootc_transfer = stratum[0].cs.deadcrootc_transfer;
    empty_shadow_strata[0].cs.frootc = stratum[0].cs.frootc;
    empty_shadow_strata[0].cs.frootc_store = stratum[0].cs.frootc_store;
    empty_shadow_strata[0].cs.frootc_transfer = stratum[0].cs.frootc_transfer;
    empty_shadow_strata[0].cs.cwdc = stratum[0].cs.cwdc;
    empty_shadow_strata[0].epv.prev_leafcalloc = stratum[0].epv.prev_leafcalloc;
    empty_shadow_strata[0].ns.npool = stratum[0].ns.npool;
    empty_shadow_strata[0].ns.leafn = stratum[0].ns.leafn;
    empty_shadow_strata[0].ns.dead_leafn = stratum[0].ns.dead_leafn;
    empty_shadow_strata[0].ns.leafn_store = stratum[0].ns.leafn_store;
    empty_shadow_strata[0].ns.leafn_transfer = stratum[0].ns.leafn_transfer;
    empty_shadow_strata[0].ns.live_stemn = stratum[0].ns.live_stemn;
    empty_shadow_strata[0].ns.livestemn_store = stratum[0].ns.livestemn_store;
    empty_shadow_strata[0].ns.livestemn_transfer = stratum[0].ns.livestemn_transfer;
    empty_shadow_strata[0].ns.dead_stemn = stratum[0].ns.dead_stemn;
    empty_shadow_strata[0].ns.deadstemn_store = stratum[0].ns.deadstemn_store;
    empty_shadow_strata[0].ns.deadstemn_transfer = stratum[0].ns.deadstemn_transfer;
    empty_shadow_strata[0].ns.live_crootn = stratum[0].ns.live_crootn;
    empty_shadow_strata[0].ns.livecrootn_store = stratum[0].ns.livecrootn_store;
    empty_shadow_strata[0].ns.livecrootn_transfer = stratum[0].ns.livecrootn_transfer;
    empty_shadow_strata[0].ns.dead_crootn = stratum[0].ns.dead_crootn;
    empty_shadow_strata[0].ns.deadcrootn_store = stratum[0].ns.deadcrootn_store;
    empty_shadow_strata[0].ns.deadcrootn_transfer = stratum[0].ns.deadcrootn_transfer;
    empty_shadow_strata[0].ns.frootn = stratum[0].ns.frootn;
    empty_shadow_strata[0].ns.frootn_store = stratum[0].ns.frootn_store;
    empty_shadow_strata[0].ns.frootn_transfer = stratum[0].ns.frootn_transfer;
    empty_shadow_strata[0].ns.cwdn = stratum[0].ns.cwdn;
    empty_shadow_strata[0].ns.retransn = stratum[0].ns.retransn;
  }
  
  if((abs(stratum[0].epv.proj_lai - target->lai) <	world[0].defaults[0].spinup[0].tolerance * target->lai)
    && (abs(stratum[0].cs.live_stemc + stratum[0].cs.dead_stemc - target->total_stemc) < world[0].defaults[0].spinup[0].tolerance * target->total_stemc)) {
    // TODO: add && for all the possible targets
    target->met = 1;
  }

  if(current_date.year - command_line[0].start_date.year > world[0].defaults[0].spinup[0].max_years){
    target->met = 1;
  }	

	return;
} /*end update_shadow_strata.c*/
