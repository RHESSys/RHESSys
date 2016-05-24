/*--------------------------------------------------------------*/
/* 																*/
/*				 		patch_daily_F					*/
/*																*/
/*	NAME														*/
/*	patch_daily_F 										*/
/*				 - performs cycling and output of a patch		*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void patch_daily_F(								*/
/*						struct	world_object	*,				*/
/*						struct	basin_object	*,				*/
/*						struct	hillslope_object	*,			*/
/*						struct	zone_object		*,				*/
/*						struct patch_object	,					*/
/*						struct command_line_object ,			*/
/*						struct tec_entry,						*/
/*						struct date)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	canopy_stata in the patch. The routine also prints out results*/
/*	where specified by current tec events files.				*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	March 4, 1997	  C.Tague 									*/
/*	baseflow moved to hillslope level 							*/
/*																*/
/*	March 9, 1997	  C.Tague 									*/
/*	moss routines only called if moss depth > 0 				*/
/*																*/
/*																*/
/*	March 13, 19987 - RAF			*/
/*	Took comments offurface_daily_F call	*/
/*	since it was under an if condition 	*/
/*	Put the comments around the if condition*/  
/*																*/
/*	July 28, 1997 - C.Tague			*/
/*	removed capillary rise routines 	*/
/*											*/
/*	Sep 2 1997								*/
/*	Removed references to moss or humic layer	*/
/*	Everything is now either a stratum or soil	*/
/*							*/
/*	Sep 3 1997					*/
/*	Took out condition that strata under snowpack	*/
/*	must be at z>0					*/
/*							*/
/*	Sept 29 1997 CT   				*/
/*	unsat drainage and cap rise are now based 	*/
/*	on actual depth to water table which is		*/
/*	determined by assuming a porosity decay		*/
/*	with depth;  transpiration is now sep.		*/
/*	into sat_zone and unsat_zone trans		*/
/*							*/
/*	Oct 22 1997 CT   				*/
/*	included calculation of water balance		*/
/*							*/
/*	Oct 22 1997 CT   				*/
/*	canopy_strata sublimation now added to 		*/
/*	patch level sum of canopy evaporation		*/
/*							*/
/*	added check_zero_stores				*/
/*							*/
/*	Feb 2010 AD											*/
/*	Added detention store evaporation, including more	*/
/*	substantial updates to surface daily F				*/
/*														*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

void		patch_daily_F(
						  struct	world_object	*world,
						  struct	basin_object	*basin,
						  struct	hillslope_object	*hillslope,
						  struct	zone_object		*zone,
						  struct 	patch_object 	*patch,
						  struct 	command_line_object *command_line,
						  struct	tec_entry		*event,
						  struct	date 			current_date)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	void compute_Lstar(
		int,
		struct	basin_object	*basin,
		struct zone_object *,
		struct patch_object *);


	double compute_delta_water(int, double, double,	double, double, double);
	
	
	double compute_layer_field_capacity(
		int,
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);
	
	void canopy_stratum_daily_F(
		struct world_object *,
		struct basin_object *,
		struct hillslope_object *,
		struct zone_object *,
		struct patch_object *,
		struct layer_object *,
		struct canopy_strata_object *,
		struct canopy_strata_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	void   surface_daily_F(
		struct world_object *,
		struct basin_object *,
		struct hillslope_object *,
		struct zone_object *,
		struct patch_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
		
	void	update_soil_moisture(
		int	verbose_flag,
		double	infiltration,
		double	net_inflow,
		struct	patch_object	*patch,
		struct 	command_line_object *command_line,
		struct	date 			current_date); 

	
	double	snowpack_daily_F (
		struct date,
		int,
		struct zone_object *,
		struct patch_object *,
		struct	snowpack_object	*,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double *,
		double *,
		double *,
		double *,
		double *,
		double *,
		double,
		double,
		double,
		double,
		double,
		double,
		int);
	
	double	compute_infiltration(
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);

	double  compute_surface_heat_flux(
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);
	
	double	compute_unsat_zone_drainage(
		int,
		int,
		double,
		double,
		double,
		double,
		double,
		double);
	
	double  compute_radiative_fluxes(
		int,
		double *,
		double *,
		double,
		double,
		double);
	
	
		/*-------------------------------------
		double  compute_stability_correction(
		int ,
		double,
		double,
		double,
		double,
		double,
		double);
	----------------------------------------*/
	
	double  compute_z_final(
		int,
		double,
		double,
		double,
		double,
		double);

	int 	check_zero_stores(
		struct  soil_c_object   *,
		struct  soil_n_object   *,
		struct  litter_c_object *,
		struct  litter_n_object *
		);
	
	int	update_decomp(
		struct	date,
		struct  soil_c_object   *,
		struct  soil_n_object   *,
		struct  litter_c_object *,
		struct  litter_n_object *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *,
		struct patch_object *);
	
	int	update_dissolved_organic_losses(
		struct	date,
		double,
		struct  soil_c_object   *,
		struct  soil_n_object   *,
		struct  litter_c_object *,
		struct  litter_n_object *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *);
	
	int	update_septic(
		struct	date,
		struct  patch_object   *);

	int	update_nitrif(
		struct  soil_c_object   *,
		struct  soil_n_object   *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *,
		struct soil_class,
		double,
		double,
		double,
		double,
		double,
		double,
		double);
	
	int	update_denitrif(
		struct  soil_c_object   *,
		struct  soil_n_object   *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *,
		struct soil_class,
		double,
		double);

	
	int	resolve_sminn_competition(
		struct  soil_n_object   *,
		double, double,
		double, double, double,
		struct ndayflux_patch_struct *);
	
	void   canopy_stratum_growth(
		struct world_object *,
		struct basin_object *,
		struct hillslope_object *,
		struct zone_object *,
		struct patch_object *,
		struct canopy_strata_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);

	int update_gw_drainage(
			struct patch_object *,
			struct hillslope_object *,
			struct zone_object *,
			struct command_line_object *,
			struct date);
			
	double	penman_monteith(
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		int);
		
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
	
	double compute_toc_wind(int,
							double,
							double,
							double,
							double);
	
	double  compute_ra_overstory(
								 int     ,
								 double  ,
								 double  ,
								 double *,
								 double *,
								 double *,
								 double *,
								 double  ,
								 double  ,
								 double	,
								 double *,
								 double *);
	
	

	long julday( struct date);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	layer;
	int stratum, ch, inx;
	int	vegtype;
	int dum;
	double	cap_rise, tmp, wilting_point;
	double	delta_unsat_zone_storage;
	double  infiltration, lhvap;
	double	infiltration_ini;
	double	infiltration_fin;
	double	net_inflow, theta;
	double	preday_snowpack_height;
	double	sat_zone_patch_demand;
	double	sat_zone_patch_demand_initial;
	double	available_sat_water;
	double	temp,scale;
	double	unsat_zone_patch_demand;
	double	unsat_zone_patch_demand_initial;
	double  add_field_capacity;
	double	water_above_field_cap;
	double	water_below_field_cap;
	double 	duration, irrigation;
	double	snow_melt_input;
	double  fertilizer_NO3, fertilizer_NH4;
	double	resp, transpiration_reduction_percent;
	double 	surfaceN_to_soil;
	double	FERT_TO_SOIL;
	double	pond_height;
	double tmpra, tmpga, tmpgasnow, tmpwind, tmpwindcan, tmpwindsnow, tmpustar;
	double Kup_direct_snow, Kup_diffuse_snow;
	double Kdown_direct_covered, Kdown_diffuse_covered, Kdown_direct_exposed, Kdown_diffuse_exposed;
	double Kup_direct_snow_covered, Kup_diffuse_snow_covered, Kup_direct_snow_exposed, Kup_diffuse_snow_exposed;
	double PAR_direct_covered, PAR_diffuse_covered, PAR_direct_exposed, PAR_diffuse_exposed;
	double snow_melt_covered, snow_melt_exposed;
	double 	rz_drainage,unsat_drainage;
	struct	canopy_strata_object	*strata;
	struct	litter_object	*litter;
	struct  dated_sequence	clim_event;
	
	/*--------------------------------------------------------------*/
	/*	We assume the zone soil temp applies to the patch as well.	*/
	/* 	unless we are using the surface energy iteration code 	in which */
	/* 	case we use the temperature from the previous day		*/
	/*	alos for the Kdowns and PAR (for now Ldown can be kept )	*/
	/*--------------------------------------------------------------*/

	if (command_line[0].surface_energy_flag == 0) 
		patch[0].Tsoil = zone[0].metv.tsoil;


	patch[0].Kdown_direct = zone[0].Kdown_direct;
	patch[0].Kdown_diffuse = zone[0].Kdown_diffuse;
	patch[0].PAR_direct = zone[0].PAR_direct;
	patch[0].PAR_diffuse = zone[0].PAR_diffuse;
	patch[0].evaporation_surf = 0.0;
	patch[0].potential_evaporation = 0.0;
	patch[0].Ldown = zone[0].Ldown;
	patch[0].Ldown_final = 0.0;
	
	patch[0].Kstar_canopy = 0.0;
	patch[0].Kstar_canopy_final = 0.0;
	patch[0].LE_canopy = 0.0;
	patch[0].LE_canopy_final = 0.0;
	patch[0].Kdown_direct_subcanopy = 0.0;
	patch[0].Kdown_diffuse_subcanopy = 0.0;
	patch[0].Ldown_subcanopy = 0.0;
	
	patch[0].wind_final = 0.0;
	patch[0].windsnow_final = 0.0;
	patch[0].ustar = 0.0;
	patch[0].ustar_final = 0.0;
	
	patch[0].snowpack.overstory_fraction = 0.0;
	patch[0].overstory_fraction = 0.0;
	
	patch[0].ga = 0.0;
	patch[0].gasnow = 0.0;
	
	tmpwind = zone[0].wind;
	
	dum = 0;
	patch[0].Kup_direct = 0.0;
	patch[0].Kup_diffuse = 0.0;
	patch[0].Kup_direct_final = 0.0;
	patch[0].Kup_diffuse_final = 0.0;
	Kup_direct_snow = 0.0;
	Kup_diffuse_snow = 0.0;
	
	patch[0].Kdown_direct_bare = 0.0;
	
	patch[0].snowpack.sublimation = 0.0;
	patch[0].snowpack.Rnet = 0.0;
	patch[0].snowpack.Q_H = 0.0;
	patch[0].snowpack.Q_LE = 0.0;
	patch[0].snowpack.Q_rain = 0.0;
	patch[0].snowpack.Q_melt = 0.0;
	
	patch[0].LE_soil = 0.0;
	
	
	patch[0].snowpack.K_reflectance = 0.0;
	patch[0].snowpack.K_absorptance = 0.0;
	patch[0].snowpack.PAR_reflectance = 0.0;
	patch[0].snowpack.PAR_absorptance = 0.0;
	patch[0].snowpack.Kstar_direct = 0.0;
	patch[0].snowpack.Kstar_diffuse = 0.0;
	patch[0].snowpack.APAR_direct = 0.0;
	patch[0].snowpack.APAR_diffuse = 0.0;
	
	Kdown_direct_covered = 0.0;
	Kdown_diffuse_covered = 0.0;
	Kdown_direct_exposed = 0.0;
	Kdown_diffuse_exposed = 0.0;
	Kup_direct_snow_covered = 0.0;
	Kup_diffuse_snow_covered = 0.0;
	Kup_direct_snow_exposed = 0.0;
	Kup_diffuse_snow_exposed = 0.0;	
	PAR_direct_covered = 0.0;
	PAR_diffuse_covered = 0.0;
	PAR_direct_exposed = 0.0;
	PAR_diffuse_exposed = 0.0;
	snow_melt_covered = 0.0;
	snow_melt_exposed = 0.0;
	
	patch[0].exfiltration_unsat_zone = 0.0;
	patch[0].exfiltration_sat_zone = 0.0;
	
	patch[0].T_canopy = zone[0].metv.tavg;
	patch[0].T_canopy_final = 0.0;
	

	if ( command_line[0].verbose_flag == -5 ){
	printf("\nPATCH DAILY F:");
	}
	
	/*--------------------------------------------------------------*/
	/*	Set the patch rain and snow throughfall equivalent to the	*/
	/*	rain and snow coming down over the zone.					*/
	/* check to see if there are base station inputs 		*/
	/*--------------------------------------------------------------*/

	if (patch[0].base_stations != NULL) {
		inx = patch[0].base_stations[0][0].dated_input[0].irrigation.inx;
		if (inx > -999) {
			clim_event = patch[0].base_stations[0][0].dated_input[0].irrigation.seq[inx];
			while (julday(clim_event.edate) < julday(current_date)) {
				patch[0].base_stations[0][0].dated_input[0].irrigation.inx += 1;
				inx = patch[0].base_stations[0][0].dated_input[0].irrigation.inx;
				clim_event = patch[0].base_stations[0][0].dated_input[0].irrigation.seq[inx];
				}
			if ((clim_event.edate.year != 0) && ( julday(clim_event.edate) == julday(current_date)) ) {
				irrigation = clim_event.value;
				}
			else irrigation = 0.0;
			} 
		else irrigation = patch[0].landuse_defaults[0][0].irrigation;
		}
	else irrigation = patch[0].landuse_defaults[0][0].irrigation;
	/*--------------------------------------------------------------*/
	/*	process any daily rainfall				*/
	/*--------------------------------------------------------------*/
	patch[0].rain_throughfall = zone[0].rain + irrigation;

	/* the N_depo is add in patch_hourly.c in hourly */
	/* it could be washed away hourly or daily, depending on whether the precipitation data is hourly or daily */
	patch[0].NO3_throughfall = 0;


	if (command_line[0].snow_scale_flag == 1) {
		patch[0].snow_throughfall = zone[0].snow * patch[0].snow_redist_scale;
		}
	else	patch[0].snow_throughfall = zone[0].snow;

	patch[0].wind = zone[0].wind;
	patch[0].windsnow = zone[0].wind;

	patch[0].precip_with_assim += patch[0].rain_throughfall + patch[0].snow_throughfall;

	if ((patch[0].landuse_defaults[0][0].septic_water_load > ZERO) 
		|| (patch[0].landuse_defaults[0][0].septic_NO3_load > ZERO)) {
		if (update_septic( current_date, patch) != 0) {
			printf("\n Error in update_septic ...exiting");
			exit(EXIT_FAILURE);
			}
		}


	patch[0].acc_year.pcp += zone[0].rain + zone[0].snow + irrigation;
	patch[0].acc_year.snowin += zone[0].snow;
	/*--------------------------------------------------------------*/
	/* if snowmelt is from another model (and input rather than computed */
	/* get that value and set it up to substitute for rhessys internal snowmelt */
	/*--------------------------------------------------------------*/
	snow_melt_input=-999.0;
	if (patch[0].base_stations != NULL) {
		inx = patch[0].base_stations[0][0].dated_input[0].snow_melt_input.inx;
		if (inx > -999) {
			clim_event = patch[0].base_stations[0][0].dated_input[0].snow_melt_input.seq[inx];
			while (julday(clim_event.edate) < julday(current_date)) {
				patch[0].base_stations[0][0].dated_input[0].snow_melt_input.inx += 1;
				inx = patch[0].base_stations[0][0].dated_input[0].snow_melt_input.inx;
				clim_event = patch[0].base_stations[0][0].dated_input[0].snow_melt_input.seq[inx];
				}
			if ((clim_event.edate.year != 0) && ( julday(clim_event.edate) == julday(current_date)) ) {
				snow_melt_input = clim_event.value;
				}
			else snow_melt_input = 0.0;
			} 
		else snow_melt_input=-999.0;
	}



	/*--------------------------------------------------------------*/
	/*	Compute the stability correction factor for aero cond	*/
	/*--------------------------------------------------------------*/
	patch[0].stability_correction = 1.0;
	
	
	/*--------------------------------------------------------------*/
	/*      Determine patch SOIL heat flux.                         */
	/*      (This is ignored if there is a 0 height stratum.        */
	/*--------------------------------------------------------------*/

	patch[0].surface_heat_flux = -1 * compute_surface_heat_flux(
		command_line[0].verbose_flag,
		patch[0].snow_stored,
		patch[0].unsat_storage,
		patch[0].sat_deficit,
		zone[0].metv.tavg,
		zone[0].metv.tnightmax,
		zone[0].metv.tsoil,
		patch[0].soil_defaults[0][0].deltaz,
		patch[0].soil_defaults[0][0].min_heat_capacity,
		patch[0].soil_defaults[0][0].max_heat_capacity);

	/*----------------------------------------------------------------------*/
	/*	Compute the no-canopy aerodynamic resistance at 2m as a baseline	*/
	/*	for null covers	*/
	/*----------------------------------------------------------------------*/
	tmpra = compute_ra_overstory(
								 command_line[0].verbose_flag,
								 0.0,
								 0.4,
								 &(tmpwind),
								 &(tmpwindcan),
								 &(tmpwindsnow),
								 &(tmpustar),
								 zone[0].base_stations[0][0].screen_height,
								 0.0,
								 0.0,
								 &(tmpga),
								 &(tmpgasnow));
	
	/* Set values for no stratum case. These will be overwritten if veg present. */
	patch[0].ga = tmpga;
	patch[0].gasnow = tmpgasnow;
	patch[0].wind = tmpwind;
	patch[0].windsnow = tmpwindsnow;
	patch[0].ustar = tmpustar;
	
	/*--------------------------------------------------------------*/
	/*	Cycle through patch layers with height greater than the	*/
	/*	snowpack.						*/
	/*--------------------------------------------------------------*/
	
	/*	Calculate initial pond height		*/
	pond_height = max(0.0,-1 * patch[0].sat_deficit_z + patch[0].detention_store);
	
	/*--------------------------------------------------------------*/
	/* Layers above snowpack and pond */
	/*--------------------------------------------------------------*/
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		patch[0].snowpack.overstory_height = zone[0].base_stations[0][0].screen_height;
		if ( (patch[0].layers[layer].height > patch[0].snowpack.height) &&
			(patch[0].layers[layer].height > pond_height) ){
			if ( command_line[0].verbose_flag == -5 ){
				printf("\n     ABOVE SNOWPACK AND POND");
			}
			patch[0].snowpack.overstory_fraction = max(patch[0].snowpack.overstory_fraction,
													   (1.0 - patch[0].layers[layer].null_cover));
			patch[0].snowpack.overstory_height = max(patch[0].snowpack.overstory_height,
													 patch[0].layers[layer].height);
			patch[0].overstory_fraction = max(patch[0].overstory_fraction,
													   (1.0 - patch[0].layers[layer].null_cover));
			patch[0].Kdown_direct_final = patch[0].layers[layer].null_cover * patch[0].Kdown_direct;
			patch[0].Kdown_diffuse_final = patch[0].layers[layer].null_cover * patch[0].Kdown_diffuse;
			patch[0].PAR_direct_final = patch[0].layers[layer].null_cover * patch[0].PAR_direct;
			patch[0].PAR_diffuse_final = patch[0].layers[layer].null_cover * patch[0].PAR_diffuse;
			patch[0].Ldown_final = patch[0].layers[layer].null_cover * patch[0].Ldown;
			patch[0].Kstar_canopy_final = patch[0].Kstar_canopy;
			patch[0].LE_canopy_final = patch[0].LE_canopy;
			patch[0].rain_throughfall_final = patch[0].layers[layer].null_cover * patch[0].rain_throughfall;
			patch[0].snow_throughfall_final = patch[0].layers[layer].null_cover * patch[0].snow_throughfall;
			patch[0].NO3_throughfall_final = patch[0].layers[layer].null_cover * patch[0].NO3_throughfall;
			patch[0].T_canopy_final = patch[0].layers[layer].null_cover * patch[0].T_canopy;
			if (dum == 0) {
				patch[0].ga_final = patch[0].layers[layer].null_cover * tmpga;
				patch[0].gasnow_final = patch[0].layers[layer].null_cover * tmpgasnow;
				patch[0].wind_final = patch[0].layers[layer].null_cover * tmpwind;
				patch[0].windsnow_final = patch[0].layers[layer].null_cover * tmpwindsnow;
				patch[0].ustar_final = patch[0].layers[layer].null_cover * tmpustar;
				if ( command_line[0].verbose_flag == -5 ){
					printf("\n     ***TOP: ga=%lf gasnow=%lf wind=%lf windsnow=%lf",patch[0].ga_final, patch[0].gasnow_final, patch[0].wind_final, patch[0].windsnow_final);
				}
			}
			else {				
				patch[0].ga_final = patch[0].layers[layer].null_cover * patch[0].ga;
				patch[0].gasnow_final = patch[0].layers[layer].null_cover * patch[0].gasnow;
				patch[0].wind_final = patch[0].layers[layer].null_cover * patch[0].wind;
				patch[0].windsnow_final = patch[0].layers[layer].null_cover * patch[0].windsnow;
				patch[0].ustar_final = patch[0].layers[layer].null_cover * patch[0].ustar;
				if ( command_line[0].verbose_flag == -5 ){
					printf("\n     ***NOT TOP: ga=%lf gasnow=%lf wind=%lf windsnow=%lf",patch[0].ga_final, patch[0].gasnow_final, patch[0].wind_final, patch[0].windsnow_final);
				}
			}

			/*--------------------------------------------------------------*/
			/*		Cycle through the canopy strata in this layer	*/
			/*--------------------------------------------------------------*/
			for ( stratum=0 ; stratum<patch[0].layers[layer].count; stratum++ ){
					canopy_stratum_daily_F(
						world,
						basin,
						hillslope,
						zone,
						patch,
						&(patch[0].layers[layer]),
						patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])],
            patch[0].shadow_strata[(patch[0].layers[layer].strata[stratum])],
						command_line,
						event,
						current_date );
				dum += 1;
			}
			patch[0].Kdown_direct = patch[0].Kdown_direct_final;
			patch[0].Kup_direct = patch[0].Kup_direct_final;
			patch[0].Kdown_diffuse = patch[0].Kdown_diffuse_final;
			patch[0].Kup_diffuse = patch[0].Kup_diffuse_final;
			patch[0].PAR_direct = patch[0].PAR_direct_final;
			patch[0].PAR_diffuse = patch[0].PAR_diffuse_final;
			patch[0].Ldown = patch[0].Ldown_final;
			patch[0].Kstar_canopy = patch[0].Kstar_canopy_final;
			patch[0].LE_canopy = patch[0].LE_canopy_final;
			patch[0].rain_throughfall = patch[0].rain_throughfall_final;
			patch[0].snow_throughfall = patch[0].snow_throughfall_final;
			patch[0].NO3_throughfall = patch[0].NO3_throughfall_final;
			patch[0].ga = patch[0].ga_final;
			patch[0].gasnow = patch[0].gasnow_final;
			patch[0].wind = patch[0].wind_final;
			patch[0].windsnow = patch[0].windsnow_final;
			patch[0].ustar = patch[0].ustar_final;
			patch[0].T_canopy = patch[0].T_canopy_final;
		}
	}
	
	/*--------------------------------------------------------------*/
	/*	Compute patch level long wave radiation processes.			*/
	/*--------------------------------------------------------------*/
	compute_Lstar(
				  command_line[0].verbose_flag,
				  basin,
				  zone,
				  patch );
	
	
	/*--------------------------------------------------------------*/
	/*	We assume the snowpack is conceptually over the		*/
	/*	current ponded water.					*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Now add the throughfall of snow	to the snowpack	 	 		*/
	/*		rain is added to the snowpack if it exists				*/
	/*		and snowpack melt allowed to occur		 				*/
	/*		this means that in rain on snow - rain is included		*/
	/*		as snowmelt												*/
	/*--------------------------------------------------------------*/
	preday_snowpack_height = patch[0].snowpack.height;
	patch[0].snowpack.water_equivalent_depth += patch[0].snow_throughfall;
	
	patch[0].Kdown_direct_subcanopy = patch[0].Kdown_direct;
	patch[0].Kdown_diffuse_subcanopy = patch[0].Kdown_diffuse;
	
	if ( command_line[0].verbose_flag == -5 ){
	printf("\n     wind=%lf windfin=%lf windsnow=%lf SWE=%lf Kstarcan=%lf Kdowndirpch=%lf Kdowndifpch=%lf detstore=%lf T_canopy=%lf", 
			patch[0].wind, 
			patch[0].wind_final, 
			patch[0].windsnow, 
			patch[0].snowpack.water_equivalent_depth, 
			patch[0].Kstar_canopy/86.4, 
			patch[0].Kdown_direct/86.4, 
			patch[0].Kdown_diffuse/86.4,
		   patch[0].detention_store,
		   patch[0].T_canopy);
	}

	
	patch[0].Kdown_direct_bare = patch[0].Kdown_direct;
	patch[0].Kdown_diffuse_bare = patch[0].Kdown_diffuse;
	

	if ( patch[0].snowpack.water_equivalent_depth > ZERO ) {
		
		/*--------------------------------------------------------------*/
		/*	Calculate snowmelt 											*/
		/*--------------------------------------------------------------*/
		/*	Check to see if snowpack is above pond. If so, proceed 	*/
		/*	with snowpack daily F.  Otherwise, melt all snow and add 	*/
		/*	to rain throughfall.										*/
		/*--------------------------------------------------------------*/
		if ( patch[0].snowpack.water_equivalent_depth > pond_height ) {
			
			/* COVER FRACTION */
			if ((patch[0].snowpack.overstory_fraction < 1) && (patch[0].snowpack.overstory_fraction > 0)) {
				if ( command_line[0].verbose_flag == -5 ){
					printf("\nSNOWPACK WITH COVER FRACTION %lf", 
						   patch[0].snowpack.overstory_fraction);
				}				
				/* Separate Kdown under canopy from patch-average Kdown below canopy layers */
				/* Using zone Kdown for exposed portion */
				Kdown_direct_covered = ( patch[0].Kdown_direct - zone[0].Kdown_direct 
											* (1 - patch[0].snowpack.overstory_fraction) )
											/ patch[0].snowpack.overstory_fraction;
				Kdown_diffuse_covered = ( patch[0].Kdown_diffuse - zone[0].Kdown_diffuse 
											* (1 - patch[0].snowpack.overstory_fraction) )
											/ patch[0].snowpack.overstory_fraction;
				Kdown_direct_exposed = zone[0].Kdown_direct;
				Kdown_diffuse_exposed = zone[0].Kdown_diffuse;
				PAR_direct_covered = ( patch[0].PAR_direct - zone[0].PAR_direct 
											* (1 - patch[0].snowpack.overstory_fraction) )
											/ patch[0].snowpack.overstory_fraction;
				PAR_diffuse_covered = ( patch[0].PAR_diffuse - zone[0].PAR_diffuse 
											* (1 - patch[0].snowpack.overstory_fraction) )
											/ patch[0].snowpack.overstory_fraction;
				PAR_direct_exposed = zone[0].PAR_direct;
				PAR_diffuse_exposed = zone[0].PAR_diffuse;
				
				/* Lundberg 1994 reduce conductance for snow vs. rain by factor of 10 */
				snow_melt_covered = snowpack_daily_F(
						current_date,
						command_line[0].verbose_flag,
						zone,
						patch,
						&patch[0].snowpack,
						basin[0].theta_noon,
						zone[0].metv.tavg,
						zone[0].e_dewpoint,
						patch[0].gasnow/10.0,
						zone[0].metv.pa,
						zone[0].cloud_fraction,
						patch[0].rain_throughfall,
						patch[0].snow_throughfall,
						&Kdown_direct_covered,
						&Kup_direct_snow_covered,
						&Kdown_diffuse_covered,
						&Kup_diffuse_snow_covered,
						&PAR_direct_covered,
						&PAR_diffuse_covered,
						patch[0].soil_defaults[0][0].maximum_snow_energy_deficit,
						patch[0].soil_defaults[0][0].snow_water_capacity,
						patch[0].soil_defaults[0][0].snow_light_ext_coef,
						patch[0].soil_defaults[0][0].snow_melt_Tcoef,
						1.0,
						patch[0].snowpack.overstory_fraction,
						0);
				snow_melt_exposed = snowpack_daily_F(
						current_date,
						command_line[0].verbose_flag,
						zone,
						patch,
						&patch[0].snowpack,
						basin[0].theta_noon,
						zone[0].metv.tavg,
						zone[0].e_dewpoint,
						patch[0].gasnow/10.0,
						zone[0].metv.pa,
						zone[0].cloud_fraction,
						patch[0].rain_throughfall,
						patch[0].snow_throughfall,
						&Kdown_direct_exposed,
						&Kup_direct_snow_exposed,
						&Kdown_diffuse_exposed,
						&Kup_diffuse_snow_exposed,
						&PAR_direct_exposed,
						&PAR_diffuse_exposed,
						patch[0].soil_defaults[0][0].maximum_snow_energy_deficit,
						patch[0].soil_defaults[0][0].snow_water_capacity,
						patch[0].soil_defaults[0][0].snow_light_ext_coef,
						patch[0].soil_defaults[0][0].snow_melt_Tcoef,
						0.0,
						(1-patch[0].snowpack.overstory_fraction),
						1);
				
				patch[0].snow_melt = (snow_melt_covered * patch[0].snowpack.overstory_fraction)
						+ (snow_melt_exposed * (1-patch[0].snowpack.overstory_fraction));
				patch[0].Kdown_direct = (Kdown_direct_covered * patch[0].snowpack.overstory_fraction)
						+ (Kdown_direct_exposed * (1-patch[0].snowpack.overstory_fraction));
				patch[0].Kdown_diffuse = (Kdown_diffuse_covered * patch[0].snowpack.overstory_fraction)
						+ (Kdown_diffuse_exposed * (1-patch[0].snowpack.overstory_fraction));
				patch[0].PAR_direct = (PAR_direct_covered * patch[0].snowpack.overstory_fraction)
						+ (PAR_direct_exposed * (1-patch[0].snowpack.overstory_fraction));
				patch[0].PAR_diffuse = (PAR_diffuse_covered * patch[0].snowpack.overstory_fraction)
						+ (PAR_diffuse_exposed * (1-patch[0].snowpack.overstory_fraction));
				
			}
			/* NO COVER FRACTION */
			else {
				if ( command_line[0].verbose_flag == -5 ){
					printf("\nSNOWPACK WITHOUT COVER FRACTION %lf", 
						   patch[0].snowpack.overstory_fraction);
				}								
				patch[0].snow_melt = snowpack_daily_F(
					current_date,
					command_line[0].verbose_flag,
					zone,
					patch,
					&patch[0].snowpack,
					basin[0].theta_noon,
					zone[0].metv.tavg,
					zone[0].e_dewpoint,
					patch[0].gasnow/10.0,
					zone[0].metv.pa,
					zone[0].cloud_fraction,
					patch[0].rain_throughfall,
					patch[0].snow_throughfall,
					&patch[0].Kdown_direct,
					&Kup_direct_snow,
					&patch[0].Kdown_diffuse,
					&Kup_diffuse_snow,
					&patch[0].PAR_direct,
					&patch[0].PAR_diffuse,
					patch[0].soil_defaults[0][0].maximum_snow_energy_deficit,
					patch[0].soil_defaults[0][0].snow_water_capacity,
					patch[0].soil_defaults[0][0].snow_light_ext_coef,
					patch[0].soil_defaults[0][0].snow_melt_Tcoef,
					patch[0].snowpack.overstory_fraction,
					1.0,
					1);
					if (patch[0].snowpack.overstory_fraction == 0) {
						Kup_direct_snow_exposed = Kup_direct_snow;
						Kup_diffuse_snow_exposed = Kup_diffuse_snow;
						}
				}
				
			/* FOR ALL COVER FRACTIONS */
			patch[0].Kup_direct += Kup_direct_snow_exposed * (1 - patch[0].snowpack.overstory_fraction);
			patch[0].Kup_diffuse += Kup_diffuse_snow_exposed * (1 - patch[0].snowpack.overstory_fraction);
		
			patch[0].snowpack.water_equivalent_depth -= patch[0].snow_melt;
			patch[0].snowpack.sublimation = min(patch[0].snowpack.sublimation, patch[0].snowpack.water_equivalent_depth);
			patch[0].snowpack.height = patch[0].snowpack.water_equivalent_depth / 0.1; /* snow density ~ 0.1 */

			if (snow_melt_input == -999.0) 
				patch[0].rain_throughfall += patch[0].snow_melt;
			else {
				patch[0].rain_throughfall += snow_melt_input;
				patch[0].snow_melt = snow_melt_input;
			}
			patch[0].snow_throughfall = 0.0;
			patch[0].snowpack.water_equivalent_depth -= patch[0].snowpack.sublimation;
			/* Force turbulent fluxes to 0 under snowpack */
			patch[0].ga = 0.0;
			patch[0].wind = 0.0;
		}
		
		else {
			patch[0].rain_throughfall += patch[0].snowpack.water_equivalent_depth;
			patch[0].snow_throughfall = 0.0;
			patch[0].snowpack.water_equivalent_depth = 0.0;
			patch[0].snowpack.height = 0.0;
		}
	}
	else{
		/*--------------------------------------------------------------*/
		/*	Just to create symmetrical output for snow and no snow	*/
		/*	days we do some fake calls which snowpack_daily_F does	*/
		/*--------------------------------------------------------------*/
		temp = 1;
		temp = compute_radiative_fluxes(0,&temp,&temp,1,1,1);
		temp = compute_radiative_fluxes(0,&temp,&temp,1,1,1);
		temp = compute_radiative_fluxes(0,&temp,&temp,1,1,1);
		temp = compute_radiative_fluxes(0,&temp,&temp,1,1,1);
		patch[0].snow_melt = 0.0;
		patch[0].snowpack.energy_deficit = 0.001;
		patch[0].snowpack.Kstar_direct = 0.0;
		patch[0].snowpack.Kstar_diffuse = 0.0;
		patch[0].snowpack.APAR_direct = 0.0;
		patch[0].snowpack.APAR_diffuse = 0.0;
		patch[0].snowpack.water_equivalent_depth = 0.0;
	}
	
	if (patch[0].snowpack.water_equivalent_depth < 0.0001) {
		patch[0].rain_throughfall += patch[0].snowpack.water_equivalent_depth;
		patch[0].snowpack.water_equivalent_depth = 0.0;
		patch[0].snowpack.energy_deficit = 0.001;
		patch[0].snowpack.surface_age = 0.0;
		patch[0].snowpack.T = 0.0;
		patch[0].snowpack.height = 0.0;
		}
	
	if ( command_line[0].verbose_flag == -5 ){
		printf("\n     AFTER SNOWPACK: Kup_direct=%lf Kup_diffuse=%lf", 
			   patch[0].Kup_direct/86.4, 
			   patch[0].Kup_diffuse/86.4);
	}
	
	
	/*--------------------------------------------------------------*/
	/*	Cycle through patch layers with height less than the	*/
	/*	snowpack but more than  0				*/
	/*	Note that the rain throughfall should equal the*/
	/*	rain and snow melt getting through the snowpack.	*/
	/*	There should be no snow_throughfall if there is a snow	*/
	/*	pack (i.e. only moss layers will have any incident	*/
	/*	snow throughfall in the loop below)			*/
	/*	We then conceptually look at it as a snowpack 		*/
	/*	"HOVERING" above any patch layers lower than its current*/
	/*	maximum height.  This is fine since if we assume that	*/
	/*	The snowpack does not transmit shortwave or par		*/
	/*	no vapour fluxes will take place anyways and only	*/
	/*	respiration will occurr which we want .			*/
	/*								*/
	/*	Patches under the snowpack but over the pond.		*/
	/*	need to use previous day (or beginning of the day)	*/
	/*	snowpack height to avoid processing some strata		*/
	/*	twice							*/
	/*--------------------------------------------------------------*/
	/* Layers below snowpack and above pond */
	/*--------------------------------------------------------------*/
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		if ( (preday_snowpack_height > 0.0) && (patch[0].layers[layer].height <= preday_snowpack_height) &&
			(patch[0].layers[layer].height > pond_height) ){
			if ( command_line[0].verbose_flag == -5 ){
				printf("\n     BELOW SNOWPACK AND ABOVE POND");
			}
			patch[0].Tday_surface_offset_final = 0.0;
			patch[0].Kdown_direct_final = patch[0].layers[layer].null_cover * patch[0].Kdown_direct;
			patch[0].Kdown_diffuse_final = patch[0].layers[layer].null_cover * patch[0].Kdown_diffuse;
			patch[0].PAR_direct_final = patch[0].layers[layer].null_cover * patch[0].PAR_direct;
			patch[0].PAR_diffuse_final = patch[0].layers[layer].null_cover * patch[0].PAR_diffuse;
			patch[0].rain_throughfall_final = patch[0].layers[layer].null_cover * patch[0].rain_throughfall;
			patch[0].snow_throughfall_final = patch[0].layers[layer].null_cover * patch[0].snow_throughfall;
			patch[0].NO3_throughfall_final = patch[0].layers[layer].null_cover * patch[0].NO3_throughfall;
			patch[0].ga_final = patch[0].layers[layer].null_cover * patch[0].ga;
			patch[0].wind_final = patch[0].layers[layer].null_cover * patch[0].wind;
			patch[0].T_canopy_final = patch[0].layers[layer].null_cover * patch[0].T_canopy;
			for ( stratum=0 ;stratum<patch[0].layers[layer].count; stratum++ ){
					canopy_stratum_daily_F(
						world,
						basin,
						hillslope,
						zone,
						patch,
						&(patch[0].layers[layer]),
						patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])],
						patch[0].shadow_strata[(patch[0].layers[layer].strata[stratum])],
						command_line,
						event,
						current_date );
			}
			patch[0].Kdown_direct = patch[0].Kdown_direct_final;
			patch[0].Kdown_diffuse = patch[0].Kdown_diffuse_final;
			patch[0].PAR_direct = patch[0].PAR_direct_final;
			patch[0].PAR_diffuse = patch[0].PAR_diffuse_final;
			patch[0].rain_throughfall = patch[0].rain_throughfall_final;
			patch[0].snow_throughfall = patch[0].snow_throughfall_final;
			patch[0].NO3_throughfall = patch[0].NO3_throughfall_final;
			patch[0].Tday_surface_offset = patch[0].Tday_surface_offset_final;
			patch[0].ga = patch[0].ga_final;
			patch[0].wind = patch[0].wind_final;
			patch[0].ustar = patch[0].ustar_final;
			patch[0].T_canopy = patch[0].T_canopy_final;
		}
	}
		
	/*--------------------------------------------------------------*/
	/*	Layers below the pond.					*/
	/*--------------------------------------------------------------*/
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		if (patch[0].layers[layer].height <= pond_height){
			if ( command_line[0].verbose_flag == -5 ){
				printf("\n     BELOW POND (INCLUDING SURFACE)");
			}
			patch[0].Tday_surface_offset_final = 0.0;
			patch[0].Kdown_direct_final = patch[0].layers[layer].null_cover * patch[0].Kdown_direct;
			patch[0].Kdown_diffuse_final = patch[0].layers[layer].null_cover * patch[0].Kdown_diffuse;
			patch[0].PAR_direct_final = patch[0].layers[layer].null_cover * patch[0].PAR_direct;
			patch[0].PAR_diffuse_final = patch[0].layers[layer].null_cover * patch[0].PAR_diffuse;
			patch[0].rain_throughfall_final = patch[0].layers[layer].null_cover * patch[0].rain_throughfall;
			patch[0].snow_throughfall_final = patch[0].layers[layer].null_cover * patch[0].snow_throughfall;
			patch[0].NO3_throughfall_final = patch[0].layers[layer].null_cover * patch[0].NO3_throughfall;
			patch[0].ga_final = patch[0].layers[layer].null_cover * patch[0].ga;
			patch[0].wind_final = patch[0].layers[layer].null_cover * patch[0].wind;
			patch[0].T_canopy_final = patch[0].layers[layer].null_cover * patch[0].T_canopy;
			for ( stratum=0 ; stratum<patch[0].layers[layer].count; stratum++ ){
					canopy_stratum_daily_F(
						world,
						basin,
						hillslope,
						zone,
						patch,
						&(patch[0].layers[layer]),
						patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])],
						patch[0].shadow_strata[(patch[0].layers[layer].strata[stratum])],
						command_line,
						event,
						current_date );
			}
			patch[0].Kdown_direct = patch[0].Kdown_direct_final;
			patch[0].Kdown_diffuse = patch[0].Kdown_diffuse_final;
			patch[0].PAR_direct = patch[0].PAR_direct_final;
			patch[0].PAR_diffuse = patch[0].PAR_diffuse_final;
			patch[0].rain_throughfall = patch[0].rain_throughfall_final;
			patch[0].snow_throughfall = patch[0].snow_throughfall_final;
			patch[0].NO3_throughfall = patch[0].NO3_throughfall_final;
			patch[0].Tday_surface_offset = patch[0].Tday_surface_offset_final;
			patch[0].ga = patch[0].ga_final;
			patch[0].wind = patch[0].wind_final;
			patch[0].ustar = patch[0].ustar_final;
			patch[0].T_canopy = patch[0].T_canopy_final;
		}
	}
	
	if ( command_line[0].verbose_flag == -5 ){
		printf("\n     PATCH DAILY POST LAYERS: ga=%lf Kdowndirpch=%lf Kdowndiffpch=%lf rainthru=%lf snowthru=%lf wind=%lf ustar=%lf Tcan=%lf", 
			   patch[0].ga,patch[0].Kdown_direct/86.4, patch[0].Kdown_diffuse/86.4, 
			   patch[0].rain_throughfall, patch[0].snow_throughfall,
			   patch[0].wind, patch[0].ustar,
			   patch[0].T_canopy);
	}

	/*--------------------------------------------------------------*/
	/*	Need to account for snow "throughfall" that is a result	*/
	/*	of leaves dropping, reducing potential interception, 	*/
	/*	and thus reducing snow storage.	This should be small	*/
	/*	enough (and generally in the fall) to ignore melting	*/
	/*	of this snow.						*/
	/*--------------------------------------------------------------*/
	patch[0].snowpack.water_equivalent_depth += patch[0].snow_throughfall;

	/*--------------------------------------------------------------*/
	/*	Do the pond energy balance now.				*/
	/*	Currently we just throw the pond in as rain.		*/
	/* 	before adding in detention store, determine input	*/
	/*	due to atm N wet dep concentration			*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/*      added in nitrogen deposition                            */
	/*	will consider N deposition as a concentration		*/
	/*	in throughfall - at moment				*/
	/*	simply added N deposition as a total flux		*/
	/*--------------------------------------------------------------*/


	if (patch[0].base_stations != NULL) {
		inx = patch[0].base_stations[0][0].dated_input[0].fertilizer_NO3.inx;
		if (inx > -999) {
			clim_event = patch[0].base_stations[0][0].dated_input[0].fertilizer_NO3.seq[inx];
			while (julday(clim_event.edate) < julday(current_date)) {
				patch[0].base_stations[0][0].dated_input[0].fertilizer_NO3.inx += 1;
				inx = patch[0].base_stations[0][0].dated_input[0].fertilizer_NO3.inx;
				clim_event = patch[0].base_stations[0][0].dated_input[0].fertilizer_NO3.seq[inx];
				}
			if ((clim_event.edate.year != 0) && ( julday(clim_event.edate) == julday(current_date)) ) {
				fertilizer_NO3 = clim_event.value;
				}
			else fertilizer_NO3 = 0.0;
			} 
		else fertilizer_NO3 = patch[0].landuse_defaults[0][0].fertilizer_NO3;
		}
	else fertilizer_NO3 = patch[0].landuse_defaults[0][0].fertilizer_NO3;

	if (patch[0].base_stations != NULL) {
		inx = patch[0].base_stations[0][0].dated_input[0].fertilizer_NH4.inx;
		if (inx > -999) {
			clim_event = patch[0].base_stations[0][0].dated_input[0].fertilizer_NH4.seq[inx];
			while (julday(clim_event.edate) < julday(current_date)) {
				patch[0].base_stations[0][0].dated_input[0].fertilizer_NH4.inx += 1;
				inx = patch[0].base_stations[0][0].dated_input[0].fertilizer_NH4.inx;
				clim_event = patch[0].base_stations[0][0].dated_input[0].fertilizer_NH4.seq[inx];
				}
			if ((clim_event.edate.year != 0) && ( julday(clim_event.edate) == julday(current_date)) ) {
				fertilizer_NH4 = clim_event.value;
				}
			else fertilizer_NH4 = 0.0;
			} 
		else fertilizer_NH4 = patch[0].landuse_defaults[0][0].fertilizer_NH4;
		}
	else fertilizer_NH4 = patch[0].landuse_defaults[0][0].fertilizer_NH4;

	/*
	if (patch[0].drainage_type == STREAM) {
		fertilizer_NO3 = 0.0;
		fertilizer_NH4 = 0.0;
		}
	*/

	patch[0].fertilizer_NO3 += fertilizer_NO3;
	patch[0].fertilizer_NH4 += fertilizer_NH4;
	//patch[0].surface_NO3 += zone[0].ndep_NO3;
	patch[0].surface_NO3 += 0.5 * patch[0].NO3_throughfall;
	patch[0].surface_NH4 += zone[0].ndep_NH4;

	/*--------------------------------------------------------------*/
	/*	a certain amount of surface_N is incorporated into the */
	/*	soil each day - we used 66% based on fertilizer experiments 	*/
	/*	Agronomy Guide 1989-1990 pg 17, Penn State web site fact sheet */
	/*--------------------------------------------------------------*/
	FERT_TO_SOIL = 100;
	if (patch[0].fertilizer_NH4 > ZERO) {
		surfaceN_to_soil = FERT_TO_SOIL * patch[0].fertilizer_NH4;
		patch[0].fertilizer_NH4 -= surfaceN_to_soil;
		patch[0].soil_ns.sminn += surfaceN_to_soil;
		}

	if (patch[0].fertilizer_NO3 > ZERO) {
		surfaceN_to_soil = FERT_TO_SOIL * patch[0].fertilizer_NO3;
		patch[0].fertilizer_NO3 -= surfaceN_to_soil;
		patch[0].soil_ns.nitrate += surfaceN_to_soil;
		}

		
	/*--------------------------------------------------------------*/
	/* adjust PH using data patch level inputs			*/
	/*--------------------------------------------------------------*/
	if (patch[0].base_stations != NULL) {
		inx = patch[0].base_stations[0][0].dated_input[0].PH.inx;
		if (inx > -999) {
			clim_event = patch[0].base_stations[0][0].dated_input[0].PH.seq[inx];
			while (julday(clim_event.edate) < julday(current_date)) {
				patch[0].base_stations[0][0].dated_input[0].PH.inx += 1;
				inx = patch[0].base_stations[0][0].dated_input[0].PH.inx;
				clim_event = patch[0].base_stations[0][0].dated_input[0].PH.seq[inx];
				}
			if ((clim_event.edate.year != 0) && ( julday(clim_event.edate) == julday(current_date)) ) {
				patch[0].PH = clim_event.value;
				}
			} 
		}

	
	/*	Add rain throughfall to detention store for infiltration	*/
	/*	and evaporation routines.									*/
	
	patch[0].detention_store += 0.5 * patch[0].rain_throughfall;
	patch[0].surface_NO3 += 0.5 * patch[0].NO3_throughfall;


	/* Calculate det store, litter, and bare soil evap first */
	
	surface_daily_F(
					world,
					basin,
					hillslope,
					zone,
					patch,
					command_line,
					event,
					current_date );	
	
	if ( command_line[0].verbose_flag == -5 ){
		printf("\n     AFTER SURFACE: Kup_direct=%lf Kup_diffuse=%lf", 
			   patch[0].Kup_direct/86.4, 
			   patch[0].Kup_diffuse/86.4);
	}
	
	patch[0].detention_store += 0.5 * patch[0].rain_throughfall;
	
	/*--------------------------------------------------------------*/
	/* if there is hourly rain input, don't run the daily infiltration	*/
	/*--------------------------------------------------------------*/
	if (zone[0].hourly_rain_flag!=1) {	
		/*--------------------------------------------------------------*/
		/* 	Above ground Hydrologic Processes			*/
		/* 	compute infiltration into the soil			*/
		/*	from snowmelt or rain_throughfall			*/
		/*	for now assume that all water infilatrates		*/
		/*	and some may be lost to deeper groundwater		*/
		/*--------------------------------------------------------------*/
		net_inflow = 0.0;
		duration = 0.0;
		if (patch[0].detention_store > ZERO) {
			/*------------------------------------------------------------------------*/
			/*	drainage to a deeper groundwater store				  */
			/*	move both nitrogen and water				    	*/
			/*------------------------------------------------------------------------*/
			if (command_line[0].gw_flag > 0) {
			if ( update_gw_drainage(patch,
					hillslope,
					zone,
					command_line,
					current_date) != 0) {
					fprintf(stderr,"fATAL ERROR: in update_decomp() ... Exiting\n");
					exit(EXIT_FAILURE);
				}
			}
			net_inflow = patch[0].detention_store;
			/*--------------------------------------------------------------*/
			/*      - if rain duration is zero, then input is from snow     */
			/*      melt  assume full daytime duration                      */
			/*--------------------------------------------------------------*/
			if (zone[0].daytime_rain_duration <= ZERO) {
				duration = zone[0].metv.dayl/(86400);
				}
			else duration = zone[0].daytime_rain_duration/(86400);
			
			if (patch[0].rootzone.depth > ZERO)	{
				infiltration = compute_infiltration(
					command_line[0].verbose_flag,
					patch[0].sat_deficit_z,
					patch[0].rootzone.S,
					patch[0].Ksat_vertical,
					patch[0].soil_defaults[0][0].Ksat_0_v,
					patch[0].soil_defaults[0][0].mz_v,
					patch[0].soil_defaults[0][0].porosity_0,
					patch[0].soil_defaults[0][0].porosity_decay,
					net_inflow,
					duration,
					patch[0].soil_defaults[0][0].psi_air_entry);
				}

			else {
				infiltration = compute_infiltration(
					command_line[0].verbose_flag,
					patch[0].sat_deficit_z,
					patch[0].S,
					patch[0].Ksat_vertical,
					patch[0].soil_defaults[0][0].Ksat_0_v,
					patch[0].soil_defaults[0][0].mz_v,
					patch[0].soil_defaults[0][0].porosity_0,
					patch[0].soil_defaults[0][0].porosity_decay,
					net_inflow,
					duration,
					patch[0].soil_defaults[0][0].psi_air_entry);
			}

		}
		else infiltration = 0.0;

		if (infiltration < 0.0) {
			printf("\nInfiltration %lf < 0 for %d on %ld",
				infiltration,
				patch[0].ID, current_date.day);
		}
		/*--------------------------------------------------------------*/
		/* determine fate of hold infiltration excess in detention store */
		/* infiltration excess will removed during routing portion	*/
		/*--------------------------------------------------------------*/
		
		infiltration = min(infiltration,patch[0].detention_store);

		/*--------------------------------------------------------------*/
		/* now take infiltration out of detention store 	*/
		/*--------------------------------------------------------------*/
		
		patch[0].detention_store -= infiltration;
			/*--------------------------------------------------------------*/
			/*	Determine if the infifltration will fill up the unsat	*/
			/*	zone or not.						*/
			/*	We use the strict assumption that sat deficit is the	*/
			/*	amount of water needed to saturate the soil.		*/
			/*--------------------------------------------------------------*/
		
		if (infiltration > ZERO) {
			/*--------------------------------------------------------------*/
			/*	Update patch level soil moisture with final infiltration.	*/
			/*--------------------------------------------------------------*/
			update_soil_moisture(
				command_line[0].verbose_flag,
				infiltration,
				net_inflow,
				patch,
				command_line,
				current_date );
		}
		
		patch[0].recharge = infiltration;
	} // end if hourly rain flag			
	/*--------------------------------------------------------------*/
	/*	Calculate patch level transpiration			*/
	/*--------------------------------------------------------------*/
	patch[0].transpiration_sat_zone = 0.0;
	patch[0].transpiration_unsat_zone = 0.0;
	patch[0].evaporation = patch[0].snowpack.sublimation;
	patch[0].PET = 0.0;
	patch[0].rain_stored = patch[0].litter.rain_stored;
	patch[0].snow_stored = 0.0;
	patch[0].ndf.plant_potential_ndemand = 0.0;
	patch[0].net_plant_psn = 0.0;
	patch[0].totalc = 0.0;
	patch[0].totaln = 0.0;
	patch[0].lai = 0.0;
	unsat_zone_patch_demand = patch[0].exfiltration_unsat_zone;
	sat_zone_patch_demand = patch[0].exfiltration_sat_zone;
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		/*--------------------------------------------------------------*/
		/*	Cycle through the canopy strata in this layer		*/
		/*								*/
		/*	We assume that the stratum already has computed a	*/
		/*	unsat_zone and sat_zone transpiration demand based on	*/
		/*	its rooting depth profile.				*/
		/*--------------------------------------------------------------*/
		for ( stratum=0 ; stratum<patch[0].layers[layer].count; stratum++ ){
			strata =
				patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])];
			/*--------------------------------------------------------------*/
			/*	Add up nitrogen demand					*/
			/*--------------------------------------------------------------*/
			patch[0].ndf.plant_potential_ndemand += strata[0].cover_fraction
				* (strata[0].ndf.potential_N_uptake);
			/*--------------------------------------------------------------*/
			/*	Add up evaporation.					*/
			/*--------------------------------------------------------------*/
			patch[0].evaporation +=	strata[0].cover_fraction
				* (strata[0].evaporation +	strata[0].sublimation) ;
			/*--------------------------------------------------------------*/
			/*      Add up canopy snow and rain stored for water balance.   */
			/*--------------------------------------------------------------*/
			patch[0].rain_stored += strata->cover_fraction * strata->rain_stored ;
			patch[0].snow_stored += strata->cover_fraction * strata->snow_stored ;
			/*--------------------------------------------------------------*/
			/*	Add uptranspiration demand 				*/
			/*--------------------------------------------------------------*/
			unsat_zone_patch_demand += strata->cover_fraction
				* strata->transpiration_unsat_zone;
			sat_zone_patch_demand += strata->cover_fraction
				* strata->transpiration_sat_zone;
			if ( command_line[0].verbose_flag > 1 ) {
				printf("\n%ld %ld %ld  -334.1 ",
					current_date.year, current_date.month, current_date.day);
				printf("\n %d %f %f %f %f %f %f %f", strata->ID,
					strata->cover_fraction,	strata->evaporation,
					strata->sublimation,strata->snow_stored,strata->rain_stored,
					strata->transpiration_unsat_zone,
					strata->transpiration_sat_zone);
			}
		}
	}

			/*--------------------------------------------------------------*/
		/* add canopy evaporation and snow sublimation to PET						*/
			/*--------------------------------------------------------------*/
		patch[0].PET = patch[0].evaporation+patch[0].evaporation_surf;

	if ( command_line[0].verbose_flag > 1 ) {
		printf("\n%ld %ld %ld  -335.1 ",
			current_date.year, current_date.month, current_date.day);
		printf("\n %d %f %f %f %f %f %f",
			patch[0].ID, patch[0].evaporation, patch[0].evaporation_surf,
			patch[0].rain_stored, patch[0].snow_stored, unsat_zone_patch_demand,
			sat_zone_patch_demand);
	}
	
	if ( command_line[0].verbose_flag == -5 ){
		printf("\n***ET DEMANDS START: rzdepth=%lf rzstor=%lf rzS=%lf rzFC=%lf rzpotsat=%lf unsatstor=%lf FC=%lf WP=%lf\n***                  S=%lf satdefz_preday=%lf satdefz=%lf satdef=%lf exfil_unsat=%lf exfil_sat=%lf unsatdemand=%lf satdemand=%lf",
			   patch[0].rootzone.depth,
			   patch[0].rz_storage,
			   patch[0].rootzone.S,
			   patch[0].rootzone.field_capacity,
			   patch[0].rootzone.potential_sat,
			   patch[0].unsat_storage,
			   patch[0].field_capacity,
			   patch[0].wilting_point,
			   patch[0].S,
			   patch[0].preday_sat_deficit_z,
			   patch[0].sat_deficit_z,
			   patch[0].sat_deficit,
			   patch[0].exfiltration_unsat_zone,
			   patch[0].exfiltration_sat_zone,
			   unsat_zone_patch_demand,
			   sat_zone_patch_demand);
	}
	
	
	/*--------------------------------------------------------------*/
	/* 	Fulfill transpiration demands				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Remember the transpiration demands.			*/
	/*--------------------------------------------------------------*/
	unsat_zone_patch_demand_initial = unsat_zone_patch_demand;
	sat_zone_patch_demand_initial = sat_zone_patch_demand;
	/*--------------------------------------------------------------*/
	/*	Figure out how much of the demand for unsat and sat zone*/
	/*	is met by supply from the zone.				*/
	/*	Update the storages while we are at it.			*/
	/*	We do not allow excess unsat storage demand beyond 	*/
	/*	current storage and what is available through cap rise. */
	/*	  Sat zone demand is not strictly limited here,		*/
	/*	although it is limited previously in 			*/
	/*	previously by rooting depth/ sat_deficit_z) 		*/
	/*								*/
	/*	this follows because					*/
	/*	ii) if the sat_zone demand is high it is because the	*/
	/*		water table is high - which means there will 	*/
	/*		is no problem meeting the sat_zone demand	*/
	/*	iii) if the unsat_zone demand is high the end of day	*/
	/*		cap rise will fill up the the unsat zone anyways*/
	/*		(note however that demands over field capacity	*/
	/*		of the unsat zone will not even be satisfied 	*/
	/*		by cap rise but thats too bad).			*/
	/*								*/
	/*	Note that the demands were decided based on 		*/
	/*	water table depths BEFORE infiltration and may bias	*/
	/*	demands on unsat zone.  However given that infiltration	*/
	/*	is big during a rain and demand small during rain it	*/
	/*	 is likely not a big deal.				*/
	/*--------------------------------------------------------------*/

	/*-------------------------------------------------------------------------*/
	/*	Compute current actual depth to water table				*/
	/*-------------------------------------------------------------------------*/
	patch[0].sat_deficit_z = compute_z_final(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		0.0,
		-1.0 * patch[0].sat_deficit);
	temp = patch[0].sat_deficit_z;
	
	available_sat_water = min((compute_delta_water(
			0, 
			patch[0].soil_defaults[0][0].porosity_0,
			patch[0].soil_defaults[0][0].porosity_decay,
			patch[0].soil_defaults[0][0].soil_depth,
			patch[0].rootzone.depth,
			patch[0].sat_deficit_z)), sat_zone_patch_demand);

	available_sat_water = max(available_sat_water, 0.0);

	patch[0].sat_deficit += available_sat_water;
	sat_zone_patch_demand -= available_sat_water;        

	patch[0].sat_deficit_z = compute_z_final(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		0.0,
		-1.0 * patch[0].sat_deficit);


		/*--------------------------------------------------------------*/
		/* 	leave behind field capacity			*/
		/*	if sat deficit has been lowered			*/
		/*	this should be an interactive process, we will use 	*/
		/*	0th order approximation					*/
		/* 	we do not do this once sat def is below 0.9 soil depth	*/
		/*     we use 0.9 to prevent numerical instability		*/
		/*--------------------------------------------------------------*/
		if (available_sat_water > ZERO) {
	       		add_field_capacity = compute_layer_field_capacity(
				command_line[0].verbose_flag,
				patch[0].soil_defaults[0][0].theta_psi_curve,
				patch[0].soil_defaults[0][0].psi_air_entry,
				patch[0].soil_defaults[0][0].pore_size_index,
				patch[0].soil_defaults[0][0].p3,
				patch[0].soil_defaults[0][0].p4,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].sat_deficit_z,
				patch[0].sat_deficit_z,
				temp);
			add_field_capacity = max(add_field_capacity, 0.0);
			patch[0].sat_deficit += add_field_capacity;
			if ((patch[0].sat_deficit_z > patch[0].rootzone.depth) && (patch[0].preday_sat_deficit_z > patch[0].rootzone.depth))				
				patch[0].unsat_storage += add_field_capacity;
			
			else if ((patch[0].sat_deficit_z <= patch[0].rootzone.depth) && (patch[0].preday_sat_deficit_z <= patch[0].rootzone.depth))
				patch[0].rz_storage += add_field_capacity;
			else  {
				patch[0].rz_storage += add_field_capacity * (patch[0].rootzone.depth -patch[0].preday_sat_deficit_z) 
					/ (patch[0].sat_deficit_z -patch[0].preday_sat_deficit_z);
				patch[0].unsat_storage += add_field_capacity * (patch[0].sat_deficit_z - patch[0].rootzone.depth) 
					/ (patch[0].sat_deficit_z -patch[0].preday_sat_deficit_z);					
			}
		}

	/*--------------------------------------------------------------*/
	/*	See how much of  unsat zone demand can be 		*/
	/*	met and still field capacity. 				*/
	/*--------------------------------------------------------------*/
    if (patch[0].rootzone.depth > ZERO ) { /* VEG CASE */
		water_above_field_cap = max((patch[0].rz_storage - patch[0].rootzone.field_capacity), 0);
		water_above_field_cap = min(unsat_zone_patch_demand, water_above_field_cap);
		patch[0].rz_storage -= water_above_field_cap;
		unsat_zone_patch_demand -= water_above_field_cap; 
	}
	else { /* NO VEG CASE (NEED TO CHECK THIS) */
		water_above_field_cap = max((patch[0].unsat_storage - patch[0].field_capacity), 0);  
		water_above_field_cap = min(unsat_zone_patch_demand, water_above_field_cap);
		patch[0].unsat_storage -= water_above_field_cap;
		unsat_zone_patch_demand -= water_above_field_cap; 
	}

	/*--------------------------------------------------------------*/
	/*	compute new field capacity				*/
	/*--------------------------------------------------------------*/

	if (patch[0].rootzone.depth > ZERO ) { /* VEG CASE */
		if (patch[0].sat_deficit_z < patch[0].rootzone.depth)  {
			patch[0].rootzone.field_capacity = compute_layer_field_capacity(
				command_line[0].verbose_flag,
				patch[0].soil_defaults[0][0].theta_psi_curve,
				patch[0].soil_defaults[0][0].psi_air_entry,
				patch[0].soil_defaults[0][0].pore_size_index,
				patch[0].soil_defaults[0][0].p3,
				patch[0].soil_defaults[0][0].p4,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].sat_deficit_z,
				patch[0].rootzone.depth, 0.0);				
				
			patch[0].field_capacity = 0.0;
			}
		else  {
			patch[0].rootzone.field_capacity = compute_layer_field_capacity(
				command_line[0].verbose_flag,
				patch[0].soil_defaults[0][0].theta_psi_curve,
				patch[0].soil_defaults[0][0].psi_air_entry,
				patch[0].soil_defaults[0][0].pore_size_index,
				patch[0].soil_defaults[0][0].p3,
				patch[0].soil_defaults[0][0].p4,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].sat_deficit_z,
				patch[0].rootzone.depth, 0.0);	

			patch[0].field_capacity = compute_layer_field_capacity(
				command_line[0].verbose_flag,
				patch[0].soil_defaults[0][0].theta_psi_curve,
				patch[0].soil_defaults[0][0].psi_air_entry,
				patch[0].soil_defaults[0][0].pore_size_index,
				patch[0].soil_defaults[0][0].p3,
				patch[0].soil_defaults[0][0].p4,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].sat_deficit_z,
				patch[0].sat_deficit_z, 0.0) - patch[0].rootzone.field_capacity;
			}

		if (patch[0].sat_deficit_z > patch[0].rootzone.depth) 
				water_below_field_cap = patch[0].rootzone.field_capacity - patch[0].rz_storage;
		else
			water_below_field_cap = patch[0].rootzone.field_capacity - patch[0].rz_storage - (patch[0].rootzone.potential_sat-patch[0].sat_deficit);
		} /* END VEG CASE */
	
	else  { /* NO VEG CASE (NEED TO CHECK THIS) */
			patch[0].field_capacity = compute_layer_field_capacity(
			   command_line[0].verbose_flag,
			   patch[0].soil_defaults[0][0].theta_psi_curve,
			   patch[0].soil_defaults[0][0].psi_air_entry,
			   patch[0].soil_defaults[0][0].pore_size_index,
			   patch[0].soil_defaults[0][0].p3,
			   patch[0].soil_defaults[0][0].p4,
			   patch[0].soil_defaults[0][0].porosity_0,
			   patch[0].soil_defaults[0][0].porosity_decay,
			   patch[0].sat_deficit_z,
			   patch[0].sat_deficit_z, 0.0);
		
		water_below_field_cap = patch[0].field_capacity - patch[0].unsat_storage;
		} /* END NO VEG CASE */
	


	/*--------------------------------------------------------------*/
	/*	fill the leftover demand with cap rise.			*/
	/*--------------------------------------------------------------*/
	cap_rise = max(min(patch[0].potential_cap_rise, min(unsat_zone_patch_demand, water_below_field_cap)), 0.0);
	cap_rise = min((compute_delta_water(
			0, 
			patch[0].soil_defaults[0][0].porosity_0,
			patch[0].soil_defaults[0][0].porosity_decay,
			patch[0].soil_defaults[0][0].soil_depth,
			patch[0].soil_defaults[0][0].soil_depth,
			patch[0].sat_deficit_z)), cap_rise);
	
	cap_rise = min(cap_rise, unsat_zone_patch_demand);
	unsat_zone_patch_demand -= cap_rise;
	patch[0].cap_rise += cap_rise;
	patch[0].potential_cap_rise -= cap_rise;
	patch[0].sat_deficit += cap_rise;				
	/*--------------------------------------------------------------*/
	/*	Now supply the remaining demand with water left in	*/
	/*	the unsat zone.  We are going below field cap now!!	*/
	/*	First guess at change in sat storage to meet demand.	*/
	/*--------------------------------------------------------------*/
	delta_unsat_zone_storage = min(unsat_zone_patch_demand, patch[0].rz_storage);

	if ((patch[0].rz_storage > ZERO) && (patch[0].sat_deficit > ZERO)) {
		patch[0].wilting_point = exp(-1.0*log(-1.0*100.0*patch[0].psi_max_veg/patch[0].soil_defaults[0][0].psi_air_entry) 
									 * patch[0].soil_defaults[0][0].pore_size_index);
		patch[0].wilting_point *= (min(patch[0].sat_deficit, patch[0].rootzone.potential_sat));
		delta_unsat_zone_storage = min(patch[0].rz_storage-patch[0].wilting_point, delta_unsat_zone_storage);
		delta_unsat_zone_storage = max(delta_unsat_zone_storage, 0.0);
		}
	else {
		patch[0].wilting_point = 0;
		}

	patch[0].rz_storage -= delta_unsat_zone_storage;
	unsat_zone_patch_demand -= delta_unsat_zone_storage;			

	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/* 	Resolve plant uptake and soil microbial N demands	*/
	/*--------------------------------------------------------------*/
	if (command_line[0].grow_flag > 0)  {
                resolve_sminn_competition(&(patch[0].soil_ns),patch[0].surface_NO3,
                        patch[0].surface_NH4,
                        patch[0].rootzone.depth,
                        patch[0].soil_defaults[0][0].active_zone_z,
                        patch[0].soil_defaults[0][0].N_decay_rate,
                        &(patch[0].ndf));
	}
	/*--------------------------------------------------------------*/
	/*	Reduce the stratum actual transpiration and compute 	*/
	/*	final N-uptake and daily allocation as a function 	*/
	/*	of available C and N (if growth flag is on)		*/
	/* 	we reduce carbon flux by current water use efficiency 	*/
	/*--------------------------------------------------------------*/

	patch[0].trans_reduc_perc = 1.0;
	transpiration_reduction_percent = 1.0;
	
	if (patch[0].rootzone.depth > ZERO ) {
	if (( unsat_zone_patch_demand_initial + sat_zone_patch_demand_initial) > ZERO)
		transpiration_reduction_percent = (1.0-(unsat_zone_patch_demand + sat_zone_patch_demand)/(unsat_zone_patch_demand_initial + sat_zone_patch_demand_initial));
	else
		transpiration_reduction_percent = 1.0;
	}

	if ( command_line[0].verbose_flag == -5 ){
		printf("\n***START: exfil_unsat=%lf exfil_sat=%lf unsatdemand_ini=%lf unsatdemand=%lf satdemand_ini=%lf satdemand=%lf",
			   patch[0].exfiltration_unsat_zone,
			   patch[0].exfiltration_sat_zone,
			   unsat_zone_patch_demand_initial,
			   unsat_zone_patch_demand,
			   sat_zone_patch_demand_initial,
			   sat_zone_patch_demand);
	}
	
	
	if ( unsat_zone_patch_demand_initial > 0 ){
		patch[0].exfiltration_unsat_zone = patch[0].exfiltration_unsat_zone
			* (1 - unsat_zone_patch_demand / unsat_zone_patch_demand_initial );
		patch[0].transpiration_unsat_zone = patch[0].transpiration_unsat_zone
			* (1 - unsat_zone_patch_demand / unsat_zone_patch_demand_initial );
		if ( command_line[0].verbose_flag == -5 ){
			printf("\n***CASE1 TRIGGERED: exfil_unsat=%lf demand_ini=%lf demand=%lf",patch[0].exfiltration_unsat_zone,unsat_zone_patch_demand_initial,unsat_zone_patch_demand);
			}
		}
	if ( sat_zone_patch_demand_initial > 0 ) {
		patch[0].exfiltration_sat_zone = patch[0].exfiltration_sat_zone
			* (1 - sat_zone_patch_demand /  sat_zone_patch_demand_initial );
		patch[0].transpiration_sat_zone = patch[0].transpiration_sat_zone
			* (1 - sat_zone_patch_demand /  sat_zone_patch_demand_initial );
		if ( command_line[0].verbose_flag == -5 ){
			printf("\n***CASE2 TRIGGERED: exfil_sat=%lf demand_ini=%lf demand=%lf",patch[0].exfiltration_sat_zone,sat_zone_patch_demand_initial,sat_zone_patch_demand);
		}		
		}
	
	patch[0].trans_reduc_perc = transpiration_reduction_percent;

	/*--------------------------------------------------------------*/
	/* add soil evap to PET																					*/
	/*--------------------------------------------------------------*/

	patch[0].PET += (patch[0].exfiltration_sat_zone + patch[0].exfiltration_unsat_zone);

	/*--------------------------------------------------------------*/
	/* in order to restrict denitri/nitrific on non-veg patches type */
	/* 	tag vegtype							*/	
	/*--------------------------------------------------------------*/
	vegtype = 0;
  patch[0].target_status = 1;

	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		for ( stratum=0 ; stratum < patch[0].layers[layer].count; stratum++ ){
			strata =
				patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])];
   
	    if(command_line[0].vegspinup_flag > 0){
        if (strata->target.met == 0)
          patch[0].target_status = 0;
        }
			  if ( (strata[0].defaults[0][0].epc.veg_type != NON_VEG) ){

			   	if (transpiration_reduction_percent < 1.0) {
				  strata->cdf.psn_to_cpool = strata->cdf.psn_to_cpool  * transpiration_reduction_percent;
				  strata->cs.availc = (strata->cs.availc + strata->cdf.total_mr)  * transpiration_reduction_percent - strata->cdf.total_mr;
				  strata->gs_sunlit *= transpiration_reduction_percent;		
				  strata->gs_shade *= transpiration_reduction_percent;		
				  strata->mult_conductance.LWP *= transpiration_reduction_percent;
				  strata->ndf.potential_N_uptake *= transpiration_reduction_percent;
				}

				vegtype=1;
				canopy_stratum_growth(
					world,
					basin,
					hillslope,
					zone,
					patch,
					strata,
					command_line,
					event,
					current_date );
			}
			else { 
				if ( strata->phen.annual_allocation == 1){
					strata->cdf.leafc_store_to_leafc_transfer =
						strata->cs.leafc_store;
					strata->cs.leafc_transfer +=
						strata->cdf.leafc_store_to_leafc_transfer;
					strata->cs.leafc_store -=
						strata->cdf.leafc_store_to_leafc_transfer;
					strata->ndf.leafn_store_to_leafn_transfer =
						strata->ns.leafn_store;
					strata->ns.leafn_transfer +=
						strata->ndf.leafn_store_to_leafn_transfer;
					strata->ns.leafn_store -=
						strata->ndf.leafn_store_to_leafn_transfer;
				}
			}
			if ( unsat_zone_patch_demand_initial > 0.0 )
				strata->transpiration_unsat_zone = strata->transpiration_unsat_zone
				*(1-unsat_zone_patch_demand/unsat_zone_patch_demand_initial);
			if ( sat_zone_patch_demand_initial > 0.0 )
				strata->transpiration_sat_zone = strata->transpiration_sat_zone
				*(1 - sat_zone_patch_demand / sat_zone_patch_demand_initial );
			patch[0].transpiration_unsat_zone += strata->cover_fraction
				* strata->transpiration_unsat_zone;
			patch[0].transpiration_sat_zone += strata->cover_fraction
				* strata->transpiration_sat_zone;
			patch[0].PET += strata->cover_fraction * strata->PET;
			patch[0].totalc += strata->cover_fraction * strata->cs.totalc;
			patch[0].totaln += strata->cover_fraction * strata->ns.totaln;
			patch[0].net_plant_psn += strata->cover_fraction *	strata->cs.net_psn;
			strata->acc_year.psn += strata->cs.net_psn;
			patch[0].lai += strata->cover_fraction * strata->epv.proj_lai;
		}
	}
	/*-------------------------------------------------------------------------*/
	/*	Compute current actual depth to water table				*/
	/*------------------------------------------------------------------------*/
	patch[0].sat_deficit_z = compute_z_final(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		0.0,
		-1.0 * patch[0].sat_deficit);

	/*--------------------------------------------------------------*/
	/*      Recompute patch soil moisture storage                   */
	/*--------------------------------------------------------------*/
	if (patch[0].sat_deficit < ZERO) {
		patch[0].S = 1.0;
		patch[0].rootzone.S = 1.0;
		rz_drainage = 0.0;
		unsat_drainage = 0.0;
	}
	else if (patch[0].sat_deficit_z > patch[0].rootzone.depth)  {		/* Constant vertical profile of soil porosity */
		/*-------------------------------------------------------*/
		/*	soil drainage and storage update	     	 */
		/*-------------------------------------------------------*/
		
		patch[0].rootzone.S = min(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);
		rz_drainage = compute_unsat_zone_drainage(
			command_line[0].verbose_flag,
			patch[0].soil_defaults[0][0].theta_psi_curve,
			patch[0].soil_defaults[0][0].pore_size_index,
			patch[0].rootzone.S,
			patch[0].soil_defaults[0][0].mz_v,
			patch[0].rootzone.depth,
			patch[0].soil_defaults[0][0].Ksat_0_v / 2,
			patch[0].rz_storage - patch[0].rootzone.field_capacity);


		patch[0].rz_storage -=  rz_drainage;
		patch[0].unsat_storage +=  rz_drainage;
		
		patch[0].S = patch[0].unsat_storage / (patch[0].sat_deficit - patch[0].rootzone.potential_sat);	
		patch[0].rootzone.S = min(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);
		unsat_drainage = compute_unsat_zone_drainage(
			command_line[0].verbose_flag,
			patch[0].soil_defaults[0][0].theta_psi_curve,
			patch[0].soil_defaults[0][0].pore_size_index,
			patch[0].S,
			patch[0].soil_defaults[0][0].mz_v,
			patch[0].sat_deficit_z,
			patch[0].soil_defaults[0][0].Ksat_0_v / 2,
			patch[0].unsat_storage - patch[0].field_capacity);

		patch[0].unsat_storage -=  unsat_drainage;
		patch[0].sat_deficit -=  unsat_drainage;
	}									
	else  {
		patch[0].rz_storage += patch[0].unsat_storage;	/* transfer left water in unsat storage to rootzone layer */
		patch[0].unsat_storage = 0.0;   

		patch[0].S = min(patch[0].rz_storage / patch[0].sat_deficit, 1.0);
		rz_drainage = compute_unsat_zone_drainage(
			command_line[0].verbose_flag,
			patch[0].soil_defaults[0][0].theta_psi_curve,
			patch[0].soil_defaults[0][0].pore_size_index,
			patch[0].S,
			patch[0].soil_defaults[0][0].mz_v,
			patch[0].sat_deficit_z,
			patch[0].soil_defaults[0][0].Ksat_0 / 2,
			patch[0].rz_storage - patch[0].rootzone.field_capacity);		

		unsat_drainage = 0.0;

		patch[0].rz_storage -=  rz_drainage;
		patch[0].sat_deficit -=  rz_drainage;
	}	
	patch[0].unsat_drainage += unsat_drainage;
	patch[0].rz_drainage += rz_drainage;
	patch[0].hourly_unsat_drainage += unsat_drainage;
	patch[0].hourly_rz_drainage += rz_drainage;
	/* ---------------------------------------------- */
	/*     Final rootzone saturation calculation      */
	/* ---------------------------------------------- */
	if (patch[0].sat_deficit > patch[0].rootzone.potential_sat)
		patch[0].rootzone.S = min(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);
	else 
		patch[0].rootzone.S = min((patch[0].rz_storage + patch[0].rootzone.potential_sat - patch[0].sat_deficit)
			/ patch[0].rootzone.potential_sat, 1.0);								
	/*-----------------------------------------------------*/
	/*  re-Compute potential saturation for rootzone layer   */
	/*-----------------------------------------------------*/			
	if (patch[0].rootzone.depth > ZERO)
		patch[0].rootzone.potential_sat = compute_delta_water(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		patch[0].rootzone.depth, 0.0);			

	patch[0].delta_snowpack = patch[0].snowpack.water_depth
		+ patch[0].snowpack.water_equivalent_depth - patch[0].preday_snowpack;
	patch[0].delta_rain_stored = patch[0].rain_stored
		- patch[0].preday_rain_stored;
	patch[0].delta_snow_stored = patch[0].snow_stored
		- patch[0].preday_snow_stored;

	/*------------------------------------------------------------------------*/
	/*	Compute current actual depth to water table				*/
	/*------------------------------------------------------------------------*/
	patch[0].sat_deficit_z = compute_z_final(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		0.0,
		-1.0 * patch[0].sat_deficit);


	theta = patch[0].rootzone.S;
	patch[0].theta_std = (patch[0].soil_defaults[0][0].theta_mean_std_p2*theta*theta + 
				patch[0].soil_defaults[0][0].theta_mean_std_p1*theta);
	/*-------------------------------------------------------------------------*/
	/*	finalized soil and litter decomposition					*/
	/* 	and any septic losses							*/
	/*------------------------------------------------------------------------*/
	if ((command_line[0].grow_flag > 0) && (vegtype == 1)) {
		
		if ( update_decomp(
			current_date,
			&(patch[0].soil_cs),
			&(patch[0].soil_ns),
			&(patch[0].litter_cs),
			&(patch[0].litter_ns),
			&(patch[0].cdf),
			&(patch[0].ndf),
			patch) != 0){
			fprintf(stderr,"fATAL ERROR: in update_decomp() ... Exiting\n");
			exit(EXIT_FAILURE);
		}


		if (patch[0].soil_defaults[0][0].DON_production_rate > ZERO) {
			if ( update_dissolved_organic_losses(
				current_date,
				patch[0].soil_defaults[0][0].DON_production_rate,
				&(patch[0].soil_cs),
				&(patch[0].soil_ns),
				&(patch[0].litter_cs),
				&(patch[0].litter_ns),
				&(patch[0].cdf),
				&(patch[0].ndf)) != 0){
				fprintf(stderr,"fATAL ERROR: in update_decomp() ... Exiting\n");
				exit(EXIT_FAILURE);
			}
		patch[0].surface_DOC += (patch[0].cdf.do_litr1c_loss + 
				patch[0].cdf.do_litr2c_loss + patch[0].cdf.do_litr3c_loss + patch[0].cdf.do_litr4c_loss);
		patch[0].surface_DON += (patch[0].ndf.do_litr1n_loss + patch[0].ndf.do_litr2n_loss + patch[0].ndf.do_litr3n_loss + 
				 patch[0].ndf.do_litr4n_loss);
		}

		if ( update_nitrif(
			&(patch[0].soil_cs),
			&(patch[0].soil_ns),
			&(patch[0].cdf),
			&(patch[0].ndf),
			patch[0].soil_defaults[0][0].soil_type,
			patch[0].PH, 
			patch[0].rootzone.S,
			patch[0].Tsoil,
			patch[0].soil_defaults[0][0].porosity_0,
			0.25,
			patch[0].soil_defaults[0][0].NO3_adsorption_rate,patch[0].theta_std) != 0){
			fprintf(stderr,"fATAL ERROR: in update_nitrific() ... Exiting\n");
			exit(EXIT_FAILURE);
		}
		if ( update_denitrif(
			&(patch[0].soil_cs),
			&(patch[0].soil_ns),
			&(patch[0].cdf),
			&(patch[0].ndf),
			patch[0].soil_defaults[0][0].soil_type,
			patch[0].rootzone.S, patch[0].theta_std) != 0){
			fprintf(stderr,"fATAL ERROR: in update_denitrif() ... Exiting\n");
			exit(EXIT_FAILURE);
		}

	}


	/* track variables for snow assimilation  */
	if (patch[0].snowpack.water_equivalent_depth > ZERO) {
		basin[0].snowpack.energy_deficit += patch[0].snowpack.energy_deficit * patch[0].area;
		basin[0].snowpack.surface_age += patch[0].snowpack.surface_age * patch[0].area;
		basin[0].snowpack.T += patch[0].snowpack.T * patch[0].area;
		basin[0].area_withsnow += patch[0].area;
		}

	/* track variables for fire spread */
	if (command_line[0].firespread_flag == 1) {
		patch[0].fire.et = (patch[0].fire_defaults[0][0].ndays_average*patch[0].fire.et  +  
		(patch[0].transpiration_sat_zone + patch[0].transpiration_unsat_zone
		+ patch[0].evaporation + patch[0].evaporation_surf 
		+ patch[0].exfiltration_unsat_zone + patch[0].exfiltration_sat_zone))/
		(patch[0].fire_defaults[0][0].ndays_average + 1);

		patch[0].fire.pet = (patch[0].fire_defaults[0][0].ndays_average*patch[0].fire.pet    
				+ patch[0].PET) / 
		(patch[0].fire_defaults[0][0].ndays_average + 1);
		}
	


	patch[0].soil_cs.totalc = ((patch[0].soil_cs.soil1c)
		+ (patch[0].soil_cs.soil2c) +	(patch[0].soil_cs.soil3c)
		+ (patch[0].soil_cs.soil4c));
	patch[0].totalc += ((patch[0].soil_cs.totalc) + (patch[0].litter_cs.litr1c)
		+ (patch[0].litter_cs.litr2c) + (patch[0].litter_cs.litr3c)
		+ (patch[0].litter_cs.litr4c));
	patch[0].soil_ns.totaln = ((patch[0].soil_ns.soil1n)
		+ (patch[0].soil_ns.soil2n) + (patch[0].soil_ns.soil3n)
		+ (patch[0].soil_ns.soil4n) + (patch[0].soil_ns.nitrate)
		+ (patch[0].soil_ns.sminn));
	patch[0].totaln += (patch[0].soil_ns.totaln + (patch[0].litter_ns.litr1n)
		+ (patch[0].litter_ns.litr2n) + (patch[0].litter_ns.litr3n)
		+ (patch[0].litter_ns.litr4n));
	patch[0].nitrogen_balance = patch[0].preday_totaln - patch[0].totaln - patch[0].ndf.N_to_gw
		+ zone[0].ndep_NO3 + zone[0].ndep_NH4 - patch[0].ndf.denitrif + fertilizer_NO3 + fertilizer_NH4;

	resp =  (patch[0].cdf.litr1c_hr + patch[0].cdf.litr2c_hr
		+ patch[0].cdf.litr4c_hr + patch[0].cdf.soil1c_hr
		+ patch[0].cdf.soil2c_hr + patch[0].cdf.soil3c_hr
		+ patch[0].cdf.soil4c_hr);
	patch[0].carbon_balance = patch[0].preday_totalc + patch[0].net_plant_psn
		- patch[0].totalc - (patch[0].cdf.litr1c_hr + patch[0].cdf.litr2c_hr
		+ patch[0].cdf.litr4c_hr + patch[0].cdf.soil1c_hr
		+ patch[0].cdf.soil2c_hr + patch[0].cdf.soil3c_hr
		+ patch[0].cdf.soil4c_hr);

	if (command_line[0].snow_scale_flag == 1)
	  patch[0].water_balance = zone[0].rain + zone[0].snow*patch[0].snow_redist_scale 
		+ patch[0].preday_detention_store +
		+ irrigation 
		+ patch[0].landuse_defaults[0][0].septic_water_load 
		+ zone[0].rain_hourly_total - ( patch[0].gw_drainage
		+ patch[0].transpiration_sat_zone + patch[0].transpiration_unsat_zone
		+ patch[0].evaporation + patch[0].evaporation_surf 
		+ patch[0].exfiltration_unsat_zone + patch[0].exfiltration_sat_zone)
		- (patch[0].rz_storage - patch[0].preday_rz_storage)		
		- (patch[0].unsat_storage - patch[0].preday_unsat_storage)
		- (patch[0].preday_sat_deficit - patch[0].sat_deficit)
		- patch[0].delta_snowpack - patch[0].delta_rain_stored
		- patch[0].delta_snow_stored - patch[0].detention_store;
	else	
	  patch[0].water_balance = zone[0].rain + zone[0].snow 
		+ patch[0].preday_detention_store +
		+ irrigation 
		+ patch[0].landuse_defaults[0][0].septic_water_load 
		+ zone[0].rain_hourly_total - ( patch[0].gw_drainage
		+ patch[0].transpiration_sat_zone + patch[0].transpiration_unsat_zone
		+ patch[0].evaporation + patch[0].evaporation_surf 
		+ patch[0].exfiltration_unsat_zone + patch[0].exfiltration_sat_zone)
		- (patch[0].rz_storage - patch[0].preday_rz_storage)			
		- (patch[0].unsat_storage - patch[0].preday_unsat_storage)
		- (patch[0].preday_sat_deficit - patch[0].sat_deficit)
		- patch[0].delta_snowpack - patch[0].delta_rain_stored
		- patch[0].delta_snow_stored - patch[0].detention_store;

	/*
	if ((patch[0].water_balance > 0.00000001)||
		(patch[0].water_balance < -0.00000001)){
		printf("\n Water Balance is %12.8f on %ld %ld %ld for patch %d of type %d",
			patch[0].water_balance,
			current_date.day,
			current_date.month,
			current_date.year,
			patch[0].ID,
			patch[0].drainage_type);
		printf("\nRain %lf %lf, dt %lf, rh %lf, T %lf %lf, E %lf ES %lf, Ex %lf %lf, RZ %lf %lf, US %lf %lf, SD %lf %lf, S %lf, RS %lf (%lf %lf), SS %lf, DT %lf",
		 zone[0].rain , zone[0].snow , patch[0].preday_detention_store ,
		 zone[0].rain_hourly_total ,  
		 patch[0].transpiration_sat_zone , patch[0].transpiration_unsat_zone
		, patch[0].evaporation , patch[0].evaporation_surf
		, patch[0].exfiltration_unsat_zone , patch[0].exfiltration_sat_zone
		, patch[0].rz_storage , patch[0].preday_rz_storage
		, patch[0].unsat_storage , patch[0].preday_unsat_storage
		, patch[0].preday_sat_deficit , patch[0].sat_deficit
		, patch[0].delta_snowpack , patch[0].delta_rain_stored
		, patch[0].preday_rain_stored, patch[0].rain_stored
		, patch[0].delta_snow_stored , patch[0].detention_store);	
	
	}
	 
	*/
	
	/* Calculate LE for surface evap */
	/* soil&litter&detstore evap x latent heat vaporization x water density */
	patch[0].LE_soil = (patch[0].evaporation_surf + patch[0].exfiltration_sat_zone 
						+ patch[0].exfiltration_unsat_zone) 
						* (2.5023e6 - 2430.54 * zone[0].metv.tday) / 1000 * 1000;
	
	
	/*---------------------------------------------------------------------*/
	/*	get rid of any negative soil or litter stores			*/
	/*---------------------------------------------------------------------*/

	if (command_line[0].grow_flag > 0)
		ch = check_zero_stores(
			&(patch[0].soil_cs),
			&(patch[0].soil_ns),
			&(patch[0].litter_cs),
			&(patch[0].litter_ns));

	if ( command_line[0].verbose_flag > 1 ) {
		printf("\n%ld %ld %ld  -335.2 ",
			current_date.year, current_date.month, current_date.day);
		printf("\n   %8.5f %8.5f %8.5f %8.5f %8.5f %8.5f %8.5f %8.5f ",
			infiltration,
			patch[0].snowpack.water_equivalent_depth,
			patch[0].infiltration_excess,
			patch[0].transpiration_sat_zone + patch[0].transpiration_unsat_zone,
			patch[0].unsat_drainage,
			patch[0].unsat_storage,
			patch[0].cap_rise,
			patch[0].sat_deficit);
		printf("\n%ld %ld %ld  -335.3 ",
			current_date.year, current_date.month, current_date.day);
		printf("\n   %8.5f, %8.5f %8.5f %8.5f %8.5f %8.5f %8.5f %8.5f",
			zone[0].rain + zone[0].snow,
			patch[0].infiltration_excess,
			patch[0].transpiration_sat_zone + patch[0].transpiration_unsat_zone,
			patch[0].evaporation + patch[0].exfiltration_sat_zone
			+ patch[0].exfiltration_unsat_zone,
			(patch[0].unsat_storage - patch[0].preday_unsat_storage),
			(patch[0].preday_sat_deficit - patch[0].sat_deficit),
			patch[0].delta_snowpack,
			patch[0].delta_rain_stored + patch[0].delta_snow_stored);
	}
if ( command_line[0].verbose_flag == -5 ){
	printf("\n***END PATCH DAILY: exfil_unsat=%lf",patch[0].exfiltration_unsat_zone);
}

	return;
} /*end patch_daily_F.c*/
