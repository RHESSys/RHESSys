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


#define NUM_VAR_BASIN 7
#define NUM_VAR_HILLSLOPE 8
#define NUM_VAR_ZONE 12
#define NUM_VAR_PATCH 37
#define NUM_VAR_STRATA 56



#endif



/*-----------------------------------------------------------------------------
 *  Num of Variables (read by tag) in basin = 7
 *-----------------------------------------------------------------------------*/
/*
    basin_ID
    x
    y
    z
    default_ID
    latitude
    n_basestations //this one is fixed to be the indicator for end of the reading process
*/

/*-----------------------------------------------------------------------------
 *  Num of Variables (read by tag) in hillslope = 8
 *-----------------------------------------------------------------------------*/
/*
    hillslope_ID
    x
    y
    z
    default_ID
    gw_storage
    gw_NO3
    n_basestations
*/
 /*-----------------------------------------------------------------------------
  *  Num of variables (read by tag) in zone = 12
  *-----------------------------------------------------------------------------*/
/*
    zone ID // there is white space between, 'zone' and 'ID', need to fix to 'zone_ID'
    x
    y
    z
    default_ID
    area
    slope
    aspect
    isohyet
    e_horizon
    w_horizon
    n_basestations
*/

/*-----------------------------------------------------------------------------
 *  Num of variables (read by tag) in patch = 37 
 *-----------------------------------------------------------------------------*/
/*
    patch_ID
    x
    y
    z
    soil_default_ID
    landuse_default_ID
    fire_default_ID
    surface_energy_default_ID
    area
    slope
    lna
    Ksat_vertical
    m_par
    std
    rz_storag
    unsat_storage
    sat_deficit
    snowpack.water_equivalent_depth
    snowpack_water_depth
    snowpack_T
    snowpack_surface_age
    snowpack_energy_deficit
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
  * Num of variables (read by tag) in canopy strata = 56  
  *-----------------------------------------------------------------------------*/
/*
    canopy_strata_ID
    default_ID
    spinup_object_ID
    cover_fraction
    gap_fraction
    root_depth
    snow_stored
    rain_stored
    cs_cpool
    cs_leafc
    cs_dead_leafc
    cs_leafc_store
    cs_leafc_transfer
    cs_live_stemc
    cs_livestemc_store
    cs_livestemc_transfer
    cs_dead_stemc
    cs_deadstemc_store
    cs_deadstemc_transfer
    cs_live_crootc
    cs_livecrootc_store
    cs_livecrootc_transfer
    cs_dead_crootc
    cs_deadcrootc_store
    cs_deadcrootc_transfer
    cs_frootc
    cs_frootc_store
    cs_frootc_transfer
    cs_cwdc
    epv.prev_leafcalloc
    ns_npool
    ns_leafn
    ns_dead_leafn
    ns_leafn_store
    ns_leafn_transfer
    ns_live_stemn
    ns_livestemn_store
    ns_livestemn_transfer
    ns_dead_stemn
    ns_deadstemn_store
    ns_deadstemn_transfer
    ns_live_crootn
    ns_livecrootn_store
    ns_livecrootn_transfer
    ns_dead_crootn
    ns_deadcrootn_store
    ns_deadcrootn_transfer
    ns_frootn
    ns_frootn_store
    ns_frootn_transfer
    ns_cwdn
    ns_retransn
    epv_wstress_days
    epv_max_fparabs
    epv_min_vwc
    n_basestations

*/





