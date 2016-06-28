/*--------------------------------------------------------------*/
/* 								*/
/*		compute_litter_rain_stored				*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_litter_rain_stored -  rain intercepted by litter and 	*/
/*		updates rain flaaing through.			*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_litter_rain_stored(				*/
/*			struct	canopy_strata_object	*);	*/
/*								*/
/*	returns:						*/
/*	rain_storage (m) - amount of rain left in canopy	*/
/*								*/
/*								*/
/*	also updates detention store, and litter evaporation 	*/
/*		for the patch					*/
/*	DESCRIPTION						*/
/*								*/
/*	Updates amount of rain stored after using supplied 	*/
/*	daytime potential evaporation,  rainfall 			*/ 
/*	to evaporate and input respectively to the storage.	*/
/*	Evaporation from the storage is performed first.	*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

double	compute_litter_rain_stored(
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
	double	potential_evaporation;
	double	rain_storage;
	double	storage_evaporated, actual_evaporated;
	double	potential_interception_evaporated;
	double	throughfall;
	struct	litter_object *litter;
	
	if (verbose_flag == -5) {
		printf("\n     COMPUTE LITTER RAIN STORED: START evap_surf=%lf",patch[0].evaporation_surf);
	}
	
	litter = &(patch[0].litter);
	/*--------------------------------------------------------------*/
	/*	Transfer potential evaporation to a working variable.	*/
	/*--------------------------------------------------------------*/
	potential_evaporation = patch[0].potential_evaporation;
			
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
	/*	Compute amount of storage evaporated.			*/
	/*	m = m							*/
	/*--------------------------------------------------------------*/
	storage_evaporated = min(potential_evaporation,litter[0].rain_stored);
	/*--------------------------------------------------------------*/
	/*	Update amount of rain in storage after evaporation.	*/
	/*	m = m							*/
	/*--------------------------------------------------------------*/
	litter[0].rain_stored  -= storage_evaporated;
	/*--------------------------------------------------------------*/
	/*	Update potentail evaporation.				*/
	/*	m = m							*/
	/*--------------------------------------------------------------*/
	potential_evaporation -= storage_evaporated;
	/*--------------------------------------------------------------*/
	/*	Compute amount of potential interception evaporated.	*/
	/* 	m = m							*/
	/*--------------------------------------------------------------*/
	potential_interception_evaporated  =
		min( potential_evaporation, potential_interception);

	potential_interception_evaporated = max(0.0, potential_interception_evaporated);
	/*--------------------------------------------------------------*/
	/*	Update the potential interception after evaporation.	*/
	/* 	m -= m							*/
	/*--------------------------------------------------------------*/
	potential_interception -= potential_interception_evaporated;
	/*--------------------------------------------------------------*/
	/*	Compute amount of evaporation that happened.		*/
	/* 	m = m + m						*/
	/*--------------------------------------------------------------*/
	actual_evaporated = storage_evaporated+potential_interception_evaporated;
	if (patch[0].snowpack.water_equivalent_depth > ZERO) {
		patch[0].snowpack.water_equivalent_depth += actual_evaporated;
	}
	else {
		patch[0].evaporation_surf +=  actual_evaporated;
		}
	/*--------------------------------------------------------------*/
	/*	Adjust the amount of remaining potential evaporation	*/
	/*--------------------------------------------------------------*/
	patch[0].potential_evaporation -= actual_evaporated;
	if( verbose_flag >2)
		printf("%8.6f ",patch[0].evaporation_surf);
	if( verbose_flag >2)
		printf("%8.6f ",potential_interception_evaporated);
	if( verbose_flag >2)
		printf("%8.6f ",storage_evaporated);
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
	
	if (verbose_flag == -5) {
		printf(" END evap_surf=%lf",patch[0].evaporation_surf);
	}

	return( rain_storage);
} /*end compute_litter_rain_stored*/
