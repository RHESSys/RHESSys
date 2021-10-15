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
/*	PROGRAMMER NOTES	   			                                   	*/
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "phys_constants.h"
#include "rhessys.h"

void	update_shadow_strata(
							   struct	world_object		      *world,
							   struct   zone_object               *zone,
							   struct   patch_object              *patch,
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

   struct canopy_strata_object *canopy_target;
	struct canopy_strata_object *canopy_subtarget;
	int c;
	int layer;
 // printf("\ntarget met: %d", stratum[0].target.met);

  if(stratum[0].target.met != 1){
    shadow_strata[0].cover_fraction = stratum[0].cover_fraction;
    shadow_strata[0].gap_fraction = stratum[0].gap_fraction;
    shadow_strata[0].rootzone.depth = stratum[0].rootzone.depth;
    shadow_strata[0].snow_stored = stratum[0].snow_stored;
    shadow_strata[0].rain_stored = stratum[0].rain_stored;
    shadow_strata[0].cs.stem_density = stratum[0].cs.stem_density;
    shadow_strata[0].cs.age = stratum[0].cs.age;
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
  //}

  /* use patch level LAI as target instead of only use overstory LAI NREN 20201203 */
   if (world[0].defaults[0].spinup[0].target_type == 2) {
    if (patch[0].lai >= patch[0].target.lai *(1 - world[0].defaults[0].spinup[0].tolerance) &&
        patch[0].total_stemc >= patch[0].target.total_stemc *(1 - world[0].defaults[0].spinup[0].tolerance) &&
        patch[0].height >= patch[0].target.height *(1 - world[0].defaults[0].spinup[0].tolerance) &&
        (current_date.year - command_line[0].start_date.year) > patch[0].target.age && current_date.month==9 && current_date.day==30)
    {
        stratum[0].target.met = 1;
        //printf("\n 1. for patch before check over-under ratio target: patchID is: %d, simulated patch LAI is %lf, target LAI is %lf, stratumID is %d, simulated stratum LAI is %lf \n", patch[0].ID, patch[0].lai, patch[0].target.lai, stratum[0].ID, stratum[0].epv.proj_lai);
        // prevent the patch reach target too soon because of understory fast grow at the beginning
        /*if ( stratum[0].epv.proj_lai >= 0.9*patch[0].target.lai *(1 - world[0].defaults[0].spinup[0].tolerance)  && stratum[0].ID < world[0].defaults[0].spinup[0].max_ID)
            {stratum[0].target.met = 1;} */
            //printf("\n 2. patch target meet2 after check over-under ratio for patchID is: %d, simulated patch LAI is: %lf, stratumID is: %d, simulate stratum LAI is %lf \n target LAI is %lf, target.met %d \n", patch[0].ID, patch[0].lai, stratum[0].ID, stratum[0].epv.proj_lai, patch[0].target.lai, stratum[0].target.met);

    } // if patchi lai



   } //if world

   /* add third option to use zone effective LAI as target to solve the MSR incompatible problem NR 20210105*/
   else if (world[0].defaults[0].spinup[0].target_type == 3) {
        if ( (zone[0].lai >= zone[0].target.lai *(1 - world[0].defaults[0].spinup[0].tolerance)) &&
             (zone[0].total_stemc >= zone[0].target.total_stemc * (1- world[0].defaults[0].spinup[0].tolerance)) &&
             (zone[0].height >= zone[0].target.height *(1 - world[0].defaults[0].spinup[0].tolerance)) &&
             (current_date.year - command_line[0].start_date.year) > zone[0].target.age && current_date.month==9 && current_date.day==30)
         {
                stratum[0].target.met = 1;
                 printf("\n 3. zone target meet zoneID is: %d, simulated zone effective LAI is: %lf, stratumID is: %d, simulate stratum LAI is %lf \n target LAI is %lf, target.met %d \n", zone[0].ID, zone[0].effective_lai, stratum[0].ID, stratum[0].epv.proj_lai, zone[0].target.lai, stratum[0].target.met);
        }

   }


  else if (world[0].defaults[0].spinup[0].target_type == 1){ //default is one

 if (stratum[0].epv.proj_lai >= (stratum[0].target.lai - world[0].defaults[0].spinup[0].tolerance * stratum[0].target.lai)) {
    if ((stratum[0].cs.live_stemc + stratum[0].cs.dead_stemc) >= (stratum[0].target.total_stemc - world[0].defaults[0].spinup[0].tolerance * stratum[0].target.total_stemc)) {
      if (stratum[0].epv.height >= (stratum[0].target.height - world[0].defaults[0].spinup[0].tolerance * stratum[0].target.height)) {
        if(((current_date.year - command_line[0].start_date.year) > stratum[0].target.age) && current_date.month==9 && current_date.day==30){
          stratum[0].target.met = 1;
        }
      }
    }
  }
}

 if((current_date.year - command_line[0].start_date.year > world[0].defaults[0].spinup[0].max_years) && current_date.month ==9 && current_date.day==30 && stratum[0].target.met != 1){
    stratum[0].target.met = 1;
    printf("\n exceeded max years for stratumID:%d, simluated stratum LAI is %lf, target LAI is %lf, simulate patch LAI is %lf  \n", stratum[0].ID, stratum[0].epv.proj_lai, stratum[0].target.lai, patch[0].lai);
  }

 }//52 if target not meet
	return;
} /*end update_shadow_strata.c*/
