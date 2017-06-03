/*--------------------------------------------------------------*/
/* 																*/
/*						zone_hourly								*/
/*																*/
/*	NAME														*/
/*	zone_hourly 												*/
/*				 - performs cycling and output of a zone		*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void zone_hourly( 											*/
/*						 struct world_object *	,				*/
/*						 struct basin_object *	,				*/
/*						 struct hillslope_object *	,			*/
/*						 struct zone_object *	,				*/
/*						 struct command_line_object * ,			*/
/*						 struct tec_entry *,					*/
/*						 struct date) 							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	intervals in the zone.  The routine also prints out results	*/
/*	where specified by current tec events files.				*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/* March 9, 1997 - C.Tague										*/
/*	- fixed bug - missplace endif bracket						*/
/*	causing instability in rad. calculations					*/
/*																*/
/* May 15, 1997 - C. Tague					*/
/*	- added a check on cos_slope_beam_angle			*/
/*	so that if it is negative - addition to			*/
/*	direct radiation is zero (not negative)			*/
/*	i.e if sun is behind a hill, no direct radiation	*/ 
/*	is added						*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void		zone_hourly(
						struct	world_object 	*world,
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
	void patch_hourly (
		struct	world_object 	*,
		struct	basin_object	*,
		struct	hillslope_object	*,
		struct 	zone_object 	*,
		struct patch_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date);
	
	void	*alloc(	size_t, char *, char *);
	long  julday( struct date );
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int 	patch;
	int	inx;
	double	Kdown_direct_flat_toa;
	double	temp;
	struct	dated_sequence	clim_event;
	double 	snow_rain_range;
	/*--------------------------------------------------------------*/
	/* 	check for hourly precipitation data			*/
	/* 	for now only assume one base station per zone		*/
	/*--------------------------------------------------------------*/
	zone[0].hourly_rain_flag = 0;
	zone[0].hourly[0].rain = 0.0;
	zone[0].hourly[0].snow = 0.0; 
	inx = zone[0].base_stations[0][0].hourly_clim[0].rain.inx;

	if (inx > -999)  {
		clim_event = zone[0].base_stations[0][0].hourly_clim[0].rain.seq[inx];
		while (clim_event.edate.year!=0 && (julday(clim_event.edate) + clim_event.edate.hour/24.0 < julday(current_date) + current_date.hour/24.0)) {
			zone[0].base_stations[0][0].hourly_clim[0].rain.inx += 1;
			inx = zone[0].base_stations[0][0].hourly_clim[0].rain.inx;
			clim_event = zone[0].base_stations[0][0].hourly_clim[0].rain.seq[inx];
			}
		
		if ( (clim_event.edate.year != 0) &&
			(julday(clim_event.edate) == julday(current_date)) && (clim_event.edate.hour == current_date.hour) ) {
			zone[0].hourly_rain_flag = 1;
			zone[0].hourly[0].rain = clim_event.value;

			/*--------------------------------------------------------------*/
			/*if there is hourly input, turn daily rain to 0 		*/
			/*--------------------------------------------------------------*/
			zone[0].rain = 0;


			/*--------------------------------------------------------------*/
			/*if temperture is low enough, some of rain will turn to snow	*/
			/*--------------------------------------------------------------*/
			
			/*--------------------------------------------------------------*/
			/*	.metv.tavg	(degrees C)				*/
			/*--------------------------------------------------------------*/
			if ( zone[0].metv.tavg == -999.0 ){
				zone[0].metv.tavg = (zone[0].metv.tmax + zone[0].metv.tmin)/2.0;
			}

			/*--------------------------------------------------------------*/
			/*	metv.tday	(degrees C)				*/
			/*								*/
			/*	Eq 1. Page 4, "MTCLIM"					*/
			/*--------------------------------------------------------------*/
			if ( zone[0].metv.tday == -999.0 ){
				zone[0].metv.tday = zone[0].defaults[0][0].temcf
					* (zone[0].metv.tmax - zone[0].metv.tavg) + zone[0].metv.tavg;
			}
			/*--------------------------------------------------------------*/
			/*	metv.tnight 	(degrees C)				*/
			/*								*/
			/*	zcomp.c C Rhessys code					*/
			/*--------------------------------------------------------------*/
			if ( zone[0].metv.tnight == -999.0 ){
				zone[0].metv.tnight = (zone[0].metv.tday + zone[0].metv.tmin)/2.0;
			}
			/*--------------------------------------------------------------*/
			/*	If we have no snow data determine if the rain should be snow*/
			/*								*/
			/*	Based on U.S Army Corps of ENgineers (1956) Snow Hydrology	*/
			/*	use a min/max to get range in which there is a mix of snow/rain */
			/*								*/
			/*--------------------------------------------------------------*/
			if (zone[0].hourly[0].snow == 0 ){
				if (zone[0].metv.tavg < zone[0].defaults[0][0].max_snow_temp ){
					if (zone[0].metv.tavg <= zone[0].defaults[0][0].min_rain_temp){
						zone[0].hourly[0].snow = zone[0].hourly[0].rain;
						zone[0].hourly[0].rain = 0.0;
					}
					else{
						snow_rain_range = zone[0].defaults[0][0].max_snow_temp
							- zone[0].defaults[0][0].min_rain_temp;
						if ( snow_rain_range < 0.001){
							zone[0].hourly[0].snow = zone[0].hourly[0].rain;
							zone[0].hourly[0].rain = 0.0;
						}
						else{
							zone[0].hourly[0].snow = min((zone[0].defaults[0][0].max_snow_temp
								- zone[0].metv.tavg) * zone[0].hourly[0].rain / snow_rain_range, zone[0].hourly[0].rain);
							zone[0].hourly[0].rain = zone[0].hourly[0].rain - zone[0].hourly[0].snow;
						}
					}
				}
				else{
					zone[0].hourly[0].snow = 0.0;
				}
			}
			

			inx = zone[0].base_stations[0][0].hourly_clim[0].rain_duration.inx;
			/*--------------------------------------------------------------*/
			/* 	check for corresponding duration data			*/
			/*	if not there assume full hour				*/
			/*--------------------------------------------------------------*/
			if (inx > -999) {
				clim_event = zone[0].base_stations[0][0].hourly_clim[0].rain_duration.seq[inx];
				while (julday(clim_event.edate) + clim_event.edate.hour/24.0 < julday(current_date) + current_date.hour/24.0) {
					zone[0].base_stations[0][0].hourly_clim[0].rain_duration.inx += 1;
					inx = zone[0].base_stations[0][0].hourly_clim[0].rain_duration.inx;
					clim_event = zone[0].base_stations[0][0].hourly_clim[0].rain_duration.seq[inx];
					}
				if ( (julday(clim_event.edate) == julday(current_date)) && (clim_event.edate.hour == current_date.hour) ) {
					zone[0].hourly[0].rain_duration = clim_event.value;
				}
				else zone[0].hourly[0].rain_duration = 3600;
			}
			else zone[0].hourly[0].rain_duration = 3600;
		}
	}
	zone[0].snow_hourly_total += zone[0].hourly[0].snow;
	zone[0].rain_hourly_total += zone[0].hourly[0].rain;
	
	/*--------------------------------------------------------------*/
	/*	count the daytime_rain_duration				*/
	/*--------------------------------------------------------------*/
	if (zone[0].hourly_rain_flag==1) {
		//if (current_date.hour==1) {
		//	zone[0].daytime_rain_duration=0;}
		
		if (zone[0].hourly[0].rain+zone[0].hourly[0].snow > 0) {
			zone[0].rain_duration += 3600;}
	}
			
	/*--------------------------------------------------------------*/
	/*	Compute zone hourly radiation forcings.								*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Kdown_direct or Kdown_diffuse.								*/
	/*																*/
	/*  Add to top of zone daily values for these terms if not		*/
	/*	provided.  													*/
	/*--------------------------------------------------------------*/
	if (/*(zone[0].Kdown_direct_flag == 0) ||
		(zone[0].Kdown_diffuse_flag == 0) ||*/
		(zone[0].daylength_flag == 0 )){
		/*--------------------------------------------------------------*/
		/*  Accumulate incoming Kdown if the sun is up.                 */
		/*                                                              */
		/*  We define the day length as the period where the cos_sza >0.*/
		/*  It is likely that Kdown_diffuse is non-zero even if         */
		/*  cos_sza < 0 but we ignore this for now.                     */
		/*--------------------------------------------------------------*/
		if ( basin[0].hourly[0].cos_sza  > 0 ){
			/*--------------------------------------------------------------*/
			/*		Atmospheric attenuation to direct radiation	(no units)  */
			/*--------------------------------------------------------------*/
			zone[0].hourly[0].direct_attenuation
				= pow( zone[0].atm_trans, basin[0].hourly[0].optical_air_mass);
			/*--------------------------------------------------------------*/
			/*		Downwelling total irradiance at BOA along sza			*/
			/*		W / m ** 2  = W /m ** 2  								*/
			/*--------------------------------------------------------------*/
			zone[0].hourly[0].Kdown_BOA
				= world[0].Io * zone[0].hourly[0].direct_attenuation;
			if (command_line[0].verbose_flag > 5) 
				printf("\n Io(W/m^2)= %f, Atm_trans= %f, air_mass= %f direct_att= %f Kdown_BOA(W/m^2)= %f ",
				world[0].Io,
				zone[0].atm_trans,
				basin[0].hourly[0].optical_air_mass,
				zone[0].hourly[0].direct_attenuation,
				zone[0].hourly[0].Kdown_BOA);
			/*--------------------------------------------------------------*/
			/*  Kdown_direct    (W/(m2)) 	                               */
			/*                                                              */
			/*  Eq. 5, Appendix D, "MTCLIM"                                 */
			/*                                                              */
			/*  We add in the constraint that the sun must be above the     */
			/*  horizon of the zone where the horizon is defined by        */
			/*  the eas and west horizons (assumes sun moves from east to   */
			/*  west with refernce to the zone).  Note that we define the  */
			/*  horizons as the angle from a flat surface at the zone and  */
			/*  NOT as the slope of the zone as zero.  We do this so that  */
			/*  the horizon can also be used to define a view factor.       */
			/*                                                              */
			/*  Note that we only accumulate the 24hr total Kdown_direct    */
			/*  so if you need the hourly Kdown_direct you have to make a   */
			/*  new state variable.                                         */
			/*																*/
			/*	Note that cos_sza = 1.0 if the sun is at a 90 degree angle	*/
			/*	to the flat surface at the zone.							*/
			/*																*/
			/* 	It is possible that the zone's east and west horizons do not*/
			/*	match that of the basins.  At present the basin's east and	*/
			/*	west horizons define length of daylight.  SO it is possible	*/
			/*	that, we consider that daylight is available evcen though	*/
			/*	the zone is still in shade.  We should decide if we should	*/
			/*	define daylength (and horizons) based on basin or zone.		*/
			/*--------------------------------------------------------------*/
			if ( (basin[0].hourly[0].cos_sza > zone[0].e_horizon) &&
				(basin[0].hourly[0].cos_sza > zone[0].w_horizon) ){
				/*--------------------------------------------------------------*/
				/*	Increment zone daylength				*/
				/*--------------------------------------------------------------*/
				if ( zone[0].daylength_flag == 0 ) zone[0].metv.dayl += 3600.0;
				/*--------------------------------------------------------------*/
				/*	Only do the following if we dont have direct radiation		*/
				/*--------------------------------------------------------------*/
				/*if ((zone[0].Kdown_direct_flag == 0) ||
					(zone[0].Kdown_diffuse_flag == 0)){*/
					/*------------------------------------------------------------*/
					/*     Kdown_direct_flat   (W/(m2))                            */
					/*                                                             */
					/*     Required to adjust max temp.                            */
					/*-------------------------------------------------------------*/
					zone[0].hourly[0].Kdown_direct_flat
						= basin[0].hourly[0].cos_sza * zone[0].hourly[0].Kdown_BOA;
					/*-------------------------------------------------------------*/
					/*     Cosine of beam slope angle. (no units)                  */
					/*                                                             */
					/*     Eq. 8 Appendix D of "MTCLIM"                            */
					/*-------------------------------------------------------------*/
					zone[0].hourly[0].cos_beam_slope
						= -1 * zone[0].sin_slope * zone[0].sin_aspect
						* basin[0].hourly[0].cos_declin_sin_hourangle
						+ ( -1 * zone[0].cos_aspect * zone[0].sin_slope
						* basin[0].sin_latitude + zone[0].cos_slope
						* basin[0].cos_latitude)
						* basin[0].hourly[0].cos_declin_cos_hourangle
						+( zone[0].cos_aspect * zone[0].sin_slope
						* basin[0].cos_latitude + zone[0].cos_slope
						* basin[0].sin_latitude ) * world[0].sin_declin;
					/*-------------------------------------------------------------*/
					/*	if sun angle is below the slope angle then		*/
					/*	hill is blocking sun and Kdown_direct is zero		*/
					/*-------------------------------------------------------------*/
					if (zone[0].hourly[0].cos_beam_slope < 0.0)
						zone[0].hourly[0].cos_beam_slope = 0.0;
					if (isnan(zone[0].hourly[0].cos_beam_slope))
						zone[0].hourly[0].cos_beam_slope = 0.0;

					/*----------------------------------------------------------*/
					/*		Kdown_direct	(W/(m2*day))							*/
					/*----------------------------------------------------------*/
					zone[0].hourly[0].Kdown_direct
						= zone[0].hourly[0].cos_beam_slope
						* zone[0].hourly[0].Kdown_BOA;
					/*-------------------------------------------------------------*/
					/*		Diffuse Radiation Calculations			*/
					/*		Source Daymet - Peter Thorto			*/
					/*		Further adjusts for horizon view factor.				*/
					/*------------------------------------------------------------*/
					if (zone[0].hourly[0].direct_attenuation > 0.000000001)
						Kdown_direct_flat_toa = zone[0].hourly[0].Kdown_direct_flat
						/ zone[0].hourly[0].direct_attenuation ;
					else
						Kdown_direct_flat_toa = 0.0;
					temp = sqrt( Kdown_direct_flat_toa
						* zone[0].hourly[0].Kdown_direct_flat);
					if (Kdown_direct_flat_toa < 0.0000000001)
						zone[0].hourly[0].Kdown_diffuse_flat = 0.0;
					else
						zone[0].hourly[0].Kdown_diffuse_flat
							= temp * (1.0 - temp / Kdown_direct_flat_toa);
					zone[0].hourly[0].Kdown_diffuse
						= zone[0].hourly[0].Kdown_diffuse_flat
						* pow(cos(zone[0].slope/2.0),2.0);
					if ( command_line[0].verbose_flag > 5 )
						printf("\n-111.3 cos_sza : %8.4f Kdown_dir_flat= %8.4f Kdown_dif_flat= %8.4f ",
						basin[0].hourly[0].cos_sza,
						zone[0].hourly[0].Kdown_direct_flat,
						zone[0].hourly[0].Kdown_diffuse_flat);
					if ( command_line[0].verbose_flag > 5 )
						printf(" cos_bsa= %8.4f Kdown_dir= %8.4f Kdown_dif= %8.4f Kdown_total(W/m^2)= %8.4f",
						zone[0].hourly[0].cos_beam_slope,
						zone[0].hourly[0].Kdown_diffuse,
						zone[0].hourly[0].Kdown_direct,
						zone[0].hourly[0].Kdown_direct
						+ zone[0].hourly[0].Kdown_diffuse);
					/*------------------------------------------------------------*/
					/*		Convert calculation fro W/m^2 to Kj/(m^2*hr)	*/
					/*-----------------------------------------------------------*/
					zone[0].Kdown_direct_flat_calc
						+= zone[0].hourly[0].Kdown_direct_flat * 3600 / 1000;
					zone[0].Kdown_direct_calc
						+= zone[0].hourly[0].Kdown_direct * 3600 / 1000;

					zone[0].Kdown_diffuse_flat_calc
						+= zone[0].hourly[0].Kdown_diffuse_flat * 3600 / 1000;
					zone[0].Kdown_diffuse_calc
						+= zone[0].hourly[0].Kdown_diffuse * 3600 / 1000;
				/*} end if */ 
			} /*end if*/
			} /*end if*/
		} /*end if*/
		/*--------------------------------------------------------------*/
		/*	Cycle through the patches 									*/
		/*--------------------------------------------------------------*/
		for ( patch=0 ; patch<zone[0].num_patches; patch++ ){
			patch_hourly(
				world,
				basin,
				hillslope,
				zone,
				zone[0].patches[patch],
				command_line,
				event,
				current_date );
		}


		return;
} /*end zone_hourly.c*/
