
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
	
	double  compute_stability_correction(
										 int ,
										 double,
										 double,
										 double,
										 double,
										 double,
										 double);
	
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	double  detention_store_evaporation;
	double  detention_store_potential_evaporation;
	double  detention_store_potential_evaporation_night;
	double  detention_store_potential_evaporation_day;
	double  detention_store_potential_dry_evaporation_rate_night;
	double  detention_store_potential_rainy_evaporation_rate_night;
	double  detention_store_potential_dry_evaporation_rate_day;
	double  detention_store_potential_rainy_evaporation_rate_day;
	double  albedo;
	double	dry_evaporation;
	double	Kstar_direct;
	double	Kstar_diffuse;
	double	APAR_diffuse;
	double	APAR_direct;
	double	potential_evaporation_rate;
	double	potential_evaporation_rate_night;
	double	potential_evaporation_rate_day;
	double	potential_rainy_evaporation_rate;
	double	potential_rainy_evaporation_rate_night;
	double	potential_rainy_evaporation_rate_day;
	double	rainy_evaporation;
	double	rnet_evap_pond, rnet_evap_litter, rnet_evap_soil;
	double  rnet;
	double	PE_rate, PE_rainy_rate;
	double	PE_rate_night, PE_rainy_rate_night;
	double	PE_rate_day, PE_rainy_rate_day;
	double	soil_potential_evaporation;
	double	soil_potential_dry_evaporation_rate;
	double	soil_potential_dry_evaporation_rate_night;
	double	soil_potential_dry_evaporation_rate_day;
	double	soil_potential_rainy_evaporation_rate;
	double	soil_potential_rainy_evaporation_rate_night;
	double	soil_potential_rainy_evaporation_rate_day;
	double	exfiltration;
	double	K_used;
	double	K_initial;
	double	hv;			/* latent heat of vaporization for water (KJ/kg) */
	double	water_density;		/* density of water in kg/m^3 */
	double 	fraction_direct_K;
	double	fraction_diffuse_K;
	double	fraction_Lstar_pond;
	double	fraction_Lstar_pond_night;
	double	fraction_Lstar_pond_day;
	double	fraction_surface_heat_flux;
	double	fraction_surface_heat_flux_night;
	double	fraction_surface_heat_flux_day;
	double  Kdown_diffuse_in, PAR_diffuse_in, Kstar_diffuse_lit, APAR_diffuse_lit, Kstar_diffuse_soil, APAR_diffuse_soil;
	double  Kdown_direct_in, PAR_direct_in, Kstar_direct_lit, APAR_direct_lit, Kstar_direct_soil, APAR_direct_soil;
	double  Kup_direct_soil, Kup_diffuse_soil, Kup_direct_lit, Kup_diffuse_lit;
	double  dum;
	struct	litter_object	*litter;
	double	litter_NO3;
	double	surface_NO3;

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
	
	rnet_evap_pond = 0.0;
	rnet_evap_litter = 0.0;
	rnet_evap_soil = 0.0;
	
	Kup_direct_soil = 0.0;
	Kup_diffuse_soil = 0.0;
	Kup_direct_lit = 0.0;
	Kup_diffuse_lit = 0.0;
	
	patch[0].Kstar_soil = 0.0;
	
	dum = 0.0;
	litter_NO3 = 0;
	surface_NO3 = 0;
	litter = &(patch[0].litter);
	/* hv = 2.495 * 1e3;	// latent heat of vaporization for water (KJ/kg) */
	hv = (2.5023e6 - 2430.54 * zone[0].metv.tday) / 1000; /* changed to match hv used in penman monteith */
	water_density = 1000;	/* density of water in kg/m^3 */
	
	double daylength = zone[0].metv.dayl;
	double nightlength = SECONDS_PER_DAY - daylength;
	double day_proportion = daylength / SECONDS_PER_DAY;

	double surface_heat_flux_day = day_proportion * patch[0].surface_heat_flux;
	double surface_heat_flux_night = patch[0].surface_heat_flux - surface_heat_flux_day;

	double rain_duration_day = zone[0].rain_duration * day_proportion;
	double rain_duration_night = zone[0].rain_duration - rain_duration_day;

#ifdef DEBUG
	assert(daylength > rain_duration_day);
	assert(nightlength > rain_duration_night);
