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
	free( base_station[0].daily_clim[0].tmin);
	free( base_station[0].daily_clim[0].tmax);
	free( base_station[0].daily_clim[0].rain);
	free( base_station[0].daily_clim[0].atm_trans);
	free( base_station[0].daily_clim[0].CO2);
	free( base_station[0].daily_clim[0].cloud_fraction);
	free( base_station[0].daily_clim[0].cloud_opacity);
	free( base_station[0].daily_clim[0].dayl);
	free( base_station[0].daily_clim[0].Delta_T);
	free( base_station[0].daily_clim[0].dewpoint);
	free( base_station[0].daily_clim[0].base_station_effective_lai);
	free( base_station[0].daily_clim[0].Kdown_diffuse);
	free( base_station[0].daily_clim[0].Kdown_direct);
	free( base_station[0].daily_clim[0].LAI_scalar);
	free( base_station[0].daily_clim[0].Ldown);
	free( base_station[0].daily_clim[0].PAR_diffuse);
	free( base_station[0].daily_clim[0].PAR_direct);
	free( base_station[0].daily_clim[0].relative_humidity);
	free( base_station[0].daily_clim[0].snow);
	free( base_station[0].daily_clim[0].tdewpoint);
	free( base_station[0].daily_clim[0].tday);
	free( base_station[0].daily_clim[0].tnight);
	free( base_station[0].daily_clim[0].tnightmax);
	free( base_station[0].daily_clim[0].tavg);
	free( base_station[0].daily_clim[0].tsoil);
	free( base_station[0].daily_clim[0].vpd);
	free( base_station[0].daily_clim[0].wind);
	free( base_station[0].daily_clim[0].wind_direction);
	free( base_station[0].daily_clim[0].ndep_NO3);
	free( base_station[0].daily_clim[0].ndep_NH4);
	free( base_station[0].daily_clim[0].lapse_rate_tmax);
	free( base_station[0].daily_clim[0].lapse_rate_tmin);
	free( base_station[0].daily_clim[0].daytime_rain_duration);
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
