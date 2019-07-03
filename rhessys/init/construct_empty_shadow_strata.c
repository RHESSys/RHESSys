/*------------------------------------------------------------------------------------------------------------------------------*/
/*                                                                                                                              */
/*                                      construct_empty_shadow_strata                                                           */
/*                                                                                                                              */
/*      construct_empty_shadow_strata.c - creates an empty shadow_strata object                                                 */
/*                                                                                                                              */
/*      NAME                                                                                                                    */
/*                                                                                                                              */
/*      SYNOPSIS                                                                                                                */
/*                                                                                                                              */
/*                                                                                                                              */
/*      OPTIONS                                                                                                                 */
/*                                                                                                                              */
/*      DESCRIPTION                                                                                                             */
/*                                                                                                                              */
/*      Allocates memory for an empty canopy strata object and reads in                                                         */
/*      parameters for the object from an open hillslopes file.                                                                 */
/*                                                                                                                              */
/*      Refer to construct_basin.c for a specification of the                                                                   */
/*      hillslopes file.                                                                                                        */
/*                                                                                                                              */
/*                                                                                                                              */
/*      PROGRAMMER NOTES                                                                                                        */
/*                                                                                                                              */
/*                                                                                                                              */
/*------------------------------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

struct canopy_strata_object *construct_empty_shadow_strata(
                                                                                                         struct command_line_object * command_line, 
                                                                                                         struct patch_object    *patch,
                                                                                                         struct canopy_strata_object *stratum,
                                                                                                         struct default_object  *defaults)
{
  
      /*--------------------------------------------------------------*/
        /*      Local function definition.                              */
        /*--------------------------------------------------------------*/
        struct base_station_object *assign_base_station(
                int ,
                int ,
                struct base_station_object **);         
        
        int compute_annual_turnover(struct epconst_struct,
                struct epvar_struct *,
                struct cstate_struct *);


        int     compute_annual_litfall(
                struct epconst_struct,
                struct phenology_struct *,
                struct cstate_struct *,
                int);
        
        int     update_rooting_depth(
                struct rooting_zone_object *,
                double,
                double,
                double,
                double);

        void    *alloc(size_t, char *, char *);
        /*--------------------------------------------------------------*/
        /*      Local variable definition.                              */
        /*--------------------------------------------------------------*/
        int     base_stationID;
        int     i;
        double  sai, rootc;
        int     default_object_ID;
        char    record[MAXSTR];
        struct  canopy_strata_object      *shadow_strata;
        
        /*--------------------------------------------------------------*/
        /*  Allocate a shadow_strata object.                            */
        /*--------------------------------------------------------------*/
        shadow_strata = (struct canopy_strata_object *) alloc( 1 *
                sizeof( struct canopy_strata_object ),"shadow_strata",
                "construct_shadow_strata" );
        
        /*--------------------------------------------------------------*/
        /*   Initialize the next canopy strata record for this patch.   */
        /*--------------------------------------------------------------*/
        shadow_strata[0].cover_fraction = NULLVAL;
        shadow_strata[0].gap_fraction = NULLVAL;
        shadow_strata[0].rootzone.depth = NULLVAL;
        shadow_strata[0].snow_stored = NULLVAL;
        shadow_strata[0].rain_stored = NULLVAL;
        shadow_strata[0].cs.cpool = NULLVAL;
        shadow_strata[0].cs.leafc = NULLVAL;
        shadow_strata[0].cs.dead_leafc = NULLVAL;
        shadow_strata[0].cs.leafc_store = NULLVAL;
        shadow_strata[0].cs.leafc_transfer = NULLVAL;
        shadow_strata[0].cs.live_stemc = NULLVAL;
        shadow_strata[0].cs.livestemc_store = NULLVAL;
        shadow_strata[0].cs.livestemc_transfer = NULLVAL;
        shadow_strata[0].cs.dead_stemc = NULLVAL;
        shadow_strata[0].cs.deadstemc_store = NULLVAL;
        shadow_strata[0].cs.deadstemc_transfer = NULLVAL;
        shadow_strata[0].cs.live_crootc = NULLVAL;
        shadow_strata[0].cs.livecrootc_store = NULLVAL;
        shadow_strata[0].cs.livecrootc_transfer = NULLVAL;
        shadow_strata[0].cs.dead_crootc = NULLVAL;
        shadow_strata[0].cs.deadcrootc_store = NULLVAL;
        shadow_strata[0].cs.deadcrootc_transfer = NULLVAL;
        shadow_strata[0].cs.frootc = NULLVAL;
        shadow_strata[0].cs.frootc_store = NULLVAL;
        shadow_strata[0].cs.frootc_transfer = NULLVAL;
        shadow_strata[0].cs.cwdc = NULLVAL;
        shadow_strata[0].epv.prev_leafcalloc = NULLVAL;
        shadow_strata[0].ns.npool = NULLVAL;
        shadow_strata[0].ns.leafn = NULLVAL;
        shadow_strata[0].ns.dead_leafn = NULLVAL;
        shadow_strata[0].ns.leafn_store = NULLVAL;
        shadow_strata[0].ns.leafn_transfer = NULLVAL;
        shadow_strata[0].ns.live_stemn = NULLVAL;
        shadow_strata[0].ns.livestemn_store = NULLVAL;
        shadow_strata[0].ns.livestemn_transfer = NULLVAL;
        shadow_strata[0].ns.dead_stemn = NULLVAL;
        shadow_strata[0].ns.deadstemn_store = NULLVAL;
        shadow_strata[0].ns.deadstemn_transfer = NULLVAL;
        shadow_strata[0].ns.live_crootn = NULLVAL;
        shadow_strata[0].ns.livecrootn_store = NULLVAL;
        shadow_strata[0].ns.livecrootn_transfer = NULLVAL;
        shadow_strata[0].ns.dead_crootn = NULLVAL;
        shadow_strata[0].ns.deadcrootn_store = NULLVAL;
        shadow_strata[0].ns.deadcrootn_transfer = NULLVAL;
        shadow_strata[0].ns.frootn = NULLVAL;
        shadow_strata[0].ns.frootn_store = NULLVAL;
        shadow_strata[0].ns.frootn_transfer = NULLVAL;
        shadow_strata[0].ns.cwdn = NULLVAL;
        shadow_strata[0].ns.retransn = NULLVAL;
        
        /*--------------------------------------------------------------*/
        /*      intialized annual flux variables                        */
        /*--------------------------------------------------------------*/
        shadow_strata[0].epv.wstress_days = NULLVAL;
        shadow_strata[0].epv.max_fparabs = NULLVAL;
        shadow_strata[0].epv.min_vwc = NULLVAL;
        
        /*--------------------------------------------------------------*/
        /*      Assign  defaults for this shadow_strata           */
        /*--------------------------------------------------------------*/
        shadow_strata[0].defaults = (struct stratum_default **)  
                alloc( sizeof(struct stratum_default *),"defaults",
                "construct_empty_shadow_strata" );
                
        /*--------------------------------------------------------------*/
        /*      assign number of  strata base stations                  */
        /*--------------------------------------------------------------*/
        shadow_strata[0].num_base_stations = patch[0].num_base_stations;
        /*--------------------------------------------------------------*/
        /*    Allocate a list of base stations for this strata.         */
        /*--------------------------------------------------------------*/
        shadow_strata[0].base_stations = (struct base_station_object **)
                alloc(shadow_strata[0].num_base_stations *
                sizeof(struct base_station_object *),"base_stations",
                "construct_empty_shadow_strata");
        /*--------------------------------------------------------------*/
        /* Read each base_station ID and then point to that base_station*/
        /*--------------------------------------------------------------*/
        for (i=0 ; i<shadow_strata[0].num_base_stations; i++){
                shadow_strata[0].base_stations[i] = stratum[0].base_stations[i];
        } /*end for*/
        return(shadow_strata);
} /*end construct_empty_shadow_strata.c*/
