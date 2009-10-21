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

	double	 compute_litter_rain_stored(
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
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	double  albedo;
	double	dry_evaporation;
	double	Kstar_direct;
	double	Kstar_diffuse;
	double	APAR_diffuse;
	double	APAR_direct;
	double	potential_evaporation_rate;
	double	potential_rainy_evaporation_rate;
	double	rainy_evaporation;
	double	rnet_evap;
	double	PE_rate, PE_rainy_rate;
	double	soil_potential_evaporation;
	double	soil_potential_dry_evaporation_rate;
	double	soil_potential_rainy_evaporation_rate;
	double	exfiltration;
	struct	litter_object	*litter;
	
	litter = &(patch[0].litter);
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
	/*--------------------------------------------------------------*/
	/*	calculate surface albedo as a function of amount of	*/
	/*		litter vs soil					*/
	/*--------------------------------------------------------------*/
	if (litter->proj_pai >= 1.0)
		albedo = LITTER_ALBEDO;
	else
		albedo = LITTER_ALBEDO * litter->proj_pai
		+ patch[0].soil_defaults[0][0].albedo * (1-litter->proj_pai);
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
		1.0,
		litter->proj_pai,
		basin[0].theta_noon,
		albedo);
	
	APAR_diffuse = compute_diffuse_radiative_fluxes(
		command_line[0].verbose_flag,
		&(patch[0].PAR_diffuse),
		patch[0].PAR_direct,
		-10000.0,
		1.0,
		litter->proj_pai,
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
		1.0,
		litter->proj_pai,
		basin[0].theta_noon,
		albedo,
		albedo);
	APAR_direct = compute_direct_radiative_fluxes(
		command_line[0].verbose_flag,
		&(patch[0].PAR_direct),
		-10000,
		1.0,
		litter->proj_pai,
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
			litter[0].rain_stored + patch[0].rain_throughfall,
			litter[0].rain_capacity,
			litter[0].gl_c,	
			litter[0].gsurf_slope,
			litter[0].gsurf_intercept);
	
	if (patch[0].rootzone.depth > ZERO)
		patch[0].gsurf = compute_nonvascular_stratum_conductance(
			command_line[0].verbose_flag,
			patch[0].rz_storage + patch[0].rain_throughfall,
			patch[0].sat_deficit,
			patch[0].soil_defaults[0][0].gl_c,	
			patch[0].soil_defaults[0][0].gsurf_slope,
			patch[0].soil_defaults[0][0].gsurf_intercept);

	else
		patch[0].gsurf = compute_nonvascular_stratum_conductance(
			command_line[0].verbose_flag,
			patch[0].unsat_storage + patch[0].rain_throughfall,
			patch[0].sat_deficit,
			patch[0].soil_defaults[0][0].gl_c,	
			patch[0].soil_defaults[0][0].gsurf_slope,
			patch[0].soil_defaults[0][0].gsurf_intercept);
	/*--------------------------------------------------------------*/
	/*	COmpute evaporation and exfiltration RATES (m/s)	*/
	/*	for daylight period .					*/
	/*	The rainy rate assumes a vpd of 10Pa.			*/
	/*	Note if surface heat flux makes evap negative we	*/
	/*	have condensation.  					*/
	/*--------------------------------------------------------------*/
	if(zone[0].metv.dayl > ZERO)
	rnet_evap = 1000 * (Kstar_direct + Kstar_diffuse + patch[0].Lstar_soil
		+ patch[0].surface_heat_flux) / zone[0].metv.dayl;
	else rnet_evap = 0.0;
	if (rnet_evap < 0.0) rnet_evap = 0.0;
	/*--------------------------------------------------------------*/
	/*	Make sure ga and gsurf are non-zero.			*/
	/*--------------------------------------------------------------*/
	patch[0].ga = max((patch[0].ga * patch[0].stability_correction),0.0000000000001);
	/*--------------------------------------------------------------*/
	/*	Estimate potential evap rates.				*/
	/*--------------------------------------------------------------*/
	potential_evaporation_rate = penman_monteith(
		command_line[0].verbose_flag,
		zone[0].metv.tday,
		zone[0].metv.pa,
		zone[0].metv.vpd,
		rnet_evap,
		1/patch[0].litter.gsurf,
		1/(patch[0].ga),
		2) ;
	potential_rainy_evaporation_rate = penman_monteith(
		command_line[0].verbose_flag,
		zone[0].metv.tday,
		zone[0].metv.pa,
		10,
		rnet_evap,
		1/patch[0].litter.gsurf,
		1/(patch[0].ga),
		2) ;
	PE_rate = penman_monteith(
		command_line[0].verbose_flag,
		zone[0].metv.tday,
		zone[0].metv.pa,
		zone[0].metv.vpd,
		rnet_evap,
		0.0,
		1/(patch[0].ga),
		2) ;
	PE_rainy_rate = penman_monteith(
		command_line[0].verbose_flag,
		zone[0].metv.tday,
		zone[0].metv.pa,
		10,
		rnet_evap,
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
	if ( zone[0].rain > 0 ){
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
	/*	rain_throughfall and potential_evaporation	*/
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
			10,
			rnet_evap,
			1/patch[0].gsurf,
			1/patch[0].ga,
			2);
		soil_potential_dry_evaporation_rate = penman_monteith(
			command_line[0].verbose_flag,
			zone[0].metv.tday,
			zone[0].metv.pa,
			zone[0].metv.vpd,
			rnet_evap,
			1/patch[0].gsurf,
			1/patch[0].ga,
			2);
		soil_potential_evaporation  = soil_potential_dry_evaporation_rate
			* (zone[0].metv.dayl - zone[0].daytime_rain_duration )
			+ soil_potential_rainy_evaporation_rate
			* zone[0].daytime_rain_duration;

		/*--------------------------------------------------------------*/
		/*	base soil evapotration/ exfiltration will only occur 	*/
		/*	on exposed soil layers					*/
		/*--------------------------------------------------------------*/
		soil_potential_evaporation = soil_potential_evaporation * (max(0, 1-litter->proj_pai));

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
	return;
} /*end surface_daily_F.c*/
