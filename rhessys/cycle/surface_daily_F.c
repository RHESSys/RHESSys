
/*--------------------------------------------------------------*/
/* 								*/
/*			surface_daily_F			*/
/*								*/
/*	NAME							*/
/*	canopy_statrum_daily_F 					*/
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	void surface_daily_F 				*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*	Feb 2010 AD											*/
/*	Added detention store evaporation.					*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

void		surface_daily_F(
							struct	world_object		*world,
							struct	basin_object		*basin,
							struct	hillslope_object	*hillslope, 
							struct	zone_object		*zone,
							struct	patch_object		*patch,
							struct 	command_line_object	*command_line,
							struct	tec_entry		*event,
							struct 	date 			current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	
	double	compute_diffuse_radiative_fluxes(
		int,
		double *,
		double,
		double,
		double,
		double,
		double,
		double);


	
	double	compute_direct_radiative_fluxes(
		int,
		double *,
		double,
		double,
		double,
		double,
		double,
		double);
	
	
	double	compute_direct_radiative_PAR_fluxes(
		int,
		double *,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);

	
	double	compute_nonvascular_stratum_conductance(
		int	,
		double	,
		double	,
		double	,
		double	,
		double	);

	double	compute_litter_rain_stored(
		int,
		struct	patch_object *);
	
	double	penman_monteith(
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		int);


	double  compute_subsurface_temperature_profile(
		struct	surface_energy_object *,
		struct surface_energy_default *,
		double,
		double,
		double *);
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	double  detention_store_evaporation;
	double  detention_store_potential_evaporation;
	double  detention_store_potential_dry_evaporation_rate;
	double  detention_store_potential_rainy_evaporation_rate;
	double  albedo;
	double	dry_evaporation;
	double	Kstar_direct;
	double	Kstar_diffuse;
	double	APAR_diffuse;
	double	APAR_direct;
	double	potential_evaporation_rate;
	double	potential_rainy_evaporation_rate;
	double	rainy_evaporation;
	double	rnet_evap, rnet_evap_pond, rnet_evap_litter, rnet_evap_soil;
	double  rnet;
	double	PE_rate, PE_rainy_rate;
	double	soil_potential_evaporation;
	double	soil_potential_dry_evaporation_rate;
	double	soil_potential_rainy_evaporation_rate;
	double	exfiltration;
	double	K_used;
	double	K_initial;
	double	hv;			/* latent heat of vaporization for water (KJ/kg) */
	double	water_density;		/* density of water in kg/m^3 */
	double 	fraction_direct_K;
	double	fraction_diffuse_K;
	double	fraction_Lstar_soil;
	double	fraction_surface_heat_flux;
	struct	litter_object	*litter;
	

	/*--------------------------------------------------------------*/
	/*	Initialize litter variables.				*/
	/*--------------------------------------------------------------*/
	Kstar_diffuse = 0.0;
	APAR_diffuse = 0.0;
	Kstar_direct = 0.0;
	APAR_direct = 0.0;
	exfiltration = 0;
	rainy_evaporation = 0;
	dry_evaporation = 0;
	rnet = 0.0;
	
	litter = &(patch[0].litter);
	hv = 2.495 * 1e3;	/* latent heat of vaporization for water (KJ/kg) */
	water_density = 1000;	/* density of water in kg/m^3 */

	/*--------------------------------------------------------------*/
	/*	DETENTION STORE EVAPORATION:								*/
	/*	Check to see if detention store is greater than current		*/
	/*	litter holding capacity.  If so, run water surface evap		*/
	/*	first.														*/
	/* but only do this if there is actually a detention_store capacity to hold water */
	/*--------------------------------------------------------------*/


    	if ( (patch[0].detention_store > (max(litter[0].rain_capacity - litter[0].rain_stored, 0.0)))
                                        && (patch[0].soil_defaults[0][0].detention_store_size > 0.0)) {

		/* assume if det store over litter then litter is saturated */
		litter[0].rain_stored = litter[0].rain_capacity;
		patch[0].detention_store -= (litter[0].rain_capacity - litter[0].rain_stored);	
	
			/*** Calculate available energy at surface. Assumes Kdowns are partially ***/
			/*** reflected by water surface based on water albedo. ***/
			if (zone[0].metv.dayl > ZERO) {
				rnet_evap_pond = 1000 * ( (1-WATER_ALBEDO) * (patch[0].Kdown_direct + patch[0].Kdown_diffuse) - 
						patch[0].Lstar_soil + patch[0].surface_heat_flux) / zone[0].metv.dayl;
				if (rnet_evap_pond <= ZERO) {
					rnet_evap_pond = 0.0;
				}
			}
			else rnet_evap_pond = 0.0;
	
			/*** Use Penman with rsurface=0 for open water evaporation. ***/
	
			patch[0].ga = max((patch[0].ga * patch[0].stability_correction),0.0001);
		
			detention_store_potential_dry_evaporation_rate = penman_monteith(
				command_line[0].verbose_flag,
				zone[0].metv.tday,
				zone[0].metv.pa,
				zone[0].metv.vpd,
				rnet_evap_pond,
				0.0,
				1/(patch[0].ga),
				2) ;
			detention_store_potential_rainy_evaporation_rate = penman_monteith(
				command_line[0].verbose_flag,
				zone[0].metv.tday,
				zone[0].metv.pa,
				10,
				rnet_evap_pond,
				0.0,
				1/(patch[0].ga),
				2) ;
	
			detention_store_potential_evaporation  = detention_store_potential_dry_evaporation_rate
					* (zone[0].metv.dayl - zone[0].daytime_rain_duration )
					+ detention_store_potential_rainy_evaporation_rate
					* zone[0].daytime_rain_duration;

			// Avoid over-estimating ET from surfaces with no detention store size 
			//   (e.g. impervious surface) by gating ET by detention_store_size
			detention_store_evaporation = min(detention_store_potential_evaporation,
							  min(patch[0].detention_store, 
							      patch[0].soil_defaults[0][0].detention_store_size) );
	
	}
	
	else detention_store_evaporation = 0.0;
	patch[0].detention_store -= detention_store_evaporation;

	/*** If snowpack is over detention store, then add evaporated water to snowpack. ***/
	
	if (patch[0].snowpack.water_equivalent_depth > ZERO) {
		patch[0].snowpack.water_equivalent_depth += detention_store_evaporation;
		detention_store_evaporation = 0.0;
	}
	
	patch[0].evaporation_surf += detention_store_evaporation;
	
	
	/*** Update patch radiation to remove any radiation used to evaporate pond. ***/
	/*** Assume same proportions as original 4 energy sources. ***/
	
	K_used =  detention_store_evaporation * hv * water_density;
	K_initial = (1-WATER_ALBEDO) * (patch[0].Kdown_direct + patch[0].Kdown_diffuse)
					- patch[0].Lstar_soil + patch[0].surface_heat_flux;
	if ( K_used > 0 ){
		if ( K_initial > 0 ){
			fraction_direct_K = (1-WATER_ALBEDO) * patch[0].Kdown_direct / K_initial;
			fraction_diffuse_K = (1-WATER_ALBEDO) * patch[0].Kdown_diffuse / K_initial;
			fraction_Lstar_soil = patch[0].Lstar_soil / K_initial;
			fraction_surface_heat_flux = patch[0].surface_heat_flux / K_initial;
		}
		else {
			fraction_direct_K = 0.0;
			fraction_diffuse_K = 0.0;
			fraction_Lstar_soil = 0.0;
			fraction_surface_heat_flux = 0.0;
		}
		patch[0].Kdown_direct = ((1-WATER_ALBEDO) * patch[0].Kdown_direct) - (fraction_direct_K * K_used);
		patch[0].Kdown_diffuse = ((1-WATER_ALBEDO) * patch[0].Kdown_diffuse) - (fraction_diffuse_K * K_used);
		patch[0].Lstar_soil = patch[0].Lstar_soil - (fraction_Lstar_soil * K_used);
		patch[0].surface_heat_flux = patch[0].surface_heat_flux - (fraction_surface_heat_flux * K_used);
	}
	
	
	/*--------------------------------------------------------------*/
	/*	LITTER STORE EVAPORATION:									*/
	/*	Now move to litter and then bare soil evaporation if the 	*/
	/*	remaining surface water can be held by the litter.			*/
	/*--------------------------------------------------------------*/
	

	if ( patch[0].detention_store <= (litter[0].rain_capacity - litter[0].rain_stored) ) {
		
		/*--------------------------------------------------------------*/
		/*	calculate surface albedo as a function of amount of	*/
		/*		litter vs soil					*/
		/*--------------------------------------------------------------*/
		if (litter->proj_pai >= 1.0)
			albedo = LITTER_ALBEDO;
		else {
			albedo = LITTER_ALBEDO * litter->proj_pai
				+ patch[0].soil_defaults[0][0].albedo * (1-litter->proj_pai);
		}

		/*--------------------------------------------------------------*/
		/*  Intercept diffuse radiation.                                */
		/*  We assume that the zone slope == patch slope.               */
		/*  We also assume that radiation reflected into the upper      */
		/*      hemisphere is lost.                                     */
		/*  We do not make adjustements for chaanging gap fraction over */
		/*      the diurnal cycle - using a suitable mean gap fraction  */
		/*      instead.                                                */
		/*  We do take into account the patch level horizon which will  */
		/*      allow simulation of clear-cuts/clearings with low sza's */
		/*  Note that for zone level temperature and radiation          */
		/*      computation we took into account only zone level horizon*/
		/*      since we figured that climate above the zone was well   */
		/*      mixed.                                                  */
		/*--------------------------------------------------------------*/
		Kstar_diffuse = compute_diffuse_radiative_fluxes(
			command_line[0].verbose_flag,
			&(patch[0].Kdown_diffuse),
			patch[0].Kdown_direct,
			-10000.0,
			0.0,
			1.0,
			basin[0].theta_noon,
			albedo);
	
		APAR_diffuse = compute_diffuse_radiative_fluxes(
			command_line[0].verbose_flag,
			&(patch[0].PAR_diffuse),
			patch[0].PAR_direct,
			-10000.0,
			0.0,
			1.0,
			basin[0].theta_noon,
			albedo);
		/*--------------------------------------------------------------*/
		/*  Intercept direct radiation.                                 */
		/*      hard to measure for each strata.  We could use top      */
		/*      of canopy albedo but this integrates the effect of the  */
		/*      entire canopy.  Furthermore, it requires in general     */
		/*      knowledge of the canopy BRDF - which we want to avoid.  */
		/*      Instead we assume a certain reflectance and             */
		/*      transmittance for each strata's canopy elements.        */
		/*  We assume that the zone slope == patch slope.               */
		/*  We also assume that radiation reflected into the upper      */
		/*      hemisphere is lost.                                     */
		/*  We do not make adjustements for chaanging gap fraction over */
		/*      the diurnal cycle - using a suitable mean gap fraction  */
		/*      instead.                                                */
		/*  We do take into account the patch level horizon which will  */
		/*      allow simulation of clear-cuts/clearings with low sza's */
		/*  Note that for zone level temperature and radiation          */
		/*      computation we took into account only zone level horizon*/
		/*      since we figured that climate above the zone was well   */
		/*      mixed.                                                  */
		/*--------------------------------------------------------------*/
		Kstar_direct = compute_direct_radiative_fluxes(
			command_line[0].verbose_flag,
			&(patch[0].Kdown_direct),
			-10000,
			0.0,
			1.0,
			basin[0].theta_noon,
			albedo,
			albedo);
		APAR_direct = compute_direct_radiative_fluxes(
			command_line[0].verbose_flag,
			&(patch[0].PAR_direct),
			-10000,
			0.0,
			1.0,
			basin[0].theta_noon,
			albedo,
			albedo);
									
		/*--------------------------------------------------------------*/
		/*	Determine non-vascular condductance to evaporation.		*/
		/*	This conductance represnets the inverse of an additional 	*/
		/*	resistance to vapour flux from the stratum rian storage		*/
		/*	surface over and above aerodynamic resistances that also	*/
		/*	affect turbulent heat transfer.  						 	*/
		/*	                            								*/
		/*	A linear relationship is currently assumed with the amount	*/
		/*	of relative rain stored - or with the patch unsat zone storage  */
		/*	relative to water equiv depth top water table if roots are present */
		/*	.  Parameters for the relationship	*/
		/*	are supplied via the stratum default file.					*/
		/*--------------------------------------------------------------*/
		litter[0].gsurf = compute_nonvascular_stratum_conductance(
				command_line[0].verbose_flag,
				litter[0].rain_stored + patch[0].detention_store,
				litter[0].rain_capacity,
				litter[0].gl_c,	
				litter[0].gsurf_slope,
				litter[0].gsurf_intercept);
		
		if (patch[0].rootzone.depth > ZERO) {
			patch[0].gsurf = compute_nonvascular_stratum_conductance(
				command_line[0].verbose_flag,
				patch[0].rz_storage + patch[0].detention_store,
				patch[0].sat_deficit,
				patch[0].soil_defaults[0][0].gl_c,	
				patch[0].soil_defaults[0][0].gsurf_slope,
				patch[0].soil_defaults[0][0].gsurf_intercept);
		}

		else {
			patch[0].gsurf = compute_nonvascular_stratum_conductance(
				command_line[0].verbose_flag,
				patch[0].unsat_storage + patch[0].detention_store,
				patch[0].sat_deficit,
				patch[0].soil_defaults[0][0].gl_c,	
				patch[0].soil_defaults[0][0].gsurf_slope,
				patch[0].soil_defaults[0][0].gsurf_intercept);
		}
		/*--------------------------------------------------------------*/
		/*	COmpute evaporation and exfiltration RATES (m/s)	*/
		/*	for daylight period .					*/
		/*	The rainy rate assumes a vpd of 10Pa.			*/
		/*	Note if surface heat flux makes evap negative we	*/
		/*	have condensation.  					*/
		/*--------------------------------------------------------------*/
		if(zone[0].metv.dayl > ZERO) {
		rnet_evap = 1000 * (Kstar_direct + Kstar_diffuse - patch[0].Lstar_soil
			+ patch[0].surface_heat_flux) / zone[0].metv.dayl;
			if (rnet_evap <= ZERO) {
					rnet_evap_pond = 0.0;
			}
		}

		else rnet_evap = 0.0;

		rnet_evap_litter = min(litter->proj_pai,1)*rnet_evap;
		rnet_evap_soil = max(0.0, rnet_evap-rnet_evap_litter);

		if (rnet_evap < 0.0) rnet_evap = 0.0;
		/*--------------------------------------------------------------*/
		/*	Make sure ga and gsurf are non-zero.			*/
		/*--------------------------------------------------------------*/
		patch[0].ga = max((patch[0].ga * patch[0].stability_correction),0.0001);
		/*--------------------------------------------------------------*/
		/*	Estimate potential evap rates.				*/
		/*--------------------------------------------------------------*/
		potential_evaporation_rate = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			zone[0].metv.vpd,
			rnet_evap_litter,
			1/patch[0].litter.gsurf,
			1/(patch[0].ga),
			2) ;
		potential_rainy_evaporation_rate = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			10,
			rnet_evap_litter,
			1/patch[0].litter.gsurf,
			1/(patch[0].ga),
			2) ;
		PE_rate = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			zone[0].metv.vpd,
			rnet_evap_litter,
			0.0,
			1/(patch[0].ga),
			2) ;
		PE_rainy_rate = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			10,
			rnet_evap_litter,
			0.0,
			1/(patch[0].ga),
			2) ;
		
		PE_rainy_rate = max(0, PE_rainy_rate);
		PE_rate = max(0, PE_rate);
		potential_evaporation_rate = max(0,potential_evaporation_rate);
		potential_rainy_evaporation_rate = max(0,potential_rainy_evaporation_rate);
		/*--------------------------------------------------------------*/
		/*	Do not allow negative potential evap if it raining	*/
		/*	since condensation/dew dep is the same as rain		*/
		/*--------------------------------------------------------------*/
		if ( zone[0].rain + zone[0].rain_hourly_total > 0 ){
			potential_evaporation_rate = max(0,potential_evaporation_rate);
			potential_rainy_evaporation_rate =
				max(0,potential_rainy_evaporation_rate);
		}
		/*--------------------------------------------------------------*/
		/*	Compute potential evaporation of litter. 		*/
		/*	Weighted by rain and non rain periods of the daytime	*/
		/*	m/day = m/s * (sec/day)					*/
		/*								*/
		/*	Note that Kstar is converted from Kj/m2*day to W/m2	*/
		/*--------------------------------------------------------------*/
		patch[0].potential_evaporation  = potential_evaporation_rate
			* (zone[0].metv.dayl - zone[0].daytime_rain_duration )
			+ potential_rainy_evaporation_rate * zone[0].daytime_rain_duration;
		patch[0].PE  = PE_rate 
			* (zone[0].metv.dayl - zone[0].daytime_rain_duration )
			+ PE_rainy_rate * zone[0].daytime_rain_duration;

		/*--------------------------------------------------------------*/
		/*	Update rain storage ( this also updates the patch level	*/
		/*	detention store and potential_evaporation	*/
		/*--------------------------------------------------------------*/
		litter[0].rain_stored  = compute_litter_rain_stored(
			command_line[0].verbose_flag,
			patch);
		/*--------------------------------------------------------------*/
		/*	if litter is empty then we have bare soil		*/
		/*								*/
		/*	We assume that the exfiltration rate is limited to 	*/
		/*	the minimum of the potential evap rate for the 		*/
		/*	soil and the max exfiltration rate.			*/
		/*	The potential evap rate is estimated using P-M with 	*/
		/*	the same aero cond as the litter but with a 		*/
		/*	surface cond estimated based on vapour conductance in 	*/
		/*	soil column.						*/
		/*								*/
		/*	The surface heat flux of the soil column is estimated	*/
		/*	aasuming no litter covering the surface (0 m height).	*/
		/*--------------------------------------------------------------*/
		
			soil_potential_rainy_evaporation_rate = penman_monteith(
				command_line[0].verbose_flag,
				zone[0].metv.tday,
				zone[0].metv.pa,
				10.0,
				rnet_evap_soil,
				1.0/patch[0].gsurf,
				1.0/patch[0].ga,
				2);
			soil_potential_dry_evaporation_rate = penman_monteith(
				command_line[0].verbose_flag,
				zone[0].metv.tday,
				zone[0].metv.pa,
				zone[0].metv.vpd,
				rnet_evap_soil,
				1.0/patch[0].gsurf,
				1.0/patch[0].ga,
				2);
			soil_potential_evaporation  = soil_potential_dry_evaporation_rate
				* (zone[0].metv.dayl - zone[0].daytime_rain_duration )
				+ soil_potential_rainy_evaporation_rate
				* zone[0].daytime_rain_duration;

			/*--------------------------------------------------------------*/
			/*	BARE SOIL EVAPORATION:									*/
			/*	base soil evapotration/ exfiltration will only occur 	*/
			/*	on exposed soil layers					*/
			/*--------------------------------------------------------------*/
			exfiltration =	min(soil_potential_evaporation,
				patch[0].potential_exfiltration);
		
			
		if ( patch[0].sat_deficit_z > 0 ){
			patch[0].exfiltration_unsat_zone = exfiltration;
			patch[0].exfiltration_sat_zone = 0;
		}
		else{
			patch[0].exfiltration_unsat_zone = 0;
			patch[0].exfiltration_sat_zone = exfiltration;
		}
		
	}

	/*--------------------------------------------------------------*/
	/* if we want to solve surface temperature profile, determine 	*/
	/* moistures and depths to send to solver			*/
	/*	then call solver compute_subsurface_temperature_profile */
	/* will also determine % of soil water that is frozen		*/
	/* note that depth for layer 3 is always the soil depth so 	*/
	/* this is set only once in construct_patch			*/
	/*--------------------------------------------------------------*/
	
	if (command_line[0].surface_energy_flag == 1) {
		patch[0].surface_energy_profile[0].depth = patch[0].litter.depth;
		patch[0].surface_energy_profile[0].moisture = patch[0].litter.rain_stored;

	if (patch[0].sat_deficit > patch[0].rootzone.potential_sat) {
		patch[0].surface_energy_profile[1].depth = 
			patch[0].surface_energy_profile[0].depth + patch[0].rootzone.depth;
		patch[0].surface_energy_profile[1].moisture = patch[0].rz_storage;
		patch[0].surface_energy_profile[2].depth = 
			patch[0].surface_energy_profile[0].depth + patch[0].sat_deficit_z;
		patch[0].surface_energy_profile[2].moisture = patch[0].unsat_storage;
		patch[0].surface_energy_profile[3].moisture = patch[0].soil_defaults[0][0].soil_water_cap - patch[0].sat_deficit;	
		}
	else {
		patch[0].surface_energy_profile[1].depth = 
			patch[0].surface_energy_profile[0].depth + patch[0].sat_deficit_z;
		patch[0].surface_energy_profile[1].moisture = patch[0].rz_storage;
		
		patch[0].surface_energy_profile[2].depth = 
			patch[0].surface_energy_profile[0].depth + patch[0].rootzone.depth;
		patch[0].surface_energy_profile[2].moisture = patch[0].rootzone.potential_sat - patch[0].sat_deficit;
		patch[0].surface_energy_profile[3].moisture = patch[0].soil_defaults[0][0].soil_water_cap - patch[0].rootzone.potential_sat;	
		}

	compute_subsurface_temperature_profile(
				patch[0].surface_energy_profile,
				patch[0].surface_energy_defaults[0],
				zone[0].metv.tavg,
				rnet,
				&(patch[0].percent_soil_water_unfrozen));

	/*--------------------------------------------------------------*/
	/* map iterative temperatures back to soil and litter, for now  */
	/* we don't differentiate between unsat, rootzone and sat for temperature */
	/*--------------------------------------------------------------*/

	patch[0].litter.T = patch[0].surface_energy_profile[0].T;
	if (patch[0].sat_deficit > patch[0].rootzone.potential_sat) 
		patch[0].rootzone.T = patch[0].surface_energy_profile[1].T;
	else
		patch[0].rootzone.T = (patch[0].surface_energy_profile[1].T * (patch[0].sat_deficit) + 
					patch[0].surface_energy_profile[2].T * (patch[0].rootzone.potential_sat - patch[0].sat_deficit)) 
					/ (patch[0].rootzone.potential_sat);

	patch[0].Tsoil = (patch[0].surface_energy_profile[1].T * 
			(patch[0].surface_energy_profile[1].depth - patch[0].surface_energy_profile[0].depth) +
			patch[0].surface_energy_profile[2].T *
			(patch[0].surface_energy_profile[2].depth - patch[0].surface_energy_profile[1].depth) +
			patch[0].surface_energy_profile[3].T *
			(patch[0].surface_energy_profile[3].depth - patch[0].surface_energy_profile[2].depth)) /
			patch[0].surface_energy_profile[3].depth;
		
	}
	
	return;
}/*end surface_daily_F.c*/
