/*--------------------------------------------------------------*/
/* 																*/
/*					output_csv_growth_canopy_stratum						*/
/*																*/
/*	output_csv_growth_canopy_stratum - creates output_csv_growth files objects.		*/
/*																*/
/*	NAME														*/
/*	output_csv_growth_canopy_stratum - output_csv_growths  */
/*			current contents of a canopy_stratum.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_csv_growth_canopy_stratum(										*/
/*					struct	canopy_stratum_object	*canopy_stratum,				*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	output_csv_growths spatial structure according to commandline			*/
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

void	output_csv_growth_canopy_stratum( int basinID, int hillID, int zoneID,
									 int patchID,
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
		"%d,%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%lf \n",
		current_date.day,
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patchID,
		stratum[0].ID,
		stratum[0].epv.proj_lai,
		stratum[0].cs.leafc * 1000.0,
		stratum[0].cs.dead_leafc * 1000.0,
		stratum[0].cs.frootc * 1000.0,
		stratum[0].cs.live_stemc * 1000.0,
		stratum[0].cs.leafc_store * 1000.0,
		stratum[0].cs.dead_stemc * 1000.0,
		stratum[0].cs.live_crootc * 1000.0,
		stratum[0].cs.dead_crootc * 1000.0,
		stratum[0].cs.cwdc * 1000.0,
		stratum[0].cdf.total_mr*1000.0,
		stratum[0].cdf.total_gr*1000.0,
		stratum[0].cdf.psn_to_cpool * 1000.0,
		stratum[0].cs.age,
		stratum[0].rootzone.depth*1000.0);
	return;
} /*end output_csv_growth_canopy_stratum*/
