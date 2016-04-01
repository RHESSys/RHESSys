#ifndef _PHYS_CONSTANTS_H_
#define _PHYS_CONSTANTS_H_


/* holds physical constants */
/* atmospheric constants */
/* from the definition of the standard atmosphere, as established
by the International CIvil Aviation Organization , and referenced in 

Iribane, J.V. and W.L. Godson, 1981.  Atmospheric Thermodynamics. 2nd
	Edition. D. Reidel Publishing Company, Dordrecht.  The Netherlands.
	(pp. 10,167-168,245)
*/

#define	G_STD	9.80665		/* (m2/s) standard gravitational accel.		*/
#define	P_STD	101325.0	/* (Pa)	standard pressure at 0.0 m elevation	*/
#define T_STD	288.15		/* (K) 	standard temp at 0.0 m elevation	*/
#define	MA	28.9644e-3	/* (kg/mol) molecular weight of air		*/
#define	MW	18.0148e-3	/* (kg/mol) molecular weight of water		*/
#define	CP	1010.0		/* (J/kg*K) specific heat of air		*/
#define	LR_STD	0.0065		/* (-K/m) standard temperature lapse rate	*/
#define	R	8.3143		/* (m3 PA / mol K) gas law constant		*/
#define	SBC	5.67e-8		/* (W/m2*K4) Stefan Boltzmann COnstant		*/
#define	EPS	0.6219		/* (MW/MA) unitless ratio of molecular weights	*/

#define HVAP 42.7       /* Heat of vaporization, kJ/mol */
#define KELVIN 273.16
#define PI	3.14159265359
#define SECONDS_PER_DAY 86400
#define ICE_DENSITY 917.0 	/* (kg/m3) density of ice */

/*physiological constants	*/
#define	RAD2PAR	0.5		/* (DIM) ratio PAR/ SWtotal			*/
#define	EPAR	4.55		/* (umol/J) PAR photon energy ratio		*/
#define SECPERRAD 13750.9871            /* seconds per radian of hour angle */

#define LITTER_ALBEDO 0.1 /* changed from 0.02 to 0.15 based on Oke 1987 */
#define WATER_ALBEDO 0.05	/* average liquid water albedo for solar angle of 60 (Dingman) */

#define LIVELAB_CN  50           /* C:N for labile fraction of live wood */
#define LIG_CN      500.0        /* C:N for all lignin components */
#define CEL_CN      250.0        /* C:N for all cellulose components */
#define SOIL1_CN    12.0         /* C:N for fast microbial recycling pool */
#define SOIL2_CN    12.0         /* C:N for slow microbial recycling pool */
#define SOIL3_CN    10.0         /* C:N for recalcitrant SOM pool (humus) */
#define SOIL4_CN    8.0         /* C:N for recalcitrant SOM pool (lignin) */

#define NONWOOD_GRPERC 1.2  /* (DIM) growth resp per unit of nonwood C grown */
#define WOOD_GRPERC    2.0  /* (DIM) growth resp per unit of woody C grown */


#define  PARTICLE_DENSITY	2.65	/* soil particle density g/cm3 (Dingman) */


#define NUM_VAR_BASIN 17
#define NUM_VAR_HILLSLOPE 18
#define NUM_VAR_ZONE 22
#define NUM_VAR_PATCH 47
#define NUM_VAR_STRATA 66



#endif


/*-----------------------------------------------------------------------------
 *  notes for value of variables:
 *  u: calculated from g2w or from user's reference value
 *  d: use default value
 *-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 *  Num of Variables (read by tag) in basin = 7, make it 17 
 *-----------------------------------------------------------------------------*/
/*
    basin_ID  //u
    x	      //d 0
    y	      //d 0
    z	      //u 
    basin_parm_ID //u
    latitude	  //u
    n_basestations //u this one is the indicator for end of the reading process
    num_hillslopes //u

*/

/*-----------------------------------------------------------------------------
 *  Num of Variables (read by tag) in hillslope = 8, make it 18
 *-----------------------------------------------------------------------------*/
/*
    hillslope_ID // u
    x		  // d 0 
    y		  // d 0
    z		  // u
    hill_parm_ID  // u
    gw.storage	  // d 0
    gw.NO3	  // d 0
    n_basestations// u 
    num_zones    //u
    
*/
 /*-----------------------------------------------------------------------------
  *  Num of variables (read by tag) in zone = 12, make it 22
  *-----------------------------------------------------------------------------*/
