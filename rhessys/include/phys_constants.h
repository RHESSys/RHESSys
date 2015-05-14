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
#define seconds_per_day 86400
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
 *  Num of Variables (read by tag) in basin = 7, make it 17 
 *-----------------------------------------------------------------------------*/
/*
    basin_ID
    x
    y
    z
    basin_parm_ID
    latitude
    n_basestations //this one is fixed to be the indicator for end of the reading process
*/

/*-----------------------------------------------------------------------------
 *  Num of Variables (read by tag) in hillslope = 8, make it 18
 *-----------------------------------------------------------------------------*/
/*
    hillslope_ID
    x
    y
    z
    hill_parm_ID
    gw.storage
    gw.NO3
    n_basestations
*/
 /*-----------------------------------------------------------------------------
  *  Num of variables (read by tag) in zone = 12, make it 22
  *-----------------------------------------------------------------------------*/
/*
    zone_ID
    x
    y
    z
    zone_parm_ID
    area
    slope
    aspect
    precip_lapse_rate
    e_horizon
    w_horizon
    n_basestations
*/

/*-----------------------------------------------------------------------------
 *  Num of variables (read by tag) in patch = 37, make it 47
 *-----------------------------------------------------------------------------*/
/*
    patch_ID
    x
    y
    z
    soil_parm_ID
    landuse_parm_ID
    fire_parm_ID
    surface_energy_parm_ID
    area
    slope
    lna
    Ksat_vertical
    mpar
    std
    rz_storage
    unsat_storage
    sat_deficit
    snowpack.water_equivalent_depth
    snowpack.water_depth
    snowpack.T
    snowpack.surface_age
    snowpack.energy_deficit
    snow_redist_scale
    litter.cover_fraction
    litter.rain_stored
    litter_cs.litr1c
    litter_ns.litr1n
    litter_cs.litr2c
    litter_cs.litr3c
    litter_cs.litr4c
    soil_cs.soil1c
    soil_ns.sminn
    soil_ns.nitrate
    soil_cs.soil2c
    soil_cs.soil3c
    soil_cs.soil4c
    n_basestations
*/

 /*-----------------------------------------------------------------------------
  * Num of variables (read by tag) in canopy strata = 56, make it 66 
  *-----------------------------------------------------------------------------*/
/*
    canopy_strata_ID
    veg_parm_ID
    spinup_object_ID
    cover_fraction
    gap_fraction
    rootzone.depth
    snow_stored
    rain_stored
    cs.cpool
    cs.leafc
    cs.dead_leafc
    cs.leafc_store
    cs.leafc_transfer
    cs.live_stemc
    cs.livestemc_store
    cs.livestemc_transfer
    cs.dead_stemc
    cs.deadstemc_store
    cs.deadstemc_transfer
    cs.live_crootc
    cs.livecrootc_store
    cs.livecrootc_transfer
    cs.dead_crootc
    cs.deadcrootc_store
    cs.deadcrootc_transfer
    cs.frootc
    cs.frootc_store
    cs.frootc_transfer
    cs.cwdc
    epv.prev_leafcalloc
    ns.npool
    ns.leafn
    ns.dead_leafn
    ns.leafn_store
    ns.leafn_transfer
    ns.live_stemn
    ns.livestemn_store
    ns.livestemn_transfer
    ns.dead_stemn
    ns.deadstemn_store
    ns.deadstemn_transfer
    ns.live_crootn
    ns.livecrootn_store
    ns.livecrootn_transfer
    ns.dead_crootn
    ns.deadcrootn_store
    ns.deadcrootn_transfer
    ns.frootn
    ns.frootn_store
    ns.frootn_transfer
    ns.cwdn
    ns.retransn
    epv.wstress_days
    epv.max_fparabs
    epv.min_vwc
    n_basestations

*/





