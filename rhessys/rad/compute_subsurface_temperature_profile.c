/*--------------------------------------------------------------*/
/* 								*/
/*			compute_subsurface_temperature_profile				*/
/*								*/
/*	compute_subsurface_temperature_profile - computes heat flux	*/
/*		between surface of patch and a stratum.		*/
/*								*/
/*	NAME							*/
/*	compute_subsurface_temperature_profile						*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_subsurface_temperature_profile(				*/
/*													*/
/*	OPTIONS											*/
/*	DESCRIPTION										*/
/* 	interatively solve a soil temperature profile and aggregate to litter, rooting zone */
/*	unsat and sat moisture profiles (se_profile) 		*/
/*													*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Sep 2, 1997 RAF									*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

void compute_subsurface_temperature_profile( 
					  struct surface_energy_profile	*se_profile,
					  struct surface_energy_default	*sedef,
					  double tsurface,
					  double rnet,
					  double *percent_soil_water_unfrozen)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double tmp;

	tmp = 	HVAP;



	return;
} /*end compute_subsurface_temperature_profile*/
