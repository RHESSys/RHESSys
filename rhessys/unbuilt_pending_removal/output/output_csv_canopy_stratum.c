/*--------------------------------------------------------------*/
/* 																*/
/*					output_csv_canopy_stratum						*/
/*																*/
/*	output_csv_canopy_stratum - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_csv_canopy_stratum - outputs current contents of a canopy_stratum.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_csv_canopy_stratum(										*/
/*					struct	canopy_stratum_object	*canopy_stratum,				*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs spatial structure according to commandline			*/
/*	specifications to specific files							*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_csv_canopy_stratum( int basinID, int hillID, int zoneID, int patchID,
							  struct	canopy_strata_object	*stratum,
							  struct	date	current_date,
							  FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	fprintf(outfile,
		"%4d,%4d,%4d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf \n",
		current_date.day,
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patchID,
		stratum[0].ID,
		stratum[0].epv.proj_lai,
		stratum[0].evaporation*1000,
		stratum[0].APAR_direct,
		stratum[0].APAR_diffuse,
		stratum[0].sublimation*1000,
		stratum[0].transpiration_unsat_zone *1000.0
			+ stratum[0].transpiration_sat_zone *1000.0,
		stratum[0].ga*1000.0,
		stratum[0].gsurf,
		stratum[0].gs*1000.0,
		stratum[0].epv.psi,
		stratum[0].cdf.leaf_day_mr*1000.0,
		stratum[0].cdf.psn_to_cpool*1000.0,
		stratum[0].rain_stored*1000.0,
		stratum[0].snow_stored*1000.0,
		stratum[0].rootzone.S);
	return;
} /*end output_csv_canopy_stratum*/
