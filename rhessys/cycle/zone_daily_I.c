/*--------------------------------------------------------------*/
/* 																*/
/*						zone_daily_I								*/
/*																*/
/*	NAME														*/
/*	zone_daily_I												*/
/*			- performs initial cycling and output of a zone		*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void zone_daily_I(											*/
/*                      long    ,								*/
/*                      struct world_object *,					*/
/*                      struct basin_object *, 					*/
/*                      struct hillslope_object *,				*/
/*                      struct zone_object *,					*/
/*                      struct command_line_object *,			*/
/*                      struct tec_entry *,						*/
/*                      struct date);							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on a zone and 		*/
/*	patches in the zone.  The routine also prints out results	*/
/*	where specified by current tec events files.				*/
/*																*/
/*	We assume that all patches owned by this zone share the		*/
/*	same top of canopy fluxes.  Also, we assume that any input	*/
/*	daily fluxes for the zone can apply to the whole patch		*/
/*	and that adjustment of these input fluxes is a function		*/
/* 	of a linear aggergation of patch parameters.				*/
/*																*/
/*	For example, the daytime max temperature is a function of	*/
/*	both base station max and min over 24hrs  & surface slope &	*/
/*	surface LAI.  If this max temp is actually supplied we		*/
/*	will assume that it is appliable to the entire zone EVEN	*/
/*	if there are variations in patch level slope's and LAI's 	*/
/*	A reason for this is that the mixing lengths for maxtemp	*/
/*	is assumed to be at the zone level.  It is , of course,		*/
/*	up to the user to disaggregate zones if this is not the case*/
/*																*/
/*	As a corollary, if the model is being run at a daily time 	*/
/*	step and we need to synthesize day max temp we do so with 	*/
/*	patch level parameters.  In this case we would compute 		*/
/*	zone level flat and sloped radiation and zone mean LAI		*/
/*	and use these in the equations for max temp.  While it is	*/
/*	true that the equation is non-linear we leave it up to the	*/
/*	user to ensure that the variation in the patch level param-	*/
/*	eteres will not result in significant bias when aggregated.	*/
/*																*/
/*	We divide parameters into critical and non-critical.		*/
/*	Critical parameters must be present to avoid a fatal error.	*/
/*	Non-critical parameters may be missing for some or all days.*/
/*	MISSING DATA IS ASSUMED TO BE FLAGGED AS -999.0 FOR FLOAT 	*/
/* 	PARAMETERS AND  9999 FOR INTEGRAL PARAMETERS.				*/
/*																*/
/*	Finally, there are some special cases which we need to make	*/
/*	simplifying assumptions for:								*/
/*																*/
/*	Kdown_direct and Kdown_diffuse supplied for the zone.		*/
/*	 	- these are likely measured as "flat surface" 			*/
/*			irradiance where the sun is above the horizon		*/
/*			since it is probably from a levelled platform.		*/
/*		- it would be tiresome to compute sloped surface		*/
/*			versions of these.  								*/
/*		- given this we will assume it is in fact sloped surface*/
/*			values we are getting (i.e based on slope and 		*/
/*			aspect of zone).									*/
/*		- our problem then is how to adjust Tmax_day without 	*/
/*			computing the RADRATE (which is an hourly thing).	*/
/*			Again, we have to assume that if Kdown_direct and	*/
/*			Kdown_diffuse are good enough for this zone the		*/
/*			Tmax_day is good enough for this zone.  			*/
/*	 	- note that we will have to make use of a base station	*/
/*			LAI to allow modelling of LAI disturbances on 		*/
/*			Tmax_day.											*/
/*																*/
/*	only one of Kdown_direct and Kdown_diffuse					*/
/*		- at the moment I treat this case as neither are given  */
/*			since it would mean									*/
/*			inferring the atmospheric attenuation based on the	*/
/*			supplied variable.  								*/
/*																*/
/*	trans supplied for the zone.								*/
/*		- this refers to daily total atmospheric transmittance	*/
/*		- if it is given and we dont have Kdown's we synthesize */
/*			the Kdowns on an hourly basis using this value		*/
/*		- otherwise we deal with corrections for cloudiness to	*/
/*			the clear sky transmissivity						*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	MISSING DATA IS ASSUMED TO BE FLAGGED AS -999.0 FOR FLOAT 	*/
/* 	PARAMETERS AND  9999 FOR INTEGRAL PARAMETERS.				*/
/*																*/
/*	I have slipped in code to premit missing data from the		*/
/*	first listed base station to be replaced by the next if		*/
/*	more than one base station is attached to the zone.			*/
/*	This has only be done for critical parameters.				*/
/*																*/
/*	This procedure is where any sort of canned interpolation	*/
/*	scheme to treat multiple base stations per zone should be	*/
/*	put.														*/
/*																*/
/*	We assume that all of the sub-zone data structures can		*/
/*	be successfully allocated.  If there is not enough memory	*/
/*	one might want to artificially sub-divide a zone.			*/
/*	We could have forced simulation one patch at a time for the */
/*	whole day however the possibility of lateral fluxes between */
/*	patches on a sub-daily scale means we have to store all		*/
/*	sub-daily patch parameters at one time anyways.				*/
/*																*/
/*																*/
/*	Sept 16 - RIchard Fernandes				*/
/*	atm_trans - the estimators for clear sky atmospheric 	*/
/*		transmissivity with no cloud fractio data is	*/
/*		not good.  We have for now put in code which	*/
/*		always uses the clear sky value - wether or not	*/
/*		either the delta_T or cloud fraction esitmator	*/
/*		is first used.					*/
/*	July 28 - C. Tague					*/
/*	changes isohyet adjustment of precip to a precip lapse  */
/* 		rate with elevation 				*/
/*																*/
/*	Sep 2 1997 RAF												*/
/*	Moss parameters removed										*/
/*																*/
/*	Sep 5 1997 CT												*/
/*	Removed elevation scaling of isohyet						*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

void zone_daily_I(
				  long day,
				  struct	world_object	*world,
				  struct	basin_object	*basin,
				  struct	hillslope_object	*hillslope,
				  struct 	zone_object 	*zone,
				  struct 	command_line_object *command_line,
				  struct	tec_entry		*event,
				  struct 	date 			current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void patch_daily_I(
		struct	world_object	*,
		struct	basin_object	*,
		struct	hillslope_object	*,
		struct 	zone_object 	*,
		struct patch_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);


	double normdist(double, double);

	double unifdist(double, double);

	long  julday(struct date);	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int		flag;
	int		i;
	double	isohyet_adjustment;
	int 	patch;
	double	temp, tmp;
	double	Tlapse_adjustment;
	double	trans_coeff1, trans_coeff2, z_delta, fn_tavg;
	int		season;
	season = 0;
	
	int	inx;
	struct	dated_sequence	clim_event;

	zone[0].rain_hourly_total = 0.0;
	zone[0].snow_hourly_total = 0.0;
	/*--------------------------------------------------------------*/
	/*	Determine the critical daily forcing parameters. 			*/
	/*																*/
	/*	Tair_min, Tair_max, rain, 									*/
	/*																*/
	/*	Step through the list of basestations.						*/
	/*--------------------------------------------------------------*/
	i = 0;
	flag = 0;
	while ( (i < zone[0].num_base_stations) && (flag<3) ){
		/*--------------------------------------------------------------*/
		/*		Tlapse_adjustment	(deg. C)							*/
		/*																*/
		/*		For example eq. 2a, pg. 5 , MTCLIM						*/
		/*		lapse rate amount for this base station to the zone.	*/
		/*--------------------------------------------------------------*/
		/* If netcdf climate data used and no elevation grid provided, assume base station and zone are same z */
        if ((command_line[0].gridded_netcdf_flag == 1) && (world[0].base_station_ncheader[0].elevflag == 0)) {
			z_delta = 0.0;
		}
		else z_delta = zone[0].z - zone[0].base_stations[i][0].z;
		/*--------------------------------------------------------------*/
		/*		compute isohyet difference adjustment					*/
		/*--------------------------------------------------------------*/
		if ( zone[0].base_stations[i][0].daily_clim[0].lapse_rate_precip == NULL) {
		if (zone[0].defaults[0][0].lapse_rate_precip_default != -999.0)
			isohyet_adjustment = zone[0].defaults[0][0].lapse_rate_precip_default*z_delta+1.0;
		else
			isohyet_adjustment = zone[0].precip_lapse_rate;

		}
		else {
			isohyet_adjustment = 
			zone[0].base_stations[i][0].daily_clim[0].lapse_rate_precip[day]*z_delta + 1.0;
			}

		isohyet_adjustment = max(0.0, isohyet_adjustment);
		/*--------------------------------------------------------------*/
		/*		if the base station's value for a parameter is present	*/
		/*			replace it.											*/
		/*																*/
		/*		if no values are there for a parameter over all base	*/
		/*		stations then a fatal error results.					*/
		/*																*/
		/*		we adjust for lapse rate right away         			*/
		/*		we also adjust precip by ratios of isohyet between the	*/
		/*		selected base station and the zone.  					*/
		/*																*/
		/*		we do not adjust for slope, cloudyness or lai as yet	*/
		/*--------------------------------------------------------------*/
		temp = zone[0].base_stations[i][0].daily_clim[0].rain[day];
		/*--------------------------------------------------------------*/
		/* 	allow for stocastic noise in precip scaling 		*/
		/*--------------------------------------------------------------*/
		if ((command_line[0].precip_scale_flag > 0) && (temp > 0.0)) {

			if (temp < zone[0].defaults[0][0].psen[PTHRESH])   {
				if (zone[0].defaults[0][0].psen[PTYPELOW] > 0) 
					isohyet_adjustment = exp(normdist(zone[0].defaults[0][0].psen[P1LOW], zone[0].defaults[0][0].psen[P2LOW]))*z_delta + 1.0;
				else
					isohyet_adjustment = z_delta * unifdist(zone[0].defaults[0][0].psen[P1LOW], zone[0].defaults[0][0].psen[P2LOW]) + 1;
			}
			else {
				if (zone[0].defaults[0][0].psen[PTYPEHIGH] > 0) 
				
					isohyet_adjustment = exp(normdist(zone[0].defaults[0][0].psen[P1HIGH], zone[0].defaults[0][0].psen[P2HIGH]))*z_delta + 1.0;
						
				else
					isohyet_adjustment = z_delta * unifdist(zone[0].defaults[0][0].psen[P1HIGH], zone[0].defaults[0][0].psen[P2HIGH]) + 1;
			}

			if (isohyet_adjustment < ZERO) 
				isohyet_adjustment = 0.0;
		} /* end stocastic noise addition */


		if ( temp != -999.0 ){
			zone[0].rain =  temp * isohyet_adjustment;
			flag++;
		}

		/*--------------------------------------------------------------*/
		/* 	temperature lapse rates with elevation can be daily values or constant (zone defaults) 		*/
		/*--------------------------------------------------------------*/

		
		temp = zone[0].base_stations[i][0].daily_clim[0].tmin[day];
		if (temp != -999.0) {
		if ( zone[0].base_stations[i][0].daily_clim[0].lapse_rate_tmin == NULL) {
			if (zone[0].rain > ZERO)
				Tlapse_adjustment = z_delta * zone[0].defaults[0][0].wet_lapse_rate;
			else
				Tlapse_adjustment = z_delta * zone[0].defaults[0][0].lapse_rate_tmin;
			zone[0].metv.tmin = temp - Tlapse_adjustment;
		}
		else {
			Tlapse_adjustment = z_delta * 
				zone[0].base_stations[i][0].daily_clim[0].lapse_rate_tmin[day];
			zone[0].metv.tmin = temp - Tlapse_adjustment;
		}
			flag++;
		}
			
	
		temp = zone[0].base_stations[i][0].daily_clim[0].tmax[day];

		if (temp != -999.0) {
		if ( zone[0].base_stations[i][0].daily_clim[0].lapse_rate_tmax == NULL) {
			if (zone[0].rain > ZERO)
				Tlapse_adjustment = z_delta * zone[0].defaults[0][0].wet_lapse_rate;
			else
				Tlapse_adjustment = z_delta * zone[0].defaults[0][0].lapse_rate_tmax;
			zone[0].metv.tmax = temp - Tlapse_adjustment;
			flag++;
		}
		else {
			Tlapse_adjustment = z_delta * 
				zone[0].base_stations[i][0].daily_clim[0].lapse_rate_tmax[day];
			zone[0].metv.tmax = temp - Tlapse_adjustment;
			flag++;
		}
		}

		if (command_line[0].tchange_flag > 0)  {
			zone[0].metv.tmax += command_line[0].tmax_add;
			zone[0].metv.tmin += command_line[0].tmin_add;
			}
        i++;                                                                     /*160419LML*/
	} /*end while*/
	/*--------------------------------------------------------------*/
	/*	Check if we filled in all of the critical parameters.		*/
	/*--------------------------------------------------------------*/
	if ( flag != 3 ){
		fprintf(stderr, "\n For zone %d", zone[0].ID);
		fprintf(stderr,
			"\nFATAL ERROR: In zone_daily_I - missing critical data for %ld %ld %ld\n",
			current_date.year, current_date.month, current_date.day);
		exit(EXIT_FAILURE);
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Repeat the same exercise for the non-critical parameters.	*/
	/*																*/
	/*	Non-critical parameters are a luxury.  If present the may	*/
	/*	avoid us from doing synthesis of their values (at possibly	*/
	/*	an hourly time step to avoid non-linearities).  So, we		*/
	/*	expend a bit of overhead to see if non-critical parameters	*/
	/*	are present.  												*/
	/*																*/
	/*	Since it is													*/
	/*	likely that either we will or will not have a given non-	*/
	/*	critical parameter for the base stations assigned to the	*/
	/*	zone I propose that we ASSUME that if the first base station*/
	/*	does not have the climate sequence for the non-critical		*/
	/*	parameters it is not worth checking the rest of the base	*/
	/*	stations.  													*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	initialize all non-critical state variables for this zone.	*/
	/*																*/
	/*	Note: Some of the parameters (such as Tair_ are adjusted */
	/*--------------------------------------------------------------*/
	zone[0].atm_trans = -999.0;
	zone[0].CO2 = -999.0;
	zone[0].cloud_fraction = -999.0;
	zone[0].cloud_opacity = -999.0;
	zone[0].Delta_T = -999.0;
	zone[0].Kdown_diffuse = -999.0;
	zone[0].Kdown_diffuse_calc = 0.0;
	zone[0].Kdown_diffuse_flat_calc = 0.0;
	zone[0].Kdown_diffuse_adjustment = -999.0;
	zone[0].Kdown_diffuse_flag = 0;
	zone[0].Kdown_direct = -999.0;
	zone[0].Kdown_direct_calc = 0.0;
	zone[0].Kdown_direct_flat_calc = 0.0;
	zone[0].Kdown_direct_adjustment = -999.0;
	zone[0].Kdown_direct_flag = 0;
	zone[0].Ldown = -999.0;
	zone->Ldown_night = -999.0;
	zone->Ldown_day = -999.0;
	zone[0].PAR_direct = -999.0;
	zone[0].PAR_diffuse = -999.0;
	zone[0].rain_duration = -999.0;
	zone[0].snow = -999.0;
	zone[0].relative_humidity = -999.0;
	zone[0].tdewpoint = -999.0;
	zone[0].metv.tavg = -999.0;
	zone[0].metv.tday = -999.0;
	zone[0].metv.tnight = -999.0;
	zone[0].metv.tsoil = -999.0;
	zone[0].metv.vpd = -999.0;
	zone[0].metv.vpd_day = -999.0;
	zone[0].metv.vpd_night = -999.0;
	zone[0].ndep_NO3 = -999.0;
	zone[0].ndep_NH4 = -999.0;
	zone[0].wind = -999.0;
	zone[0].wind_direction = -999.0;
	zone[0].LAI_scalar = -999.0;
	
	/* Re-use z_delta */
    /*160419LML seems there is an asumption that each zone has one station.*/
    if ((command_line[0].gridded_netcdf_flag == 1) && (world[0].base_station_ncheader[0].elevflag == 0)) {
		z_delta = 0.0;
	}
	else z_delta = zone[0].z - zone[0].base_stations[0][0].z;	
	
	/*--------------------------------------------------------------*/
	/*	Check if the first base station has the sequence.			*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	snow														*/
	/*																*/
	/*	If snow is not available we estimate it at the end of the 	*/
	/*	day based on end of day temperatures						*/
	/*--------------------------------------------------------------*/
	
	
	if ( zone[0].base_stations[0][0].daily_clim[0].snow != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].snow[day];
		if ( temp != -999.0 ){
			zone[0].snow = temp * isohyet_adjustment;
		}

	}

	/*--------------------------------------------------------------*/
	/*	daytime rain duration - (seconds)			*/
	/*								*/
	/*	This is the daytime time  over which rain fell.		*/
	/*	It is used to adjust the vpd to  the saturation 	*/
	/*	vpd during the rain hours and recompute the vpd		*/
	/*	for the rest of the day by adjusting the mean value	*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].daytime_rain_duration!=NULL){
		temp=zone[0].base_stations[0][0].daily_clim[0].daytime_rain_duration[day];
		if ( temp != -999.0 ){
			zone[0].rain_duration = temp * 3600;
		}
	}
	inx = zone[0].base_stations[0][0].hourly_clim[0].rain.inx;
	if (inx==0){inx=-1;}
	if(inx>-999){
		clim_event=zone[0].base_stations[0][0].hourly_clim[0].rain.seq[inx+1];
		if ((clim_event.edate.year!=0)&&(julday(clim_event.edate)==julday(current_date))){
			zone[0].rain_duration = 0;
		}
	}


	/*--------------------------------------------------------------*/
	/*	Read in the base station effective LAI for this date.		*/
	/*	We need this since the LAI compensdation for temp assumes	*/
	/*	that we consider the difference in LAI between base station	*/
	/*	and site.  Where base stations are well above any canopies  */
	/*	we might assume the effective LAI == 0.						*/
	/*																*/
	/*	If not present we make use of a fixed base_station_effective*/
	/*	lai provided with each base station object.					*/
	/*--------------------------------------------------------------*/
	if(zone[0].base_stations[0][0].daily_clim[0].base_station_effective_lai
		!= NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].base_station_effective_lai[day];
		if ( temp != -999.0 ){
			zone[0].base_station_effective_lai = temp;
		}
	}
	else{
		zone[0].base_station_effective_lai =
			zone[0].base_stations[0][0].effective_lai;
	}
	/*--------------------------------------------------------------*/
	/*	cloud fraction												*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].cloud_fraction != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].cloud_fraction[day];
		if ( temp != -999.0 ) zone[0].cloud_fraction = temp;
	}
	/*--------------------------------------------------------------*/
	/*	cloud opacity												*/
	/*																*/
	/*	Defined as the upper hemispherical albedo + bulk 			*/
	/*	absorptivity of the clouds.									*/
	/*																*/
	/*	Opacity defaults as 0.8 for clouds.							*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].cloud_opacity != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].cloud_opacity[day];
		if ( temp != -999.0 ) zone[0].cloud_opacity = temp;
	}
	else{
		zone[0].cloud_opacity = 0.8;
	}	
	/*--------------------------------------------------------------*/
	/*	Delta_T														*/
	/*																*/
	/*	Replaces the MTCLIM logic for computing adjustment to		*/
	/*	clear sky atmospheric transmissivity based on synoptic		*/
	/*  air temperatures and percip as given in Appendix D, EQ. 3	*/
	/*	of "MTCLIM" by a pre-computed Delta_T.						*/
	/*																*/
	/*	If delta T is not given we approximate it by synoptic		*/
	/*	temp and precip for the current day alone.  This is possibly*/
	/*	sub-optimal however it smooths the coding.  In addition,	*/
	/*	the use of a cloud fraction input overrides the use of		*/
	/*	Delta_T in computing bulk atmospheric transmissivity.		*/
	/*																*/
	/*	Note :  we also use Delta_T to adjust Ldown if no cloud		*/
	/*			fraction data is present.							*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].Delta_T != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].Delta_T[day];
		if ( temp != -999.0 ){
			zone[0].Delta_T = temp;
		}
		else{
			zone[0].Delta_T = zone[0].metv.tmax - zone[0].metv.tmin;
			if ( (zone[0].rain + max(zone[0].snow,0.0)) > zone[0].defaults[0][0].pptmin){
				zone[0].Delta_T =	zone[0].Delta_T * 0.75;
			}
		} /*end if else*/
		
				
	}
	else{
		zone[0].Delta_T = zone[0].metv.tmax - zone[0].metv.tmin;
		/*--------------------------------------------------------------*/
		/*		We only make rainy day tranmissivity corrections if		*/
		/*		there is a substantial amoun of rain (>1inch)			*/
		/*		From C code for rhessys.								*/
		/*--------------------------------------------------------------*/
		if ( (zone[0].rain + max(0.0,zone[0].snow)) > zone[0].defaults[0][0].pptmin){
			zone[0].Delta_T =	zone[0].Delta_T * 0.75;
		}
	}
	/*--------------------------------------------------------------*/
	/* Correct data input errors such that min T is greater than max T */
	/* but warn user of problem with their climate inputs		*/
	/*--------------------------------------------------------------*/
	if (zone[0].Delta_T < -ZERO) {
		printf("\n WARNING: Maximum temperature is less than minimum temperature on %d %d %d", 
			current_date.day, current_date.month, current_date.year); 
		zone[0].Delta_T = zone[0].Delta_T * -1.0;
	}
	/*--------------------------------------------------------------*/
	/*	Kdown_direct		(kJ/(m2*day))						*/
	/*																*/
	/*	Assumed to be applicable to this zone's slope and aspect!!	*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].Kdown_direct != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].Kdown_direct[day];
		if ( temp != -999.0 ){
			zone[0].Kdown_direct = temp;
			zone[0].Kdown_direct_flag = 1;
		}
	}
	/*--------------------------------------------------------------*/
	/*	Kdown_diffuse	(kJ/(m2*day))							*/
	/*																*/
	/*	Assumed to be applicable to this zone's slope and aspect!!	*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].Kdown_diffuse != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].Kdown_diffuse[day];
		if ( temp != -999.0 ){
			zone[0].Kdown_diffuse = temp;
			zone[0].Kdown_diffuse_flag = 1;
		}
	}
	/*--------------------------------------------------------------*/
	/*	PAR_diffuse			(umol/(m2*day))							*/
	/*																*/
	/*	Assumed to be applicable to this zone's slope and aspect!!	*/
	/*	If not available it will be computed as a fixed fraction	*/
	/*	of Kdown_diffuse which is suspect if the Kdown_diffuse is	*/
	/*	synthesized since the synthesis uses atmospheric transmiss	*/
	/*	ivity which may not be the same for Kdown and PAR.			*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].PAR_diffuse != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].PAR_diffuse[day];
		if ( temp != -999.0 ) zone[0].PAR_diffuse = temp * 1000000;
	}
	/*--------------------------------------------------------------*/
	/*	PAR_direct			(umol/(m2*day))??							*/
	/*																*/
	/*	Assumed to be applicable to this zone's slope and aspect!!	*/
	/*	If not available it will be computed as a fixed fraction	*/
	/*	of Kdown_direct which is suspect if the Kdown_direct is		*/
	/*	synthesized since the synthesis uses atmospheric transmiss	*/
	/*	ivity which may not be the same for Kdown and PAR.			*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].PAR_direct != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].PAR_direct[day];
		if ( temp != -999.0 ) zone[0].PAR_direct = temp * 1000000;
	}
	/*--------------------------------------------------------------*/
	/*	bulk atmospheric transmissivity	(includes clouds)			*/
	/*	assumed for So and Do (I dont think this is valid for PAR)	*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].atm_trans != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].atm_trans[day];
		if ( temp != -999.0 ) zone[0].atm_trans = temp;
	}
	/*--------------------------------------------------------------*/
	/*	tsoil		(deg C)												*/
	/*																*/
	/* not sure what depth is 		*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].tsoil != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].tsoil[day];
		if ( temp != -999.0 ) zone[0].metv.tsoil = temp;
	}
	/*--------------------------------------------------------------*/
	/* if it is the first day of the simulation running average of the tsoil */
	/* will not yet be set so initialize to tavg */
	/*--------------------------------------------------------------*/
	 if (zone[0].metv.tsoil_sum < -998.0)
		zone[0].metv.tsoil_sum = zone[0].metv.tavg;
	/*--------------------------------------------------------------*/
	/*	Wind speed at screen height.								*/
	/*																*/
	/*	It is up to the user to ensure that the windspeed makes		*/
	/*	sense with whatever surface resistance formulation is		*/
	/*	used for fluxes.  I have ensured that the formulations		*/
	/*	make sense for a windspeed 2m above the vegetated canopy.	*/
	/*																*/
	/*	Note that for wind speed we should assume it is applicable	*/
	/*	for a canopy of the mean patch LAI for this zone so, just	*/
	/*	as albedo at patch level can modify APAR; LAI at patch level*/
	/*	can modify wind speed.										*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].wind != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].wind[day];
		if ( temp != -999.0 ){
			zone[0].wind = temp;
		}
		else{
			zone[0].wind = zone[0].defaults[0][0].wind;
		}
	}
	else{
		zone[0].wind = zone[0].defaults[0][0].wind;
	}


	/*--------------------------------------------------------------*/
	/*	Wind direction at screen height.								*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].wind_direction != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].wind_direction[day];
		if ( temp != -999.0 ){
			zone[0].wind_direction = temp;
		}
		else{
			zone[0].wind_direction = zone[0].defaults[0][0].wind_direction;
		}
	}
	else{
		zone[0].wind_direction = zone[0].defaults[0][0].wind_direction;
	}


	/*--------------------------------------------------------------*/
	/*	Daily Nitrogen deposition				*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].ndep_NO3 != NULL )
		{
		temp = zone[0].base_stations[0][0].daily_clim[0].ndep_NO3[day];
		if ( temp != -999.0 ) zone[0].ndep_NO3 = temp;
	}
		else zone[0].ndep_NO3 = zone[0].defaults[0][0].ndep_NO3;
	if ( zone[0].base_stations[0][0].daily_clim[0].ndep_NH4 != NULL )
		{
		temp = zone[0].base_stations[0][0].daily_clim[0].ndep_NH4[day];
		if ( temp != -999.0 ) zone[0].ndep_NH4 = temp;
	}
		else zone[0].ndep_NH4 = 0.0;
	/*--------------------------------------------------------------*/
	/*	CO2 -ppm - atmospheric CO2  concentration time series	*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].CO2 != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].CO2[day];
		if ( temp != -999.0 ) zone[0].CO2 = temp;
	}
	/*--------------------------------------------------------------*/
	/*      vpd - Pa - daylight mean value.                         */
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].vpd != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].vpd[day];
		if ( temp != -999.0 ) zone[0].metv.vpd = temp;
	}
	/*--------------------------------------------------------------*/
	/*	relative humidity											*/
	/*																*/
	/*	Assumed to apply as daylight mean value.					*/
	/*	Used in place of dewpoint temperature to compute vpd		*/
	/*		if it is available.										*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].relative_humidity != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].relative_humidity[day];
		if ( temp != -999.0 ) zone[0].relative_humidity= temp;
	}
	/*--------------------------------------------------------------*/
	/*	Dewpoint temperature										*/
	/*																*/
	/*	I have moved the default dewpoint from the base station to	*/
	/*	the zone default since it applies to zones.  In the event	*/
	/*	that dewpoints recorded at a base_station is available we	*/
	/*	adjust for differences in elevations as per Appendix G 	of	*/
	/*	"MTNCLIM"; otherwise we use Tmin_air for dewpoint.			*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].tdewpoint != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].tdewpoint[day];
		if ( temp != -999.0 ){
			zone[0].tdewpoint = temp-( z_delta )
				* zone[0].defaults[0][0].dewpoint_lapse_rate;
		}
		else{
			zone[0].tdewpoint = zone[0].metv.tmin;
		}
	} /*end if else*/
	else{
		zone[0].tdewpoint = zone[0].metv.tmin;
	}
	/*--------------------------------------------------------------*/
	/*	tavg							*/
	/*																*/
	/*	Mean recorded temperature over the entire 24 hrs OR			*/
	/*	Arithmetic mean of daily tmax and tmin.				*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].tavg != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].tavg[day];
		if ( temp != -999.0 ){
			zone[0].metv.tavg = temp-( z_delta )
				* zone[0].defaults[0][0].lapse_rate;

			if (command_line[0].tchange_flag > 0)  {
				zone[0].metv.tavg += (command_line[0].tmax_add +  command_line[0].tmin_add)/2.0;
			}
		}
		else{
			zone[0].metv.tavg = (zone[0].metv.tmin + zone[0].metv.tmax) /2.0 ;
		}
	} /*end if else */
	else{
		zone[0].metv.tavg = (zone[0].metv.tmin + zone[0].metv.tmax) /2.0 ;
	}
	/*--------------------------------------------------------------*/
	/*      LAI_scalar                                              */
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].LAI_scalar != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].LAI_scalar[day];
		if ( temp != -999.0 ){
			zone[0].LAI_scalar = temp;
		}
	} /*end if*/
	else{
		zone[0].LAI_scalar = 1.0;
	}
	/*--------------------------------------------------------------*/
	/*	Ldown						*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].Ldown != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].Ldown[day];
		if ( temp != -999.0 ){
			zone[0].Ldown = temp;
		}
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	daylength	(sec)					*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].dayl != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].dayl[day];
		if ( temp != -999.0 ){
			zone[0].metv.dayl = temp;
			zone[0].daylength_flag = 1;
		}
		else{
			zone[0].metv.dayl = 0.0;
			zone[0].daylength_flag = 0;
		}
	}
	else{
		zone[0].metv.dayl = 0.0;
		zone[0].daylength_flag = 0;
	}
	/*--------------------------------------------------------------*/
	/*	tday							*/
	/*																*/
	/*	Mean recorded temperature for daylight hours and adjust for	*/
	/*	lapse rate OR												*/
	/*																*/
	/*	wait until the surface radiation terms are computed so we	*/
	/*	can use adjusted temperatures with							*/
	/*	via Equation 1 and Appendix B "MTCLIM"; in this case 		*/
	/*	note that all temperatures are already adjusted for lapse	*/
	/*	rate.														*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].tday != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].tday[day];
		if ( temp != -999.0 ){
			temp = temp - ( z_delta )
				* zone[0].defaults[0][0].lapse_rate;
			zone[0].metv.tday = temp;
		}
	} /*end if */
	/*--------------------------------------------------------------*/
	/*	tnight							*/
	/*																*/
	/*	Mean recorded temperature for hours without daylight 		*/
	/*	which we adjust for lapse rate OR							*/
	/*																*/
	/*	wait if tday is read in compute based on it and	*/
	/*	tmin using method given in zcomp.c of C Rhessys code	*/
	/*	OR															*/
	/*																*/
	/*	if tday is not given we wait until it is computed	*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].tnight != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].tnight[day];
		if ( temp != -999.0 ){
			temp = temp - ( z_delta ) *
				zone[0].defaults[0][0].lapse_rate;
			zone[0].metv.tnight = temp;
		}
		else if ( zone[0].metv.tday != -999.0 ){
			zone[0].metv.tnight  = (zone[0].metv.tday + zone[0].metv.tmin)/2.0;
		}
		else{
			zone[0].metv.tnight = -999.0;
		}
	} /*end if*/
	else if( zone[0].metv.tday != -999.0 ){
		zone[0].metv.tnight  = (zone[0].metv.tday + zone[0].metv.tmin)/2.0;
	}
	else{
		zone[0].metv.tnight = -999.0;
	}
	/*--------------------------------------------------------------*/
	/*	Tnight max - nighttime temperature at sundown used for	*/
	/*	heat flux computation - if not present we use the	*/
	/*	minimum nighttime temp and essential turn off heat 	*/
	/*	flux.							*/
	/*--------------------------------------------------------------*/
	if ( zone[0].base_stations[0][0].daily_clim[0].tnightmax != NULL ){
		temp = zone[0].base_stations[0][0].daily_clim[0].tnightmax[day];
		if ( temp != -999.0 ){
			temp = temp - ( z_delta )
				* zone[0].defaults[0][0].lapse_rate;
			zone[0].metv.tnightmax = temp;
		}
		else{
			zone[0].metv.tnightmax = zone[0].metv.tmin;
		}
	}
	else{
		zone[0].metv.tnightmax = zone[0].metv.tmin;
	}
	/*--------------------------------------------------------------*/
	/*	Only do this stuff if we dont have Kdown_direct or      	*/
	/*	Kdown_diffuse and the daily bulk transmissivity				*/
	/*--------------------------------------------------------------*/
	if  ((zone[0].Kdown_direct_flag == 0) ||
		(zone[0].Kdown_diffuse_flag == 0)){
		/*--------------------------------------------------------------*/
		/*		First zero out Kdown_direct and Kdown_diffuse just in 	*/
		/*		case oneof the two were provided (we want all or none)	*/
		/*--------------------------------------------------------------*/
		zone[0].Kdown_direct = 0.0;
		zone[0].Kdown_diffuse = 0.0;
		zone[0].Kdown_direct_flat = 0.0;
		zone[0].Kdown_diffuse_flat = 0.0;
		if ( zone[0].atm_trans == -999.0 ){
			/*--------------------------------------------------------------*/
			/*			compute the clear sky transmissivity.					*/
			/*																*/
			/*			Appendix D Eq. 1 "MTCLIM"								*/
			/*			Bristow and Campbell 1984.								*/
			/*--------------------------------------------------------------*/
			zone[0].atm_trans =
				zone[0].defaults[0][0].sea_level_clear_sky_trans
				+ zone[0].z * zone[0].defaults[0][0].atm_trans_lapse_rate;
				
			/*--------------------------------------------------------------*/
			/*			convert clear sky transmissivity to bulk transmissivity	*/
			/*																*/
			/*			We use two methods:										*/
			/*--------------------------------------------------------------*/
			/*--------------------------------------------------------------*/
			/*			Method 1 : no cloud fraction data						*/
			/*																*/
			/*			We use Eq. 2 of Appendix D of "MTCLIM: which in turn	*/
			/*			is based on "Bristow and Campbell(1984)".     			*/
			/*																*/
			/*			Note that zone[0].defaults[0].trans_coeff1 varies with	*/
			/*			seasons in "Bristow and Campbell(1984)" while we have	*/
			/*			it as a default variable for each zone.  We might be 	*/
			/*			able to change the value by a tec event.  				*/
			/*--------------------------------------------------------------*/
						
			if ( zone[0].cloud_fraction == -999.0 ){

		/*----------------------------------------------------------------------*/
		/*  Bristow and Campbell (1984). Override summer vs. winter parameters	*/
		/*  if provided. Override trans_coeff1 if -999.0 and make it a			*/
		/*	function of delta T.												*/
		/*----------------------------------------------------------------------*/
				
				/* Determine what season we are in: summer (1) or winter (2) or non-seasonal (0).				*/
				/* Flexibility to deal with northern or southern hemisphere seasons.		*/
				if (zone[0].defaults[0][0].trans_startmonth_sum < zone[0].defaults[0][0].trans_startmonth_win) {
					if ( (current_date.month >= zone[0].defaults[0][0].trans_startmonth_sum) && (current_date.month < zone[0].defaults[0][0].trans_startmonth_win) ) {
						season = 1; /* summer */
					}
					else season = 2; /* winter */
				}
				else {
					if (zone[0].defaults[0][0].trans_startmonth_sum > zone[0].defaults[0][0].trans_startmonth_win) {
						if ( (current_date.month >= zone[0].defaults[0][0].trans_startmonth_win) && (current_date.month < zone[0].defaults[0][0].trans_startmonth_sum) ) {
							season = 2; /* winter */
							}
						else season = 1; /* summer */
					} /* else summer start month and winter start month are same so we default back to non-seasonal 0 */
				}
				
				/* If it is summer and summer params provided, use them. */
				if ( (season == 1) && (zone[0].defaults[0][0].trans_coeff1_sum != -999.0) && (zone[0].defaults[0][0].trans_coeff2_sum != -999.0) ) {
						trans_coeff1 = zone[0].defaults[0][0].trans_coeff1_sum;
						trans_coeff2 = zone[0].defaults[0][0].trans_coeff2_sum;
					} /* end summer if */
				else {
					/* If it is winter and winter params provided, use them. */
					if ( (season == 2) && (zone[0].defaults[0][0].trans_coeff1_win != -999.0) && (zone[0].defaults[0][0].trans_coeff2_win != -999.0)) {
						trans_coeff1 = zone[0].defaults[0][0].trans_coeff1_win;
						trans_coeff2 = zone[0].defaults[0][0].trans_coeff2_win;
						} /* end winter if */
					/* Otherwise base Bristow and Campbell model (season doesn't matter). */
					else {
						if ( zone[0].defaults[0][0].trans_coeff1 == -999.0 )
							trans_coeff1 = 0.036 * exp(-0.154 * zone[0].Delta_T);
						else 
							trans_coeff1 = zone[0].defaults[0][0].trans_coeff1;
						trans_coeff2 = zone[0].defaults[0][0].trans_coeff2;
					}
				}
				
				/* Bristow & Campbell Eqn */
				zone[0].atm_trans = zone[0].atm_trans
								* ( 1.0 - exp( -1 * trans_coeff1
								* pow(zone[0].Delta_T,trans_coeff2)));
								
				/*----------------------------------------------------------------------*/
				/* Other Parameterizations for Bristow & Campbell */
				/*trans_coeff1 = 2269.3 * exp(-1.54 * zone[0].Delta_T);  parameterized for Niwot Ridge */
				/*trans_coeff1 = 403.3 * exp(-1.33 * zone[0].Delta_T);  parameterized for Niwot Ridge */
				/*trans_coeff1 = zone[0].defaults[0][0].trans_coeff1;  using def file param */
				
				/* Bristow & Campbell PARAMS NIW 8M */
				/*if (current_date.month>=6 && current_date.month<=10) {
					trans_coeff1 = 0.075;
				    trans_coeff2 = 1.06;
					}
				else {
					trans_coeff1 = 0.50;
					trans_coeff2 = 0.49;
					}
				 
				  Bristow & Campbell SEAS PARAMS NIW 21m w/ 1mm precip adjustment */
				/*if (current_date.month>=6 && current_date.month<=10) {
					trans_coeff1 = 0.152;
					trans_coeff2 = 1.046;
					}
				 else {
					 trans_coeff1 = 0.380;
					 trans_coeff2 = 0.685;
					}*/

				/* Bristow & Campbell ANNUAL PARAMS NIW 21m w/ 1mm precip adjustment */
				/*trans_coeff1 = 0.317;
				trans_coeff2 = 0.731;*/
				
				/*----------------------------------------------------------------------*/
				/* ALTERNATIVE FORMULATION: Donatelli & Campbell Eqn */
				/*trans_coeff1 = 0.70;
				 trans_coeff2 = 50;
				 fn_tavg = 0.017 * exp(exp(-0.053*zone[0].metv.tavg));
				 zone[0].atm_trans = zone[0].atm_trans
				 * ( 1.0 - exp( -1 * trans_coeff1 * fn_tavg
				 * pow(zone[0].Delta_T,2) * exp(zone[0].metv.tmin/trans_coeff2)));*/
				/*----------------------------------------------------------------------*/
				
				
				zone[0].Kdown_direct_adjustment = 1.0;
				zone[0].Kdown_diffuse_adjustment = 1.0;
				zone[0].cloud_fraction = 1.0 - zone[0].atm_trans
							/(zone[0].defaults[0][0].sea_level_clear_sky_trans
							+ zone[0].z * zone[0].defaults[0][0].atm_trans_lapse_rate);
				zone[0].cloud_fraction = max(zone[0].cloud_fraction,0.0);
				zone[0].cloud_fraction = min(zone[0].cloud_fraction,1.0);
				zone[0].cloud = zone[0].cloud_opacity * zone[0].cloud_fraction * 12.0;
			}
			else{
				/*--------------------------------------------------------------*/
				/*			Method 2:  cloud fraction data							*/
				/*																*/
				/*			Here we ignore the Delta_T adjustment method and assume*/
				/*			clouds have a uniform transmissivity (or opacity) and	*/
				/*			we ignore multiple scattering between surface and clouds*/
				/*			at this stage.  Multiple scattering may make a          */
				/*			difference for low clouds in winter.					*/
				/*																*/
				/*-------------------------------------------------------------*/
				zone[0].Kdown_direct_adjustment = 1.0 - (zone[0].cloud_fraction * zone[0].cloud_opacity);
				zone[0].Kdown_diffuse_adjustment = 1.0 - zone[0].cloud_fraction;
				zone[0].cloud = zone[0].cloud_opacity * zone[0].cloud_fraction * 12.0;
				/* Cloud fraction given but not trasmissivity so must back-calculate */
				zone[0].atm_trans = (1.0 - zone[0].cloud_fraction)
										* (zone[0].defaults[0][0].sea_level_clear_sky_trans
										   + zone[0].z * zone[0].defaults[0][0].atm_trans_lapse_rate);
			} /*end if*/			
		} /*end if*/
		else {
			/* Case where transmissivity is given so use it to calculate cloud fraction */
			zone[0].Kdown_direct_adjustment = 1.0;
			zone[0].Kdown_diffuse_adjustment = 1.0;
			zone[0].cloud_fraction = 1.0 - zone[0].atm_trans
										/(zone[0].defaults[0][0].sea_level_clear_sky_trans
										+ zone[0].z * zone[0].defaults[0][0].atm_trans_lapse_rate);
			zone[0].cloud_fraction = max(zone[0].cloud_fraction,0.0);
			zone[0].cloud_fraction = min(zone[0].cloud_fraction,1.0);
			zone[0].cloud = zone[0].cloud_opacity * zone[0].cloud_fraction * 12.0;
		}
		
	} /*end if*/
	
	/* CASE WHERE KDOWNS ARE GIVEN:													*/
	/* If Kdowns are given, set transmissivity to 1 to calculate "no atmosphere"	*/
	/* Kdowns. Then use ratio between observed and "no atmosphere" to calculate		*/
	/* actual transmissivity (and estimate cloud fraction) in zone_daily_F.			*/
	else {
		zone[0].Kdown_direct_adjustment = 1.0;
		zone[0].Kdown_diffuse_adjustment = 1.0;
		zone[0].atm_trans = 1.0;
	}


	
	if (command_line[0].verbose_flag == -5) {
		printf("\n%ld %ld %ld -335.1 ",
			   current_date.year, current_date.month, current_date.day);
		printf("\nZONE_I: precip=%lf cloudfrac=%lf Kdir_adj=%lf Kdif_adj=%lf trans=%lf tcoeff1=%lf tcoeff2=%lf", 
			   zone[0].snow + zone[0].rain, 
			   zone[0].cloud_fraction, 
			   zone[0].Kdown_direct_adjustment/86.4,
			   zone[0].Kdown_diffuse_adjustment/86.4, 
			   zone[0].atm_trans,
			   trans_coeff1,
			   trans_coeff2);
	}
	
	/*--------------------------------------------------------------*/
	/*	Cycle through the patches 									*/
	/*--------------------------------------------------------------*/
	for ( patch=0 ; patch<zone[0].num_patches; patch++ ){
		patch_daily_I(
			world,
			basin,
			hillslope,
			zone,
			zone[0].patches[patch],
			command_line,
			event,
			current_date );
	}

} /*end zone_daily_I.c*/
