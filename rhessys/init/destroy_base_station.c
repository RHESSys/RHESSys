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
	free( base_station[0].daily_clim );
	free( base_station[0].monthly_clim );
	free( base_station[0].hourly_clim );
	free( base_station[0].yearly_clim );
	/*--------------------------------------------------------------*/
	/*	Destroy the base station object's array.					*/
	/*--------------------------------------------------------------*/
	free( base_station );
	return;
} /*end destroy_base_stations*/