/*
    zone_ID	  //u
    x		  // d 0
    y		  // d 0
    z		  // u 
    zone_parm_ID  // u
    area	  // u 
    slope	  // u 
    aspect	  // u 
    precip_lapse_rate //d 1
    e_horizon	  // u
    w_horizon	  // u
    n_basestations // u
    zone_base_station_ID		    //u    
    num_patches			    //u

*/

/*-----------------------------------------------------------------------------
 *  Num of variables (read by tag) in patch = 37, make it 47
 *-----------------------------------------------------------------------------*/
/*
    patch_ID           // u 
    x		      // d 0 
    y		      // d 0 
    z		      // u 
    soil_parm_ID      // u 
    landuse_parm_ID   // u 
    fire_parm_ID      // u 
    surface_energy_parm_ID  // u
    area	      // u
    slope	      // u 
    lna		      // d 7
    Ksat_vertical     // d 1
    mpar	      // d 0
    std		      // d 0
    rz_storage	      // d 0
    unsat_storage     // d 0
    sat_deficit	      // d 1
    snowpack.water_equivalent_depth  // d 0
    snowpack.water_depth	    // d 0
    snowpack.T			    // d 0
    snowpack.surface_age	    // d 0
    snowpack.energy_deficit	    // d 0
    snow_redist_scale		    // d 1
    litter.cover_fraction	    // d 1
    litter.rain_stored		    // d 0
    litter_cs.litr1c		    // d janet
    litter_ns.litr1n		    // d janet
    litter_cs.litr2c		    // d 0
    litter_cs.litr3c		    // d 0
    litter_cs.litr4c		    // d 0
    soil_cs.soil1c		    // d 0
    soil_ns.sminn		    // d 0
    soil_ns.nitrate		    // d 0
    soil_cs.soil2c		    // d 0
    soil_cs.soil3c		    // d 0
    soil_cs.soil4c		    // d 0
    n_basestations		    // u
    num_canopy_strata		    // u
*/

 /*-----------------------------------------------------------------------------
  * Num of variables (read by tag) in canopy strata = 56, make it 66 
  *-----------------------------------------------------------------------------*/
/*
    canopy_strata_ID	      // u 
    veg_parm_ID		      // u 
    spinup_object_ID	      // u 
    cover_fraction	      // d 1
    gap_fraction	      // d 0
    rootzone.depth	      // u
    snow_stored		      // d 0
    rain_stored                     // d 0
    cs.cpool                        // d 0
    cs.leafc                        // d 0
    cs.dead_leafc                   // d 0
    cs.leafc_store                  // d 0
    cs.leafc_transfer               // d 0
    cs.live_stemc                   // d 0
    cs.livestemc_store     // d 0
    cs.livestemc_transfer  // d 0
    cs.dead_stemc          // d 0
    cs.deadstemc_store     // d 0
    cs.deadstemc_transfer  // d 0
    cs.live_crootc         // d 0
    cs.livecrootc_store    // d 0
    cs.livecrootc_transfer // d 0
    cs.dead_crootc         // d 0
    cs.deadcrootc_store    // d 0
    cs.deadcrootc_transfer // d 0
    cs.frootc              // d 0
    cs.frootc_store        // d 0
    cs.frootc_transfer     // d 0
    cs.cwdc                // d 0
    epv.prev_leafcalloc    // d 0
    ns.npool               // d 0
    ns.leafn               // d 0
    ns.dead_leafn          // d 0
    ns.leafn_store         // d 0
    ns.leafn_transfer      // d 0
    ns.live_stemn          // d 0
    ns.livestemn_store     // d 0
    ns.livestemn_transfer  // d 0
    ns.dead_stemn          // d 0
    ns.deadstemn_store     // d 0
    ns.deadstemn_transfer  // d 0
    ns.live_crootn         // d 0
    ns.livecrootn_store    // d 0
    ns.livecrootn_transfer // d 0
    ns.dead_crootn         // d 0
    ns.deadcrootn_store    // d 0
    ns.deadcrootn_transfer // d 0
    ns.frootn              // d 0
    ns.frootn_store        // d 0
    ns.frootn_transfer     // d 0
    ns.cwdn                // d 0
    ns.retransn            // d 0
    epv.wstress_days       // d 0
    epv.max_fparabs        // d 0
    epv.min_vwc            // d 0
    n_basestations         // u

*/