#endif

	/*patch[0].stability_correction = compute_stability_correction(
					 command_line[0].verbose_flag,
					 0.0,
					 patch[0].Tsoil,
					 0.0,
					 2.0,
					 zone[0].metv.tavg,
					 patch[0].wind);*/
	

	/*--------------------------------------------------------------*/
	/*	DETENTION STORE EVAPORATION:								*/
	/*	Check to see if detention store is greater than current		*/
	/*	litter holding capacity.  If so, run water surface evap		*/
	/*	first.														*/
	/* but only do this if there is actually a detention_store capacity to hold water */
	/*--------------------------------------------------------------*/
	
	if ( command_line[0].verbose_flag == -5 ){
		printf("\n     SURFACE DAILY START: stab=%lf detstore=%lf litraincap=%lf litrainstor=%lf Kupdir=%lf Kupdif=%lf Lstarpond=%lf Lstarsoil=%lf surfheatflux=%lf evapsurf=%lf", 
			   patch[0].stability_correction,
			   patch[0].detention_store, 
			   litter[0].rain_capacity, 
			   litter[0].rain_stored, 
			   patch[0].Kup_direct/86.4, 
			   patch[0].Kup_diffuse/86.4,
			   patch[0].Lstar_pond/86.4,
			   patch[0].Lstar_soil/86.4,
			   patch[0].surface_heat_flux,
			   patch[0].evaporation_surf);
	}
	
	/* Case where detention store sits on top of litter */
	if ( (patch[0].detention_store > (max(litter[0].rain_capacity - litter[0].rain_stored, 0.0)))
			&& (patch[0].soil_defaults[0][0].detention_store_size >= 0.0) ) {
		
		/* assume if det store over litter then litter is saturated */
		patch[0].detention_store -= (litter[0].rain_capacity - litter[0].rain_stored);
		litter[0].rain_stored = litter[0].rain_capacity;
		litter_NO3 = litter[0].NO3_stored;
		surface_NO3 = patch[0].surface_NO3;
		litter[0].NO3_stored = litter[0].rain_stored / (patch[0].detention_store + litter[0].rain_stored) * 
					( surface_NO3 + litter_NO3);
		patch[0].surface_NO3 = patch[0].detention_store / (patch[0].detention_store + litter[0].rain_stored) * 
					( surface_NO3 + litter_NO3);

		/*** Calculate available energy at surface. Assumes Kdowns are partially ***/
		/*** reflected by water surface based on water albedo. ***/
		double rnet_evap_pond_night = 1000 * ( patch[0].Lstar_pond_night + surface_heat_flux_night) / nightlength;
		double rnet_evap_pond_day = 1000 * ( (1 - WATER_ALBEDO) * (patch[0].Kdown_direct + patch[0].Kdown_diffuse)
				+ patch[0].Lstar_pond_day + surface_heat_flux_day) / daylength;
		rnet_evap_pond = rnet_evap_pond_night + rnet_evap_pond_day;
		if (rnet_evap_pond <= ZERO) rnet_evap_pond = 0.0;
		if (rnet_evap_pond_night <= ZERO) rnet_evap_pond_night = 0.0;
		if (rnet_evap_pond_day <= ZERO) rnet_evap_pond_day = 0.0;

		/*** Use Penman with rsurface=0 for open water evaporation. ***/

		patch[0].ga = max((patch[0].ga * patch[0].stability_correction),0.0001);

		detention_store_potential_dry_evaporation_rate_night = penman_monteith(
				command_line[0].verbose_flag,
				zone[0].metv.tnight,
				zone[0].metv.pa,
				zone[0].metv.vpd_night,
				rnet_evap_pond_night,
				0.0,
				1/(patch[0].ga),
				2) ;
		detention_store_potential_dry_evaporation_rate_night = max(0.0, detention_store_potential_dry_evaporation_rate_night);

		detention_store_potential_dry_evaporation_rate_day = penman_monteith(
						command_line[0].verbose_flag,
						zone[0].metv.tday,
						zone[0].metv.pa,
						zone[0].metv.vpd_day,
						rnet_evap_pond_day,
						0.0,
						1/(patch[0].ga),
						2) ;
		detention_store_potential_dry_evaporation_rate_day = max(0.0, detention_store_potential_dry_evaporation_rate_day);

		detention_store_potential_rainy_evaporation_rate_night = penman_monteith(
						command_line[0].verbose_flag,
						zone[0].metv.tnight,
						zone[0].metv.pa,
						10,
						rnet_evap_pond_night,
						0.0,
						1/(patch[0].ga),
						2) ;
		detention_store_potential_rainy_evaporation_rate_night = max(0.0, detention_store_potential_rainy_evaporation_rate_night);

		detention_store_potential_rainy_evaporation_rate_day = penman_monteith(
				command_line[0].verbose_flag,
				zone[0].metv.tday,
				zone[0].metv.pa,
				10,
				rnet_evap_pond_day,
				0.0,
				1/(patch[0].ga),
				2) ;
		detention_store_potential_rainy_evaporation_rate_day = max(0.0, detention_store_potential_rainy_evaporation_rate_day);

		/* Added adjustment for rain duration. Assumes rain duration input	*/
		/* is # of hours over 24-hr day and we divide into daylight vs.		*/
		/* night rain hours following total daylight hour fraction.			*/
		detention_store_potential_evaporation_night = (detention_store_potential_dry_evaporation_rate_night
		                * (nightlength - rain_duration_night) )
		                + (detention_store_potential_rainy_evaporation_rate_night
		                * rain_duration_night);
		detention_store_potential_evaporation_day = (detention_store_potential_dry_evaporation_rate_day
		                * (daylength - rain_duration_day) )
		                + (detention_store_potential_rainy_evaporation_rate_day
		                * rain_duration_day);
		detention_store_potential_evaporation = detention_store_potential_evaporation_day + detention_store_potential_evaporation_night;

		// Avoid over-estimating ET from surfaces with no detention store size
		//   (e.g. impervious surface) by gating ET by detention_store_size
		detention_store_evaporation = min(detention_store_potential_evaporation,
				min(patch[0].detention_store,
						patch[0].soil_defaults[0][0].detention_store_size));

		patch[0].detention_store -= detention_store_evaporation;

		patch[0].Kup_direct += (1 - patch[0].overstory_fraction) 
							* WATER_ALBEDO * patch[0].Kdown_direct;
		patch[0].Kup_diffuse += (1 - patch[0].overstory_fraction)
							* WATER_ALBEDO * patch[0].Kdown_diffuse;
	} else {
		detention_store_evaporation = 0.0;
	}
	
	
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
					+ patch[0].Lstar_pond
					/* - patch[0].Lstar_soil */
					+ patch[0].surface_heat_flux;

	if ( command_line[0].verbose_flag == -5 ){
		printf("\n     SURFACE DAILY DET STORE: dayl=%lf raindur=%lf Kused=%lf Kini=%lf Kdowndir=%lf Kdowndif=%lf Lstarsoil=%lf surfheat=%lf evapsurf=%lf", 
			   zone[0].metv.dayl, 
			   zone[0].rain_duration,
			   K_used/86.4, 
			   K_initial/86.4, 
			   patch[0].Kdown_direct/86.4, 
			   patch[0].Kdown_diffuse/86.4, 
			   patch[0].Lstar_soil/86.4, 
			   patch[0].surface_heat_flux,
			   patch[0].evaporation_surf);
	}
	
	if ( K_used > 0 ){
		if ( K_initial > 0 ){
			fraction_direct_K = (1-WATER_ALBEDO) * patch[0].Kdown_direct / K_initial;
			fraction_diffuse_K = (1-WATER_ALBEDO) * patch[0].Kdown_diffuse / K_initial;
			fraction_Lstar_pond = patch[0].Lstar_soil / K_initial;
			fraction_Lstar_pond_night = patch[0].Lstar_soil_night / K_initial;
			fraction_Lstar_pond_day = patch[0].Lstar_soil_day / K_initial;
			fraction_surface_heat_flux = patch[0].surface_heat_flux / K_initial;
			fraction_surface_heat_flux_night = surface_heat_flux_night / K_initial;
			fraction_surface_heat_flux_day = surface_heat_flux_day / K_initial;
		}
		else {
			fraction_direct_K = 0.0;
			fraction_diffuse_K = 0.0;
			fraction_Lstar_pond = 0.0;
			fraction_Lstar_pond_night = 0.0;
			fraction_Lstar_pond_day = 0.0;
			fraction_surface_heat_flux = 0.0;
			fraction_surface_heat_flux_night = 0.0;
			fraction_surface_heat_flux_day = 0.0;
		}
		patch[0].Kdown_direct = ((1-WATER_ALBEDO) * patch[0].Kdown_direct) - (fraction_direct_K * K_used);
		patch[0].Kdown_diffuse = ((1-WATER_ALBEDO) * patch[0].Kdown_diffuse) - (fraction_diffuse_K * K_used);
		patch[0].Lstar_pond = patch[0].Lstar_soil - (fraction_Lstar_pond * K_used);
		patch[0].Lstar_pond_night = patch[0].Lstar_pond_night - (fraction_Lstar_pond_night * K_used);
		patch[0].Lstar_pond_day = patch[0].Lstar_pond_day - (fraction_Lstar_pond_day * K_used);
		patch[0].surface_heat_flux = patch[0].surface_heat_flux - (fraction_surface_heat_flux * K_used);
		surface_heat_flux_night = surface_heat_flux_night - (fraction_surface_heat_flux_night * K_used);
		surface_heat_flux_day = surface_heat_flux_day - (fraction_surface_heat_flux_day * K_used);
	}
	
	if ( command_line[0].verbose_flag == -5 ){
	printf("\n          SURFACE DAILY POST DET EVAP: Kdowndir=%lf Kdowndif=%lf Kupdir=%lf Kupdif=%lf Lstarsoil=%lf surfheat=%lf detstore=%lf raincap=%lf rainstor=%lf", 
		   patch[0].Kdown_direct/86.4, 
		   patch[0].Kdown_diffuse/86.4,
		   patch[0].Kup_direct/86.4,
		   patch[0].Kup_diffuse/86.4,
		   patch[0].Lstar_pond/86.4, 
		   patch[0].surface_heat_flux, 
		   patch[0].detention_store, 
		   litter[0].rain_capacity, 
		   litter[0].rain_stored);
	}
	
	/*--------------------------------------------------------------*/
	/*	LITTER STORE EVAPORATION:									*/
	/*	Now move to litter and then bare soil evaporation if the 	*/
	/*	remaining surface water can be held by the litter.			*/
	/*--------------------------------------------------------------*/
	
	if ( patch[0].detention_store <= (max(litter[0].rain_capacity - litter[0].rain_stored, 0.0))){
			//| (patch[0].soil_defaults[0][0].detention_store_size == 0.0)) {
		
		litter[0].NO3_stored = patch[0].surface_NO3;
		patch[0].surface_NO3 = 0;
		
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
		
		/* Keep track of incoming radiation to surface (before litter) */
		Kdown_diffuse_in = patch[0].Kdown_diffuse;
		PAR_diffuse_in = patch[0].PAR_diffuse;
		
		Kstar_diffuse_lit = compute_diffuse_radiative_fluxes(
			command_line[0].verbose_flag,
			&(patch[0].Kdown_diffuse),
			&(Kup_diffuse_lit),
			patch[0].Kdown_direct,
			-10000.0,
			0.0,
			1.0,
			basin[0].theta_noon,
			LITTER_ALBEDO);
	
		APAR_diffuse_lit = compute_diffuse_radiative_fluxes(
			command_line[0].verbose_flag,
			&(patch[0].PAR_diffuse),
			&(dum),
			patch[0].PAR_direct,
			-10000.0,
			0.0,
			1.0,
			basin[0].theta_noon,
			LITTER_ALBEDO);
		
		/* Absorbed soil fluxes. */
		Kstar_diffuse_soil = (1-patch[0].soil_defaults[0][0].albedo) * Kdown_diffuse_in;
		Kup_diffuse_soil = patch[0].soil_defaults[0][0].albedo * Kdown_diffuse_in;
		APAR_diffuse_soil = (1-patch[0].soil_defaults[0][0].albedo) * PAR_diffuse_in;
		
		
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

		/* Keep track of incoming radiation to surface (before litter) */
		Kdown_direct_in = patch[0].Kdown_direct;
		PAR_direct_in = patch[0].PAR_direct;		
		
		Kstar_direct_lit = compute_direct_radiative_fluxes(
			command_line[0].verbose_flag,
			&(patch[0].Kdown_direct),
			&(Kup_direct_lit),
			-10000,
			0.0,
			1.0,
			basin[0].theta_noon,
			LITTER_ALBEDO,
			patch[0].soil_defaults[0][0].albedo);
		
		APAR_direct_lit = compute_direct_radiative_fluxes(
			command_line[0].verbose_flag,
			&(patch[0].PAR_direct),
			&(dum),
			-10000,
			0.0,
			1.0,
			basin[0].theta_noon,
			LITTER_ALBEDO,
			patch[0].soil_defaults[0][0].albedo);
		
		/* Absorbed soil fluxes. */
		Kstar_direct_soil = (1-patch[0].soil_defaults[0][0].albedo) * Kdown_direct_in;
		Kup_direct_soil = patch[0].soil_defaults[0][0].albedo * Kdown_direct_in;
		APAR_direct_soil = (1-patch[0].soil_defaults[0][0].albedo) * PAR_direct_in;

		
		if ( command_line[0].verbose_flag == -5 ){
		printf("\n          SURFACE DAILY POST LITTER INT: Kdowndir=%lf Kdowndif=%lf Lstarsoil=%lf surfheat=%lf lit_pai=%lf\n          Kdifflit=%lf Kdirlit=%lf Kdiffsoil=%lf Kdirsoil=%lf\n          Kupdir=%lf Kupdirlit=%lf Kupdirsoil=%lf Kupdiff=%lf Kupdifflit=%lf Kupdiffsoil=%lf", 
			   patch[0].Kdown_direct/86.4, 
			   patch[0].Kdown_diffuse/86.4, 
			   patch[0].Lstar_soil/86.4, 
			   patch[0].surface_heat_flux,
			   litter->proj_pai,
			   Kstar_diffuse_lit/86.4,
			   Kstar_direct_lit/86.4,
			   Kstar_diffuse_soil/86.4,
			   Kstar_direct_soil/86.4,
			   patch[0].Kup_direct/86.4,
			   Kup_direct_lit/86.4,
			   Kup_direct_soil/86.4,
			   patch[0].Kup_diffuse/86.4,
			   Kup_diffuse_lit/86.4,
			   Kup_diffuse_soil/86.4);
		}
									
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
				max(litter[0].rain_stored + patch[0].detention_store, litter[0].rain_capacity),
				litter[0].rain_capacity,
				litter[0].gl_c,	
				litter[0].gsurf_slope,
				litter[0].gsurf_intercept);
		
		/* Litter surface resistance model from NCAR CLM4 (Lawrence et al 2011 J. Adv. Model. Earth Syst). */
		/* The 1 in the exp is the fraction of litter not covered by snow, assumed 1 here since we only */
		/* evaporate from litter when no snowpack. */
		litter[0].gsurf = 1/(1/(0.004*patch[0].wind)*(1-exp(-1)));
		
		if (patch[0].rootzone.depth > ZERO) {
			patch[0].gsurf = compute_nonvascular_stratum_conductance(
				command_line[0].verbose_flag,
				max(patch[0].rz_storage + patch[0].detention_store, patch[0].sat_deficit),
				patch[0].sat_deficit,
				patch[0].soil_defaults[0][0].gl_c,	
				patch[0].soil_defaults[0][0].gsurf_slope,
				patch[0].soil_defaults[0][0].gsurf_intercept);
		}

		else {
			patch[0].gsurf = compute_nonvascular_stratum_conductance(
				command_line[0].verbose_flag,
				max(patch[0].unsat_storage + patch[0].detention_store, patch[0].sat_deficit),
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
		
		/* Set patch variable for tracking & output. */
		/* Note that this is total surface Kstar not just soil Kstar. */
		patch[0].Kstar_soil = (1 - litter[0].cover_fraction) 
									* (Kstar_direct_soil + Kstar_diffuse_soil) 
								+ litter[0].cover_fraction
									* (Kstar_direct_lit + Kstar_diffuse_lit);
		/* Update patch Kup with reflected fluxes from surface. */
		/* Assumes Kup from surface gets reabsorbed by canopy so only track open surfaces. */
		patch[0].Kup_direct += (1 - patch[0].overstory_fraction)
									* ((1 - litter[0].cover_fraction) * Kup_direct_soil 
									+ litter[0].cover_fraction * Kup_direct_lit);
		patch[0].Kup_diffuse += (1 - patch[0].overstory_fraction) 
									* ((1 - litter[0].cover_fraction) * Kup_diffuse_soil 
									+ litter[0].cover_fraction * Kup_diffuse_lit);
		
		/* Assuming net LW for litter is same as soil layer, which is	*/
		/* a reasonable approximation if litter and soil are same 	*/
		/* temperature and have same emissivity. */
		double rnet_evap_litter_night = litter[0].cover_fraction
				* 1000 * (patch[0].Lstar_soil_night
						+ surface_heat_flux_night)
						/ nightlength;
		double rnet_evap_litter_day = litter[0].cover_fraction
				* 1000 * (Kstar_direct_lit + Kstar_diffuse_lit
						+ patch[0].Lstar_soil_day
						+ surface_heat_flux_day)
						/ daylength;
		rnet_evap_litter = rnet_evap_litter_night + rnet_evap_litter_day;
		if (rnet_evap_litter <= ZERO) rnet_evap_litter = 0.0;
		if (rnet_evap_litter_night <= ZERO) rnet_evap_litter_night = 0.0;
		if (rnet_evap_litter_day <= ZERO) rnet_evap_litter_day = 0.0;
		/* Assuming net LW for soil is available for soil water evap. */
		double rnet_evap_soil_night = (1 - litter[0].cover_fraction)
															* 1000 * (patch[0].Lstar_soil_night
																	+ surface_heat_flux_night)
																	/ nightlength;
		double rnet_evap_soil_day = (1 - litter[0].cover_fraction)
													* 1000 * (Kstar_direct_soil + Kstar_diffuse_soil
															+ patch[0].Lstar_soil_day
															+ surface_heat_flux_day)
															/ daylength;
		rnet_evap_soil = rnet_evap_soil_night + rnet_evap_soil_day;
		if (rnet_evap_soil <= ZERO) rnet_evap_soil = 0.0;
		if (rnet_evap_soil_night <= ZERO) rnet_evap_soil_night = 0.0;
		if (rnet_evap_soil_day <= ZERO) rnet_evap_soil_day = 0.0;
		

		if ( command_line[0].verbose_flag == -5 ){
			printf("\n          SURFACE DAILY EVAP: rnet_lit=%lf rnet_soil=%lf gsurf_lit=%lf gsurf_soil=%lf lit.gl_c=%lf lit.gsurf_slope=%lf lit.gsurf_int=%lf", 
				   rnet_evap_litter/86.4, 
				   rnet_evap_soil/86.4, 
				   litter[0].gsurf, 
				   patch[0].gsurf,
				   litter[0].gl_c,
				   litter[0].gsurf_slope,
				   litter[0].gsurf_intercept);
		}
		
		
		/*--------------------------------------------------------------*/
		/*	Make sure ga and gsurf are non-zero.			*/
		/*--------------------------------------------------------------*/
		patch[0].ga = max((patch[0].ga * patch[0].stability_correction),0.0001);
		/*--------------------------------------------------------------*/
		/*	Estimate potential evap rates.				*/
		/*--------------------------------------------------------------*/
		potential_evaporation_rate_night = penman_monteith(
					command_line[0].verbose_flag,
					zone[0].metv.tnight,
					zone[0].metv.pa,
					zone[0].metv.vpd_night,
					rnet_evap_litter_night,
					1/patch[0].litter.gsurf,
					1/(patch[0].ga),
					2) ;
		potential_evaporation_rate_day = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			zone[0].metv.vpd_day,
			rnet_evap_litter_day,
			1/patch[0].litter.gsurf,
			1/(patch[0].ga),
			2) ;
		potential_rainy_evaporation_rate_night = penman_monteith(
					command_line[0].verbose_flag,
					zone[0].metv.tnight,
					zone[0].metv.pa,
					10,
					rnet_evap_litter_night,
					1/patch[0].litter.gsurf,
					1/(patch[0].ga),
					2) ;
		potential_rainy_evaporation_rate_day = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			10,
			rnet_evap_litter_day,
			1/patch[0].litter.gsurf,
			1/(patch[0].ga),
			2) ;
		PE_rate_night = penman_monteith(
					command_line[0].verbose_flag,
					zone[0].metv.tnight,
					zone[0].metv.pa,
					zone[0].metv.vpd_night,
					rnet_evap_litter_night,
					0.0,
					1/(patch[0].ga),
					2) ;
		PE_rate_day = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			zone[0].metv.vpd_day,
			rnet_evap_litter_day,
			0.0,
			1/(patch[0].ga),
			2) ;
		PE_rainy_rate_night = penman_monteith(
					command_line[0].verbose_flag,
					zone[0].metv.tnight,
					zone[0].metv.pa,
					10,
					rnet_evap_litter_night,
					0.0,
					1/(patch[0].ga),
					2) ;
		PE_rainy_rate_day = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			10,
			rnet_evap_litter_day,
			0.0,
			1/(patch[0].ga),
			2) ;
		
		PE_rainy_rate_night = max(0.0, PE_rainy_rate_night);
		PE_rainy_rate_day = max(0.0, PE_rainy_rate_day);
		PE_rate_night = max(0.0, PE_rate_night);
		PE_rate_day = max(0.0, PE_rate_day);
		potential_evaporation_rate_night = max(0,potential_evaporation_rate_night);
		potential_evaporation_rate_day = max(0,potential_evaporation_rate_day);
		potential_rainy_evaporation_rate_night = max(0,potential_rainy_evaporation_rate_night);
		potential_rainy_evaporation_rate_day = max(0,potential_rainy_evaporation_rate_day);

		/*--------------------------------------------------------------*/
		/*	Compute potential evaporation of litter. 		*/
		/*	Weighted by rain and non rain periods of the daytime and night time	*/
		/*	m/day = m/s * (sec/day)					*/
		/*								*/
		/*	Note that Kstar is converted from Kj/m2*day to W/m2	*/
		/*--------------------------------------------------------------*/
		double potential_evaporation_night = (potential_evaporation_rate_night
				* (nightlength - rain_duration_night) )
				+ (potential_rainy_evaporation_rate_night
				* rain_duration_night);
		double potential_evaporation_day = (potential_evaporation_rate_day
				* (daylength - rain_duration_day) )
				+ (potential_rainy_evaporation_rate_day
				* rain_duration_day);
		patch[0].potential_evaporation = potential_evaporation_night + potential_evaporation_day;

		double PE_night = (PE_rate_night
				* (nightlength - rain_duration_night) )
				+ (PE_rainy_rate_night
				* rain_duration_night);
		double PE_day = (PE_rate_day
				* (daylength - rain_duration_day) )
				+ (PE_rainy_rate_day
				* rain_duration_day);
		patch[0].PE = PE_night + PE_day;

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
		soil_potential_rainy_evaporation_rate_night = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tnight,
			zone[0].metv.pa,
			10.0,
			rnet_evap_soil_night,
			1.0/patch[0].gsurf,
			1.0/patch[0].ga,
			2);
		soil_potential_rainy_evaporation_rate_day = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			10.0,
			rnet_evap_soil_day,
			1.0/patch[0].gsurf,
			1.0/patch[0].ga,
			2);
		soil_potential_dry_evaporation_rate_night = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tnight,
			zone[0].metv.pa,
			zone[0].metv.vpd_night,
			rnet_evap_soil_night,
			1.0/patch[0].gsurf,
			1.0/patch[0].ga,
			2);
		soil_potential_dry_evaporation_rate_day = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			zone[0].metv.vpd_day,
			rnet_evap_soil_day,
			1.0/patch[0].gsurf,
			1.0/patch[0].ga,
			2);

		double soil_potential_evaporation_night = (soil_potential_dry_evaporation_rate_night
				* (nightlength - rain_duration_night) )
				+ (soil_potential_rainy_evaporation_rate_night
				* rain_duration_night);
		double soil_potential_evaporation_day = (soil_potential_dry_evaporation_rate_day
				* (daylength - rain_duration_day) )
				+ (soil_potential_rainy_evaporation_rate_day
				* rain_duration_day);
		soil_potential_evaporation = soil_potential_evaporation_night + soil_potential_evaporation_day;

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
		
		if ( command_line[0].verbose_flag == -5 ){
			printf(" soil_PE=%lf soil_PExfil=%lf exfil_unsat=%lf exfil_sat=%lf rnet_soil=%lf rsurf=%lf ra=%lf",
				   soil_potential_evaporation,
				   patch[0].potential_exfiltration,
				   patch[0].exfiltration_unsat_zone,
				   patch[0].exfiltration_sat_zone,
				   rnet_evap_soil,
				   1.0/patch[0].gsurf,
				   1.0/patch[0].ga);
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
