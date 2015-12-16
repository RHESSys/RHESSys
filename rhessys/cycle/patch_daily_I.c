/*--------------------------------------------------------------*/
/* 								*/
/*		patch_daily_I					*/
/*								*/
/*	NAME							*/
/*	patch_daily_I 						*/
/*		 - performs cycling of patch state vars		*/
/*			at the START OF THE DAY			*/ 
/*								*/
/*	SYNOPSIS 						*/
/*	void patch_daily_I(					*/
/*			 struct patch_object	,		*/
/*			 struct command_line_object ,		*/
/*			 struct tec_entry,			*/
/*			 struct date)				*/
/*								*/
/*	OPTIONS							*/
/*	struct	world_object *world,				*/
/*	struct	basin_object *basin,				*/
/*	struct 	hillslope_object *hillslope,			*/
/*	struct  zone_object *zone,				*/
/*	struct patch_object *patch,				*/
/*	struct command_line_object *command_line,		*/
/*	struct command_line_object *command_line,		*/
/*	struct	tec_entry	*event,				*/
/*	struct	date current_date - local time (?)		*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	This routine performs simulation cycles on an identified*/
/*	canopy_stata in the patch.				*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	March 12, 1997	C.Tague					*/
/*	- added calculation for patch effective lai		*/
/*								*/
/*								*/
/*	Sept 15 1997	RAF					*/
/*	Substantially modified accounting of current water	*/
/*	equivalent depth to sat zone and unsat_storage		*/
/*								*/
/*	Sept 29 1997 CT						*/
/*	switched above to an implementation using sat_deficit	*/
/*	as the TOP_model control volume - see discussion 	*/
/*	there							*/
/*								*/
/*	Oct 22 1997 CT						*/
/*								*/
/*	unsat storage now subtracted from sat_deficit after	*/
/*	return flow calculated - in previous version this	*/
/*	step was missing which  results in a 			*/
/*	serious over-estimation of sat_deficit after		*/
/*	return flow events					*/	
/*								*/
/*	Feb 2 1998 RAF						*/
/*	Included potential exfiltration module.			*/
/*								*/
/*	April 28 1998 RAF					*/
/*	Excluded stratum of height 0 from computation of	*/
/*	effective LAI of site.					*/
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include "rhessys.h"
#include "functions.h"

