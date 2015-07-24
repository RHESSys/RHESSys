/*--------------------------------------------------------------*/
/* 																*/
/*						zone_daily								*/
/*																*/
/*	NAME														*/
/*	zone_daily_F												*/
/*	 - performs final daily  cycling and output of a zone		*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void zone_daily_F(											*/
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
/*	Sep 2 1997 RAF												*/
/*	moss parameters removed										*/
/*								*/
/* 	Sept 11 1997 CT						*/
/*	added parameters for snow/rain transition		*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"


void		zone_daily_F(
						 long	day,
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
	void    patch_daily_F(
		struct	world_object	*,
		struct	basin_object	*,
		struct	hillslope_object	*,
		struct 	zone_object 	*,
		struct patch_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	long julday(struct date);
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int 	patch;
	double snow_rain_range;
	double	es;
	double Tcloud, f8, e8z, tau8;
	/*--------------------------------------------------------------*/
	/*  Update the forcing functions based on the hourly computation*/
	/*--------------------------------------------------------------*/
	/* Set daily Kdowns to calculated if not given as inputs.		*/
	/* Transmissivity & cloud fractions carry over from zone_daily_I.	*/
	if ((zone[0].Kdown_direct_flag == 0) ||
		(zone[0].Kdown_diffuse_flag == 0)) {
			zone[0].Kdown_direct_flat = zone[0].Kdown_direct_flat_calc;
			zone[0].Kdown_direct = zone[0].Kdown_direct_calc;
			zone[0].Kdown_diffuse_flat = zone[0].Kdown_diffuse_flat_calc;
			zone[0].Kdown_diffuse = zone[0].Kdown_diffuse_calc;
		}
	/* Otherwise use input Kdowns and calculate transmissivity as	*/
	/* ratio between given and calculated, then generate cloud		*/
	/* fraction estimates for longwave calculations. */ 
	else {
			zone[0].atm_trans = (zone[0].Kdown_direct+zone[0].Kdown_diffuse) 
						/ (zone[0].Kdown_direct_calc + zone[0].Kdown_diffuse_calc);
			zone[0].cloud_fraction = 1.0 - zone[0].atm_trans
						/(zone[0].defaults[0][0].sea_level_clear_sky_trans
						+ zone[0].z * zone[0].defaults[0][0].atm_trans_lapse_rate);
			zone[0].cloud_fraction = max(zone[0].cloud_fraction,0.0);
			zone[0].cloud_fraction = min(zone[0].cloud_fraction,1.0);
			zone[0].cloud = zone[0].cloud_opacity * zone[0].cloud_fraction * 12.0;
		}
	
	
	/*--------------------------------------------------------------*/
	/*	Make use of more accurate basin daylength 		*/
	/*	if it is not given and the zone horizons are 0		*/
	/*	If the zone horizons are non zero we use the 		*/
	/*	daylength computed from hourly solar geometry.		*/
	/*--------------------------------------------------------------*/
	if ( (zone[0].daylength_flag == 0) &&
		(zone[0].e_horizon == 0) &&
		(zone[0].w_horizon == 0) ){
		/*--------------------------------------------------------------*/
		/*		adjust the simulated radiation for new dayl	*/
		/*--------------------------------------------------------------*/

		if (zone[0].Kdown_direct_flag == 0){
			zone[0].Kdown_direct = zone[0].Kdown_direct
				* ( basin[0].daylength / zone[0].metv.dayl );
		}
		if (zone[0].Kdown_diffuse_flag == 0){
			zone[0].Kdown_diffuse = zone[0].Kdown_diffuse
				* ( basin[0].daylength / zone[0].metv.dayl );
		}
		/*--------------------------------------------------------------*/
		/*		update the zone daylength to be the basin dayl	*/
		/*		which is likely more accurate.			*/
		/*--------------------------------------------------------------*/
		zone[0].metv.dayl = basin[0].daylength;
	}

	/*--------------------------------------------------------------*/
	/* MOVED TEMP AND PRECIP CALCS UP SO CAN BE USED FOR CLOUD FRAC */
	/*--------------------------------------------------------------*/
	/*  metv.tavg calc in zone_daily_I								*/
	/*--------------------------------------------------------------*/
	/*	metv.tday	(degrees C)										*/
	/*																*/
	/*	Eq 1. Page 4, "MTCLIM"										*/
	/*--------------------------------------------------------------*/
	if ( zone[0].metv.tday == -999.0 ){
		zone[0].metv.tday = zone[0].defaults[0][0].temcf
		* (zone[0].metv.tmax - zone[0].metv.tavg) + zone[0].metv.tavg;
	}
	/*--------------------------------------------------------------*/
	/*	metv.tnight 	(degrees C)								*/
	/*																*/
	/*	zcomp.c C Rhessys code										*/
	/*--------------------------------------------------------------*/
	if ( zone[0].metv.tnight == -999.0 ){
		zone[0].metv.tnight = (zone[0].metv.tday + zone[0].metv.tmin)/2.0;
	}
	/*--------------------------------------------------------------*/
	/*	If we have no snow data determine if the rain should be snow*/
	/*																*/
	/*	Based on U.S Army Corps of ENgineers (1956) Snow Hydrology	*/
	/*	use a min/max to get range in which there is a mix of snow/rain */
	/*								*/
	/*--------------------------------------------------------------*/
	if (zone[0].snow == -999.0 ){
		if (zone[0].metv.tavg < zone[0].defaults[0][0].max_snow_temp ){
			if (zone[0].metv.tavg <= zone[0].defaults[0][0].min_rain_temp){
				zone[0].snow = zone[0].rain;
				zone[0].rain = 0.0;
			}
			else{
				snow_rain_range = zone[0].defaults[0][0].max_snow_temp
				- zone[0].defaults[0][0].min_rain_temp;
				if ( snow_rain_range < 0.001){
					zone[0].snow = zone[0].rain;
					zone[0].rain = 0.0;
				}
				else{
					zone[0].snow = min((zone[0].defaults[0][0].max_snow_temp
										- zone[0].metv.tavg) * zone[0].rain 
									    / snow_rain_range, zone[0].rain);
					zone[0].rain = zone[0].rain - zone[0].snow;
				}
			}
		}
		else{
			zone[0].snow = 0.0;
		}
	}
	zone[0].snow += zone[0].snow_hourly_total;
	/*--------------------------------------------------------------*/
	/*	If we have no rain duration data set it as		*/
	/*	daylength if rain 0 if not.					*/
	/*--------------------------------------------------------------*/

	if ( zone[0].daytime_rain_duration == -999.0 ){
		if ( zone[0].rain == 0 || (zone[0].snow != 0 ) ){
			zone[0].daytime_rain_duration = 0;
		}
		else{
			/* Old code */
			/* zone[0].daytime_rain_duration = zone[0].metv.dayl;*/
			/* Since this value is used for both infiltration (intensity) and 
			 radiation, we are now defining it as # of seconds of rain over
			 ENTIRE 24-hr period. If no value is given, we assume it rains
			 over the full day. */
			zone[0].daytime_rain_duration = 86400;
		}
	}
	else{
		if ( zone[0].rain == 0 && zone[0].rain_hourly_total == 0){
			zone[0].daytime_rain_duration = 0;
		}
		else{
			/* Old code */
			/*zone[0].daytime_rain_duration =
				min( zone[0].metv.dayl,zone[0].daytime_rain_duration);*/
			/* We adjust this value in the radiation routines to split
			 into daylight vs. nighttime rain hours, so here we leave
			 it as-is and just bound to 0:86400 sec. */
			zone[0].daytime_rain_duration =
				min( 86400, max(zone[0].daytime_rain_duration,0));
		}
	}
	
	/*---------------------------------------------------------------*/
	/* MOVED CLOUD FRACTION CALCS INTO ZONE DAILY I AND EARLIER IN	 */
	/* THIS ROUTINE SO CAN BE USED FOR K ADJUSTMENTS				 */
	/*---------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Deretmine if we need to adjust Kdowns or metv.tmax.			*/
	/*																*/
	/*	We assume that if either Kdown_diffuse or Kdown_direct		*/
	/*	was not supplied then themetv.tmax needs adjustment.		*/
	/*	This is because the adjustment should not be needed			*/
	/*		if we had good enough data to get Kdowns.				*/
	/*--------------------------------------------------------------*/
	if ( (zone[0].Kdown_direct_flag == 0) ||
		(zone[0].Kdown_diffuse_flag == 0) ){
		/*--------------------------------------------------------------*/
		/*	Adjust the computed Kdown's for cloud fraction if they		*/
		/*	were not read in directly.									*/
		/*	See simuilate_zone_daily_1 for the adjustment computation	*/
		/*--------------------------------------------------------------*/
		zone[0].Kdown_direct = zone[0].Kdown_direct
			* zone[0].Kdown_direct_adjustment;
		zone[0].Kdown_diffuse = zone[0].Kdown_diffuse
			* zone[0].Kdown_diffuse_adjustment;
		if (command_line[0].verbose_flag == -5) {
			printf("\nZONE DAILY F: Precip=%lf cloudfrac=%lf Kdir_adj=%lf Kdif_adj=%lf Tavg=%lf", 
				   zone[0].snow + zone[0].rain, 
				   zone[0].cloud_fraction, 
				   zone[0].Kdown_direct_adjustment/86.4,
				   zone[0].Kdown_diffuse_adjustment/86.4,
				   zone[0].metv.tavg);
		}
		/*--------------------------------------------------------------*/
		/*	radrat (unitless)											*/
		/*																*/
		/*	Equation 2a & 2b , Page 5, "MTCLIM" 						*/
		/*--------------------------------------------------------------*/
		if ( (zone[0].Kdown_direct_flat + zone[0].Kdown_diffuse_flat) != 0.0 ) {
			zone[0].radrat = (zone[0].Kdown_direct + zone[0].Kdown_diffuse) /
			(zone[0].Kdown_direct_flat + zone[0].Kdown_diffuse_flat);   
		}
		else zone[0].radrat = 1.0;
		/*zone[0].radrat = 1.0;*/
		/* EG edit: when radrat equals zero, tmax= -Inf and PSN=Nan */
		if ( zone[0].radrat == 0.0) {
			zone[0].radrat = 1.0;
		}
		
		/*------------------------------------------------------------------*/
		/* REMOVING TMAX CORRECTION SINCE EQUATION IS ALWAYS ADDING 1 DEG	*/
		/* EVEN ON FLAT SURFACES WITH SAME LAI AS BASE. EQN NEEDS EDITS		*/
		/* BEFORE RE-IMPLEMENTING.											*/
		/*--------------------------------------------------------------*/
		/*	LAI compensation (unitless)									*/
		/*																*/
		/*	Equation 2a & 2b , Page 5, "MTCLIM" 						*/
		/*	Modified so that the base station LAI is subtracted from the*/
		/*		zone lai.												*/
		/*--------------------------------------------------------------*/
		//zone[0].effective_lai = 0.0;
		//for ( patch=0 ; patch<zone[0].num_patches ; patch++ ){
		//	zone[0].effective_lai
		//		+= zone[0].patches[patch][0].effective_lai
		//		* zone[0].patches[patch][0].area;
		//}
		//zone[0].effective_lai = zone[0].effective_lai / zone[0].area;
		//if ( zone[0].radrat <  1.0 ){
		//	zone[0].LAI_temp_adjustment
		//		=-1 * ( 1/zone[0].radrat ) * ( 1 + (zone[0].effective_lai
		//		- zone[0].base_station_effective_lai )
		//		/ zone[0].defaults[0][0].max_effective_lai );
		//}
		//else {
		//	zone[0].LAI_temp_adjustment = ( zone[0].radrat )
		//		* ( 1 - (zone[0].effective_lai
		//		- zone[0].base_station_effective_lai )
		//		/ zone[0].defaults[0][0].max_effective_lai );
		//}  /*end if-else*/
		/*--------------------------------------------------------------*/
		/*metv.tmax adjusted ( degrees C)								*/
		/*--------------------------------------------------------------*/
		//zone[0].metv.tmax = zone[0].metv.tmax + zone[0].LAI_temp_adjustment;
		/*-----------------------------------------------------------------*/
		
	} /*end if*/
	
	/* EG edit: LAI temp adjustment was pushing tmax below tmin*/
	if (zone[0].metv.tmax < zone[0].metv.tmin) {
		zone[0].metv.tmax = zone[0].metv.tmin + 1.0;
	}
	/*--------------------------------------------------------------*/
	/*	If no PAR or only diffuse PAR is given get the other PAR's	*/
	/*	We assume the ratio of diffuse to direct PAR is a closed 	*/
	/*		form equation of the ratio of diffuse to direct Kdown.	*/
	/*--------------------------------------------------------------*/
	if ( (zone[0].PAR_direct == -999.0 ) && ( zone[0].PAR_diffuse == -999.0) ){
		zone[0].PAR_direct = 1000.0 * zone[0].Kdown_direct * RAD2PAR * EPAR;
		zone[0].PAR_diffuse = 1000.0 *  zone[0].Kdown_diffuse * RAD2PAR * EPAR;
	}
	else if ( zone[0].PAR_direct == -999.0 ){
		zone[0].PAR_direct = zone[0].PAR_diffuse * (zone[0].Kdown_direct
			/ zone[0].Kdown_diffuse );
	}
	else if ( zone[0].PAR_diffuse == -999.0 ){
		zone[0].PAR_diffuse = zone[0].PAR_direct * (zone[0].Kdown_diffuse
			/ zone[0].Kdown_direct );
	}

	
	/*-------------------------------------------------------------------*/
	/* LDOWN MODEL REPLACED WITH NEW ONE CALCULATED AFTER VAPOR PRESSURE */
	/*-------------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Saturation Vapour Pressure	(Pa)							*/
	/*																*/
	/*	Note the original rehssys code supplied es in mbar.			*/
	/*	c.f. eq. 1 Running and Coughlan , 1987, p. 133.				*/
	/*																*/
	/*	Since 1 bar = 100 kpa (approx) ; a millibar = 100 Pa approx.*/
	/*	This explains why the es from the original code was:		*/
	/*																*/
	/*	6.1078 * exp((17.269*z[0].metv.tday)/(237.3 +			*/
	/*									z[0].metv.tday))		*/
	/*																*/
	/*	Which is approx 100 times that of the es here.				*/
	/*																*/
	/*	Eq. 5.12 p. 110, Jones, "Plants and Microclimate"			*/
	/*--------------------------------------------------------------*/
	if (  zone[0].metv.vpd == -999.0  ){
		es = 613.75 * exp( (17.502 * zone[0].metv.tavg)
			/ ( 240.97 + zone[0].metv.tavg) );
		/*--------------------------------------------------------------*/
		/*	Make use of relative humidity if available.					*/
		/*--------------------------------------------------------------*/
		if ( zone[0].relative_humidity == -999.0 ){
			/*--------------------------------------------------------------*/
			/*	Dew Point Vapour Pressure (Pa)								*/
			/*																*/
			/*	Note the original rehssys code supplied es in mbar.			*/
			/*	c.f. eq. 1 Running and Coughlan , 1987, p. 133.				*/
			/*																*/
			/*	Since 1 bar = 100 kpa (approx) ; a millibar = 100 Pa approx.*/
			/*	This explains why the es from the original code was:		*/
			/*																*/
			/*	6.1078 * exp((17.269*z[0].tdewpoint)/(237.3 +			*/
			/*									z[0].tdewpoint))		*/
			/*																*/
			/*	Which is approx 100 times that of the es here.				*/
			/*																*/
			/*	Eq. 5.12 p. 110, Jones, "Plants and Microclimate"			*/
			/*	Assuming that tdewpoint is valid for the whole day.		*/
			/*								*/
			/*	We cannot make a correction for rain_duration here.	*/
			/*	Instead we hope that  the dewpoint vapour pressure is	*/
			/*	measured by a temperature value (night min)		*/
			/*--------------------------------------------------------------*/
			zone[0].e_dewpoint = 613.750 * exp( (17.502
				* zone[0].tdewpoint) / ( 240.97 + zone[0].tdewpoint));
		}
		else{
			/*--------------------------------------------------------------*/
			/*      Dew Point Vapour Pressure (Pa)                          */
			/*                                                              */
			/*      ONly for dayligh conditions with no rain.               */
			/*      Eq. 5.13 and 5.14 , p. 110, Jones, "Plants and Microclimate"*/
			/*--------------------------------------------------------------*/
			zone[0].e_dewpoint =  zone[0].relative_humidity * es;
		} /*end if-else*/
		/*--------------------------------------------------------------*/
		/*	metv.vpd	(Pa)						*/
		/*								*/
		/*	Eq. 5.14, p. 110, Jones, "Plants and Microclimate"	*/
		/*	Limited to at least 0.0 as per rhessys C code.		*/
		/*--------------------------------------------------------------*/

		zone[0].metv.vpd = max(es - zone[0].e_dewpoint,0.0);
		if (zone[0].relative_humidity == -999.0) {
			if (es > ZERO)
				zone[0].relative_humidity = zone[0].e_dewpoint / es;
			else
				zone[0].relative_humidity = -999.0;
			}
	}
	/* Case where vpd is given. Still need to calculate e_dewpoint	*/
	/* for snowpack sublim and RH for output.						*/
	else {
		es = 613.75 * exp( (17.502 * zone[0].metv.tavg)
						  / ( 240.97 + zone[0].metv.tavg) );
		zone[0].e_dewpoint = es - zone[0].metv.vpd;
		zone[0].relative_humidity = zone[0].e_dewpoint / es;
	}
	
	/*--------------------------------------------------------------*/
	/* NEW ATMOSPHERIC LONGWAVE MODEL								*/
	/* Clear sky emissivity from Satterlund 1979 as applied in		*/
	/* Mahat & Tarboten 2012 UEB with cloud fraction correction.	*/
	/* After testing, best results from Diley-Crawford model but	*/
	/* leaving others in as comments in case someone else wants		*/
	/* to experiment.												*/
	/*--------------------------------------------------------------*/
	if ( zone[0].Ldown == -999.0){
		/* Satterlund-Crawford */
		/*zone[0].Ldown = (zone[0].cloud_fraction 
			+ (1.0 - zone[0].cloud_fraction) * 1.08 * (1.0 - exp(-pow(zone[0].e_dewpoint/100,(zone[0].metv.tavg+273.16)/2016)))) 
			* (SBC * 86400/1000) * pow(zone[0].metv.tavg+273.16,4);*/
		
		/* Diley-Crawford */
		zone[0].Ldown = zone[0].cloud_fraction * (SBC * 86400/1000) * pow(zone[0].metv.tavg+273.16,4)
				+ (1.0 - zone[0].cloud_fraction) * ( 59.38 + 113.7*pow((zone[0].metv.tavg+273.16)/273.16,6)
				+ 96.96 * pow(4650*(zone[0].e_dewpoint/1000)/(zone[0].metv.tavg+273.16)/25,0.5)) * 86400/1000;
		
		/* split by day/night (not sensitive) */
		/*zone[0].Ldown = ( zone[0].cloud_fraction * (SBC * (seconds_per_day-zone[0].metv.dayl)/1000) * pow(zone[0].metv.tnight+273,4)
						    + (1.0 - zone[0].cloud_fraction) * ( 59.38 + 113.7*pow((zone[0].metv.tnight+273)/273.16,6)
						    + 96.96 * pow(4650*(zone[0].e_dewpoint/1000)/(zone[0].metv.tnight+273)/25,0.5)) * (seconds_per_day-zone[0].metv.dayl)/1000 )
					+ ( zone[0].cloud_fraction * (SBC * zone[0].metv.dayl/1000) * pow(zone[0].metv.tday+273,4)
					        + (1.0 - zone[0].cloud_fraction) * ( 59.38 + 113.7*pow((zone[0].metv.tday+273)/273.16,6)
							+ 96.96 * pow(4650*(zone[0].e_dewpoint/1000)/(zone[0].metv.tday+273)/25,0.5)) * zone[0].metv.dayl/1000 );*/
		
		/* Diley with Kimball cloud correction per Flerchinger 2009 */
		/*if ( (current_date.month>=5) && (current_date.month<=10) )
			Tcloud = (zone[0].metv.tavg+273.16) - 9; /* summer adjustment for cloud temp */
		/*else 
			Tcloud = (zone[0].metv.tavg+273.16) - 13; /* winter adjustment for cloud temp */
		/*f8 = -0.6732 + (0.006240*Tcloud) - (0.9140*pow(10,-5)*pow(Tcloud,2));
		e8z = 0.24 + (2.98*pow(10,-6) * pow(zone[0].e_dewpoint/1000,2) * exp(3000.0/(zone[0].metv.tavg+273.16)));
		tau8 = 1.0 - e8z*(1.4-0.4*e8z);
		zone[0].Ldown = zone[0].cloud_fraction * f8 * tau8 * (SBC * 86400/1000) * pow(Tcloud,4)
				+ (1.0 - zone[0].cloud_fraction) * ( 59.38 + 113.7*pow((zone[0].metv.tavg+273.16)/273.16,6)
				+ 96.96 * pow(4650*(zone[0].e_dewpoint/1000)/(zone[0].metv.tavg+273.16)/25,0.5)) * 86400/1000;*/
	} /*end if*/
	/*--------------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	Nitrogen Deposition					*/
	/*	- if not availabe use default value			*/
	/*--------------------------------------------------------------*/
	if (zone[0].ndep_NO3 == -999.0){
		zone[0].ndep_NO3 = zone[0].defaults[0][0].ndep_NO3;
	}
	/*--------------------------------------------------------------*/
	/*	metv.tsoil - soil temperature 		(degrees C)	*/
	/*								*/
	/*	We always update a running average for metv.tsoil in		*/
	/*	case we get a missing value.						*/
	/*--------------------------------------------------------------*/
	if ( zone[0].metv.tsoil == -999.0 ){
		zone[0].metv.tsoil_sum = 0.9 * zone[0].metv.tsoil_sum +
			0.1 *	zone[0].metv.tavg;
		zone[0].metv.tsoil = zone[0].metv.tsoil_sum;
	}
	else{
		zone[0].metv.tsoil_sum = zone[0].metv.tsoil;
	}
	/*--------------------------------------------------------------*/
	/* 	set LAI scalar to 1.0 is missing 			*/
	/*--------------------------------------------------------------*/
	if ( zone[0].LAI_scalar == -999.0 ){
		zone[0].LAI_scalar = 1.0;
	}
	/*--------------------------------------------------------------*/
	/*	Atmospheric CO2 concentration (ppm)			*/
	/*--------------------------------------------------------------*/
	if ( zone[0].CO2 == -999.0 ){
		zone[0].CO2 = zone[0].defaults[0][0].atm_CO2;
	}
	/*--------------------------------------------------------------*/
	/*	Fill in the bbgc metv structure for missing variables	*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	compute total precip 			kg/m2		*/
	/*								*/
	/*	assumes density snow = debsity rain			*/
	/*	kg/m2 = (m / m2) *( 1000kg/m3)								*/
	/*--------------------------------------------------------------*/
	zone[0].metv.prcp = (zone[0].rain + zone[0].snow)*1000.0;
	/*--------------------------------------------------------------*/
	/*	compute total shortwave flux		(W/m2)		*/
	/*	W/m2 = Kj/(m2*day) * ( 1 day / dayl s ) * 1000 j / 1kj  */
	/*--------------------------------------------------------------*/
	zone[0].metv.swavgfd = (zone[0].Kdown_direct + zone[0].Kdown_diffuse)
		/ zone[0].metv.dayl * 1000.0  ;
	/*--------------------------------------------------------------*/
	/*	compute total ppfd		umol photon/m2*s	*/
	/*	umol photon/m2*s = u mol photon/m2*day * (1 day/dayl s)	*/
	/*--------------------------------------------------------------*/
	zone[0].metv.ppfd = (zone[0].PAR_direct + zone[0].PAR_diffuse)
		/ zone[0].metv.dayl ;
	if ( command_line[0].verbose_flag > 1 )
		printf("\n%8d -222.1 ",julday(current_date)-2449000);
	if ( command_line[0].verbose_flag > 0  )
		printf("%8.4f %8.4f %8.4f %8.4f %8.4f %8.4f %8.4f %8.4f ",
		zone[0].metv.dayl,
		zone[0].metv.tmax,
		zone[0].metv.tmin,
		zone[0].metv.tavg,
		zone[0].metv.tnight,
		zone[0].metv.tday,
		zone[0].metv.tsoil,
		zone[0].tdewpoint);
	if ( command_line[0].verbose_flag > 1 )
		printf("\n%8d -222.2 ",julday(current_date)-2449000);
	if ( command_line[0].verbose_flag > 0 )
		printf("%8.4f %8.4f %8.2f ",
		zone[0].rain,
		zone[0].snow,
		zone[0].metv.vpd);
	if ( command_line[0].verbose_flag > 1 )
		printf("%8.1f %8.1f %8.0f %8.0f %8.1f ",
		zone[0].Kdown_direct,
		zone[0].Kdown_diffuse,
		zone[0].PAR_direct,
		zone[0].PAR_diffuse,
		zone[0].Ldown);
	/*--------------------------------------------------------------*/
	/*      update met running averages variables                            */
	/*--------------------------------------------------------------*/

	zone[0].metv.tmin_ravg = 1.0/(zone[0].defaults[0][0].ravg_days)*zone[0].metv.tmin 
			+ (zone[0].defaults[0][0].ravg_days-1.0)/(zone[0].defaults[0][0].ravg_days)*zone[0].metv.tmin_ravg;
	zone[0].metv.vpd_ravg = 1.0/(zone[0].defaults[0][0].ravg_days)*zone[0].metv.vpd 
			+ (zone[0].defaults[0][0].ravg_days-1.0)/(zone[0].defaults[0][0].ravg_days)*zone[0].metv.vpd_ravg;
	zone[0].metv.dayl_ravg = 1.0/(zone[0].defaults[0][0].ravg_days)*zone[0].metv.dayl 
			+ (zone[0].defaults[0][0].ravg_days-1.0)/(zone[0].defaults[0][0].ravg_days)*zone[0].metv.dayl_ravg;

	
	if (command_line[0].verbose_flag == -5) {
		printf(" Kdowndir=%lf Kdowndiff=%lf Ldown=%lf", 
			   zone[0].Kdown_direct/86.4,
			   zone[0].Kdown_diffuse/86.4,
			   zone[0].Ldown/86.4);
	}
	
	/*--------------------------------------------------------------*/
	/*	Cycle through the patches for day end computations		    	*/
	/*--------------------------------------------------------------*/
	for ( patch=0 ; patch<zone[0].num_patches; patch++ ){
		patch_daily_F(
			world,
			basin,
			hillslope,
			zone,
			zone[0].patches[patch],
			command_line,
			event,
			current_date );

	  if(command_line[0].vegspinup_flag > 0){
      if (zone[0].patches[patch]->target_status == 0){
        world[0].target_status = 0;
      }
    }
	}

	/*--------------------------------------------------------------*/
	/*      update accumulator variables                            */
	/*--------------------------------------------------------------*/
	if ((command_line[0].output_flags.monthly == 1) &&
		(command_line[0].z != NULL) ){
		zone[0].acc_month.precip += zone[0].rain + zone[0].snow;
		zone[0].acc_month.tmin += zone[0].metv.tmin;
		zone[0].acc_month.tmax += zone[0].metv.tmax;
		zone[0].acc_month.K_direct += zone[0].Kdown_direct;
		zone[0].acc_month.K_diffuse += zone[0].Kdown_diffuse;
		zone[0].acc_month.length += 1;
	}

	return;
} /*end zone_daily_F.c*/
