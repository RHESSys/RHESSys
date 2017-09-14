/*--------------------------------------------------------------*/
/* 																*/
/*					destroy_base_stations	 					*/
/*																*/
/*	destroy_base_stations.c - destroy base station objects 		*/
/*																*/
/*	NAME														*/
/*	destroy_base_stations.c - destroy base station objects 		*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMERS NOTES											*/
/*																*/
/*	Original code - MARCH 15, 1996								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void destroy_base_station( struct command_line_object *command_line,
						  struct base_station_object *base_station)
{
	/*--------------------------------------------------------------*/
	/*	local function declarations.								*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	local variable declarations 								*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Destroy the base station clim objects.						*/
	/*--------------------------------------------------------------*/
	if(base_station[0].daily_clim[0].tmin!=NULL) free( base_station[0].daily_clim[0].tmin);
	if(base_station[0].daily_clim[0].tmax!=NULL) free( base_station[0].daily_clim[0].tmax);
	if(base_station[0].daily_clim[0].rain!=NULL) free( base_station[0].daily_clim[0].rain);
	if(base_station[0].daily_clim[0].atm_trans!=NULL) free( base_station[0].daily_clim[0].atm_trans);
	if(base_station[0].daily_clim[0].CO2!=NULL) free( base_station[0].daily_clim[0].CO2);
	if(base_station[0].daily_clim[0].cloud_fraction!=NULL) free( base_station[0].daily_clim[0].cloud_fraction);
	if(base_station[0].daily_clim[0].cloud_opacity!=NULL) free( base_station[0].daily_clim[0].cloud_opacity);
	if(base_station[0].daily_clim[0].dayl!=NULL) free( base_station[0].daily_clim[0].dayl);
	if(base_station[0].daily_clim[0].Delta_T!=NULL) free( base_station[0].daily_clim[0].Delta_T);
	if(base_station[0].daily_clim[0].dewpoint!=NULL) free( base_station[0].daily_clim[0].dewpoint);
	if(base_station[0].daily_clim[0].base_station_effective_lai!=NULL) free( base_station[0].daily_clim[0].base_station_effective_lai);
	if(base_station[0].daily_clim[0].Kdown_diffuse!=NULL) free( base_station[0].daily_clim[0].Kdown_diffuse);
	if(base_station[0].daily_clim[0].Kdown_direct!=NULL) free( base_station[0].daily_clim[0].Kdown_direct);
	if(base_station[0].daily_clim[0].LAI_scalar!=NULL) free( base_station[0].daily_clim[0].LAI_scalar);
	if(base_station[0].daily_clim[0].Ldown!=NULL) free( base_station[0].daily_clim[0].Ldown);
	if(base_station[0].daily_clim[0].PAR_diffuse!=NULL) free( base_station[0].daily_clim[0].PAR_diffuse);
	if(base_station[0].daily_clim[0].PAR_direct!=NULL) free( base_station[0].daily_clim[0].PAR_direct);
	if(base_station[0].daily_clim[0].relative_humidity!=NULL) free( base_station[0].daily_clim[0].relative_humidity);
	if(base_station[0].daily_clim[0].snow!=NULL) free( base_station[0].daily_clim[0].snow);
	if(base_station[0].daily_clim[0].tdewpoint!=NULL) free( base_station[0].daily_clim[0].tdewpoint);
	if(base_station[0].daily_clim[0].tday!=NULL) free( base_station[0].daily_clim[0].tday);
	if(base_station[0].daily_clim[0].tnight!=NULL) free( base_station[0].daily_clim[0].tnight);
	if(base_station[0].daily_clim[0].tnightmax!=NULL) free( base_station[0].daily_clim[0].tnightmax);
	if(base_station[0].daily_clim[0].tavg!=NULL) free( base_station[0].daily_clim[0].tavg);
	if(base_station[0].daily_clim[0].tsoil!=NULL) free( base_station[0].daily_clim[0].tsoil);
	if(base_station[0].daily_clim[0].vpd!=NULL) free( base_station[0].daily_clim[0].vpd);
	if(base_station[0].daily_clim[0].wind!=NULL) free( base_station[0].daily_clim[0].wind);
	if(base_station[0].daily_clim[0].wind_direction!=NULL) free( base_station[0].daily_clim[0].wind_direction);
	if(base_station[0].daily_clim[0].ndep_NO3!=NULL) free( base_station[0].daily_clim[0].ndep_NO3);
	if(base_station[0].daily_clim[0].ndep_NH4!=NULL) free( base_station[0].daily_clim[0].ndep_NH4);
	if(base_station[0].daily_clim[0].lapse_rate_tmax!=NULL) free( base_station[0].daily_clim[0].lapse_rate_tmax);
	if(base_station[0].daily_clim[0].lapse_rate_tmin!=NULL) free( base_station[0].daily_clim[0].lapse_rate_tmin);
	if(base_station[0].daily_clim[0].daytime_rain_duration!=NULL) free( base_station[0].daily_clim[0].daytime_rain_duration);
	free( base_station[0].daily_clim );
	free( base_station[0].monthly_clim );
	free( base_station[0].hourly_clim[0].rain.seq);
	free( base_station[0].hourly_clim[0].rain_duration.seq);
	
	free( base_station[0].hourly_clim );
	free( base_station[0].yearly_clim );
	/*--------------------------------------------------------------*/
	/*	Destroy the base station object's array.					*/
	/*--------------------------------------------------------------*/
	free( base_station );
	return;
} /*end destroy_base_stations*/