void		patch_daily_I(
						  struct	world_object *world,
						  struct	basin_object *basin,
						  struct 	hillslope_object *hillslope,
						  struct  zone_object *zone,
						  struct patch_object *patch,
						  struct command_line_object *command_line,
						  struct	tec_entry	*event,
						  struct	date current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void   canopy_stratum_daily_I(
		struct	world_object *,
		struct	basin_object *,
		struct 	hillslope_object *,
		struct  zone_object *,
		struct	patch_object *,
		struct canopy_strata_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	double	compute_layer_field_capacity(
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
	
	
	double	compute_delta_water(
		int,
		double,
		double,
		double,
		double,
		double);

	double	compute_z_final(
		int,
		double,
		double,
		double,
		double,
		double);

	int	update_rootzone_moist(
		struct patch_object	*,
		struct	rooting_zone_object	*,
		struct command_line_object *);
	
	double	compute_capillary_rise(
		int,
		double,
		double,
		double,
		double,
		double);
	
	double  compute_soil_water_potential(
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
		double,
		double);
	
	int  compute_potential_decomp(
		double,
		double,
		double,
		double,
		double,
		struct  soil_c_object   *,
		struct  soil_n_object   *,
		struct  litter_c_object *,
		struct  litter_n_object *,
		struct  cdayflux_patch_struct *,
		struct  ndayflux_patch_struct *);
	
	void    sort_patch_layers(struct patch_object *);

		
	void	update_litter_interception_capacity (double, 
		double,
		struct litter_c_object *,
		struct litter_object *);

	int	zero_patch_daily_flux(
		struct	patch_object *,
		struct  cdayflux_patch_struct *,
		struct  ndayflux_patch_struct *);
	
	
	long julday( struct date);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	layer, inx;
	int	stratum;
	double	cnt, count, theta;
	
	double  edible_leafc, grazing_mean_nc, grazing_Closs;
	struct  canopy_strata_object *strata;
	struct  dated_sequence	clim_event;

	/*--------------------------------------------------------------*/
	/*	zero out daily fluxes					*/
	/*--------------------------------------------------------------*/
	if (zero_patch_daily_flux(patch, &(patch[0].cdf), &(patch[0].ndf))){
		fprintf(stderr,
			"Error in zero_patch_daily_flux() from patch_daily_I.c... Exiting\n");
		exit(EXIT_FAILURE);
	}


	patch[0].precip_with_assim = 0.0;

	
	/*-----------------------------------------------------*/
	/*  Compute potential saturation for rootzone layer   */
	/*-----------------------------------------------------*/			
	if (patch[0].rootzone.depth > ZERO)  {
	patch[0].rootzone.potential_sat = compute_delta_water(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		patch[0].rootzone.depth, 0.0);			
	 if (patch[0].rootzone.potential_sat > ZERO)
		if (patch[0].sat_deficit_z > patch[0].rootzone.depth)	
		patch[0].rootzone.S = patch[0].rz_storage/patch[0].rootzone.potential_sat;
		else
		patch[0].rootzone.S = min((patch[0].rz_storage + patch[0].rootzone.potential_sat - patch[0].sat_deficit)/patch[0].rootzone.potential_sat,1.0);
	else
		patch[0].rootzone.S = 0.0;
	}
	else  {
		patch[0].rootzone.potential_sat = 0.0;
		patch[0].rootzone.S = 0.0;
		}

	if (patch[0].sat_deficit < ZERO) 
		patch[0].S = 1.0;
	else
		patch[0].S = (patch[0].rz_storage+patch[0].unsat_storage)/patch[0].sat_deficit;
	
	/*--------------------------------------------------------------*/
	/*  compute standard deviation of theta based on soil parameters */
	/* assume no decay of porosity here 				*/
	/*--------------------------------------------------------------*/
	theta = patch[0].S * patch[0].soil_defaults[0][0].porosity_0;
	patch[0].theta_std = (patch[0].soil_defaults[0][0].theta_mean_std_p2*theta*theta + 
				patch[0].soil_defaults[0][0].theta_mean_std_p1*theta);

	/*--------------------------------------------------------------*/
	/*	compute new field capacity				*/
	/*--------------------------------------------------------------*/

	if (patch[0].sat_deficit_z <= patch[0].rootzone.depth)  {
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
		if ( command_line[0].verbose_flag == -5 ){
			printf("\n***PCHDAILYI CASE1: satdefz=%lf rzdepth=%lf rzFC=%lf FC=%lf",
				   patch[0].sat_deficit_z,
				   patch[0].rootzone.depth,
				   patch[0].rootzone.field_capacity,
				   patch[0].field_capacity);
		}
		
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
		
		if ( command_line[0].verbose_flag == -5 ){
			printf("\n***PCHDAILYI CASE2: satdefz=%lf rzdepth=%lf rzFC=%lf FC=%lf",
				   patch[0].sat_deficit_z,
				   patch[0].rootzone.depth,
				   patch[0].rootzone.field_capacity,
				   patch[0].field_capacity);
		}
		
	}



	/*--------------------------------------------------------------*/
	/*	Estimate potential cap rise.				*/
	/*	limited to water in sat zone.				*/
	/*--------------------------------------------------------------*/
	patch[0].potential_cap_rise =	compute_capillary_rise(
		command_line[0].verbose_flag,
		patch[0].sat_deficit_z,
		patch[0].soil_defaults[0][0].psi_air_entry,
		patch[0].soil_defaults[0][0].pore_size_index,
		patch[0].soil_defaults[0][0].mz_v,
		patch[0].soil_defaults[0][0].Ksat_0_v );
	if (patch[0].potential_cap_rise < ZERO)
		patch[0].potential_cap_rise = 0.0;
	patch[0].cap_rise=0.0;



	/*--------------------------------------------------------------*/
	/*	Compute the max exfiltration rate.			*/
	/*								*/
	/*	First check if we are saturated.  If so the 		*/
	/*	potential exfiltration rate is the capillary rise rate.	*/
	/*								*/
	/*	If we are within the active unsat zone then we assume	*/
	/*	that the potential exfiltration rate is the minimum of	*/
	/*	the computed exfiltration rate and the potential cap	*/
	/*	rise - i.e. hydrologic connectivity between surface and	*/
	/*	water table.						*/
	/*								*/
	/*--------------------------------------------------------------*/
	if ( patch[0].sat_deficit_z <= patch[0].soil_defaults[0][0].psi_air_entry ){
		patch[0].potential_exfiltration = patch[0].potential_cap_rise;
	}
	else {
		if ( patch[0].soil_defaults[0][0].active_zone_z < patch[0].sat_deficit_z ){
			/*--------------------------------------------------------------*/
			/*	Estimate potential exfiltration from active zone 	*/
			/*--------------------------------------------------------------*/
			/* Added new check to see if non-zero root depth... was unintentionally */
			/* zeroing out potential exfil in non-vegetated cases.	-AD				*/
			if (patch[0].rootzone.depth > ZERO)	{
				patch[0].potential_exfiltration = compute_potential_exfiltration(
					command_line[0].verbose_flag,
					patch[0].rootzone.S,
					patch[0].soil_defaults[0][0].active_zone_z,
					patch[0].soil_defaults[0][0].Ksat_0_v,
					patch[0].soil_defaults[0][0].mz_v,
					patch[0].soil_defaults[0][0].psi_air_entry,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].soil_defaults[0][0].porosity_0);
				}
			else {
			patch[0].potential_exfiltration = compute_potential_exfiltration(
					command_line[0].verbose_flag,
					patch[0].S,
					patch[0].soil_defaults[0][0].active_zone_z,
					patch[0].soil_defaults[0][0].Ksat_0_v,
					patch[0].soil_defaults[0][0].mz_v,
					patch[0].soil_defaults[0][0].psi_air_entry,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].soil_defaults[0][0].porosity_0);
				}
		}
		else {
			/*--------------------------------------------------------------*/
			/*	Estimate potential exfiltration from active zone 	*/
			/*--------------------------------------------------------------*/
			if (patch[0].rootzone.depth > ZERO)	{
				patch[0].potential_exfiltration = compute_potential_exfiltration(
					command_line[0].verbose_flag,
					patch[0].rootzone.S,
					patch[0].sat_deficit_z,
					patch[0].soil_defaults[0][0].Ksat_0_v,
					patch[0].soil_defaults[0][0].mz_v,
					patch[0].soil_defaults[0][0].psi_air_entry,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].soil_defaults[0][0].porosity_0);
				}
			else {
				patch[0].potential_exfiltration = compute_potential_exfiltration(
					command_line[0].verbose_flag,
					patch[0].S,
					patch[0].sat_deficit_z,
					patch[0].soil_defaults[0][0].Ksat_0_v,
					patch[0].soil_defaults[0][0].mz_v,
					patch[0].soil_defaults[0][0].psi_air_entry,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].soil_defaults[0][0].porosity_0);
				}
		}
	}



	/*-----------------------------------------------------*/
	/* 	Check for any grazing activity from a land use default file			*/
	/*-----------------------------------------------------*/
	if (patch[0].base_stations != NULL) {
		inx = patch[0].base_stations[0][0].dated_input[0].grazing_Closs.inx;
		if (inx > -999) {
			clim_event = patch[0].base_stations[0][0].dated_input[0].grazing_Closs.seq[inx];
			while (julday(clim_event.edate) < julday(current_date)) {
				patch[0].base_stations[0][0].dated_input[0].grazing_Closs.inx += 1;
				inx = patch[0].base_stations[0][0].dated_input[0].grazing_Closs.inx;
				clim_event = patch[0].base_stations[0][0].dated_input[0].grazing_Closs.seq[inx];
				}
			if ((clim_event.edate.year != 0) && ( julday(clim_event.edate) == julday(current_date)) ) {
				grazing_Closs = clim_event.value;
				}
			else grazing_Closs = 0.0;
			} 
		else grazing_Closs = patch[0].landuse_defaults[0][0].grazing_Closs;
		}
	else grazing_Closs = patch[0].landuse_defaults[0][0].grazing_Closs;
	patch[0].grazing_Closs = grazing_Closs;

	/*--------------------------------------------------------------*/
	/*	Cycle through the canopy layers.			*/
	/*--------------------------------------------------------------*/
	edible_leafc = 0.0;
	grazing_mean_nc = 0.0;
	cnt = 0;
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		/*--------------------------------------------------------------*/
		/*	Cycle through the canopy strata				*/
		/*--------------------------------------------------------------*/
		for ( stratum=0 ; stratum<patch[0].layers[layer].count; stratum++ ){

			strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])];
			patch[0].preday_rain_stored += strata->cover_fraction * strata->rain_stored;
			patch[0].preday_snow_stored += strata->cover_fraction * strata->snow_stored;
			if ((strata[0].defaults[0][0].epc.edible == 1) && (strata[0].cs.leafc > ZERO)) {
				edible_leafc += strata->cs.leafc * strata->cover_fraction;
				cnt += 1;
				grazing_mean_nc += strata->ns.leafn/strata->cs.leafc * strata->cover_fraction;
				}
			canopy_stratum_daily_I(
				world,
				basin,
				hillslope,
				zone,
				patch,
				patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])],
				command_line,
				event,
				current_date );
		}
	}
	patch[0].grazing_Closs = min(edible_leafc, patch[0].grazing_Closs);
	if (cnt > 0)
		patch[0].grazing_mean_nc = grazing_mean_nc / cnt;

	/*--------------------------------------------------------------*/
	/*	Calculate effective patch lai from stratum					*/
	/*	- for later use by zone_daily_F								*/
	/*      Accumulate root biomass for patch soil -		*/
	/*      required for N updake from soil                         */
	/*	also determine total plant carbon			*/
	/*	- if grow option is specified				*/
	/*--------------------------------------------------------------*/
	patch[0].effective_lai = 0.0;
	patch[0].soil_cs.frootc = 0.0;
	patch[0].rootzone.depth = 0.0;
	count = 0.0;
	for ( stratum=0 ; stratum<patch[0].num_canopy_strata; stratum++){
		patch[0].effective_lai += patch[0].canopy_strata[stratum][0].epv.proj_lai;
		if (command_line[0].grow_flag > 0) {
			patch[0].soil_cs.frootc
				+= patch[0].canopy_strata[stratum][0].cover_fraction
				* patch[0].canopy_strata[stratum][0].cs.frootc;
			patch[0].preday_totalc
				+= patch[0].canopy_strata[stratum][0].cover_fraction
				* patch[0].canopy_strata[stratum][0].cs.preday_totalc;
			patch[0].preday_totaln
				+= patch[0].canopy_strata[stratum][0].cover_fraction
				* patch[0].canopy_strata[stratum][0].ns.preday_totaln;
				
				
		}
		patch[0].rootzone.depth = max(patch[0].rootzone.depth, 
			 patch[0].canopy_strata[stratum][0].rootzone.depth);
	}
	patch[0].effective_lai = patch[0].effective_lai / patch[0].num_canopy_strata;
	/*--------------------------------------------------------------*/
	/*	re-sort patch layers to account for any changes in 	*/
	/*	height							*/
	/*------------------------------------------------------------------------*/
	sort_patch_layers(patch);


	/*------------------------------------------------------------------------*/
	/*	compute current soil moisture potential					*/
	/*	do this before nitrogen updake occurs later in the day			*/
	/*------------------------------------------------------------------------*/
	patch[0].psi = compute_soil_water_potential(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].theta_psi_curve,
		patch[0].Tsoil,
		-1.0*patch[0].soil_defaults[0][0].psi_max,
		-10.0,
		patch[0].soil_defaults[0][0].psi_air_entry,
		patch[0].soil_defaults[0][0].pore_size_index,
		patch[0].soil_defaults[0][0].p3,
		patch[0].soil_defaults[0][0].p4,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].S);


	if (command_line[0].grow_flag > 0) {

		/*--------------------------------------------------------------*/
		/*	update litter interception capacity			*/
		/*--------------------------------------------------------------*/
		update_litter_interception_capacity(
			patch[0].litter.moist_coef,
			patch[0].litter.density,
			&(patch[0].litter_cs),
			&(patch[0].litter));

	
		if (compute_potential_decomp(
			patch[0].Tsoil,
			patch[0].soil_defaults[0][0].psi_max,
			patch[0].soil_defaults[0][0].psi_air_entry,
			patch[0].rootzone.S,
			patch[0].theta_std,
			&(patch[0].soil_cs),
			&(patch[0].soil_ns),
			&(patch[0].litter_cs),
			&(patch[0].litter_ns),
			&(patch[0].cdf),
			&(patch[0].ndf)
			) != 0){
			fprintf(stderr,"fATAL ERROR: in compute_potential_decomp() ... Exiting\n");
			exit(EXIT_FAILURE);
		}
	}

		/*--------------------------------------------------------------*/
		/*	zeros the accumulative rain_throughfall for 24 hours	*/
		/*--------------------------------------------------------------*/
	patch[0].rain_throughfall_24hours=0.0;
	patch[0].recharge=0;
	patch[0].rz_drainage=0;
	patch[0].unsat_drainage=0;

	return;
}/*end patch_daily_I.c*/
