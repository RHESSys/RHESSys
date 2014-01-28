/*--------------------------------------------------------------*/
/* 								*/
/*		compute_hourly_litter_rain_stored				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_hourly_litter_rain_stored -  rain intercepted by litter and 	*/
/*		updates rain flaaing through.			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_hourly_litter_rain_stored(				*/
/*			struct	canopy_strata_object	*);	*/
/*								*/
/*	returns:						*/
/*	rain_storage (m) - amount of rain left in canopy	*/
/*								*/
/*								*/
/*	also updates detention store, won't count litter evaporation 	*/
/*		for the patch in this hourly routing		*/
/*	DESCRIPTION						*/
/*	this routing is copied and edited from the daily version*/
/* 	deleting the evaporation part, because in hourly routing*/
/* 	we assume there is little ET during storm event.	*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	compute_hourly_litter_rain_stored(
								   int	verbose_flag,
								   struct	patch_object	*patch) 
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double	potential_interception;
	double	rain_storage;
	double	throughfall;
	struct	litter_object *litter;
	
	litter = &(patch[0].litter);
	/*--------------------------------------------------------------*/
	/*	Compute amount potentially intercepted.			*/
	/*	m = m2PlANT / m2ground *  ( (kg  / m2 * day * m2PLANT )	*/
	/*		* ( 1 m3 H20 / 1000 kg H20 )			*/
	/*--------------------------------------------------------------*/
	potential_interception = min(  patch[0].detention_store,
		 litter[0].rain_capacity
		- litter[0].rain_stored);

	if (potential_interception < ZERO) potential_interception = 0.0;

	/*--------------------------------------------------------------*/
	/*	Add amount not potentially intercepted to throughfall.	*/
	/*	m = m - m 						*/
	/*--------------------------------------------------------------*/
	throughfall  = patch[0].detention_store - potential_interception;
	/*--------------------------------------------------------------*/
	/*  Update rain storage.										*/
	/*	m = m							*/
	/*--------------------------------------------------------------*/
	rain_storage = min( litter[0].rain_stored + potential_interception,
		 litter[0].rain_capacity );
	if( verbose_flag > 2)
		printf("%8.6f ",rain_storage);
	/*--------------------------------------------------------------*/
	/*	Update rain throughfall.									*/
	/*	m += m							*/
	/*--------------------------------------------------------------*/
	throughfall += max(potential_interception - (rain_storage
		- litter[0].rain_stored),0);
	if( verbose_flag > 2)
		printf("%8.6f ",throughfall);
	patch[0].detention_store = throughfall;

	return( rain_storage);
} /*end compute_litter_rain_stored*/
