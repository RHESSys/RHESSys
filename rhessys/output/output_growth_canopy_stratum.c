/*--------------------------------------------------------------*/
/* 																*/
/*					output_growth_canopy_stratum						*/
/*																*/
/*	output_growth_canopy_stratum - creates output_growth files objects.		*/
/*																*/
/*	NAME														*/
/*	output_growth_canopy_stratum - output_growths  */
/*			current contents of a canopy_stratum.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_growth_canopy_stratum(										*/
/*					struct	canopy_stratum_object	*canopy_stratum,				*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	output_growths spatial structure according to commandline			*/
/*	specificatiocs to specific files							*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_growth_canopy_stratum( int basinID, int hillID, int zoneID,
									 int patchID,
									 struct	canopy_strata_object	*stratum,
									 struct	date	current_date,
									 FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declaratiocs.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

	fprintf(outfile,
		"%d %d %d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %lf %d %d %lf\n",
		current_date.day,
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patchID,
		stratum[0].ID,
		stratum[0].epv.proj_lai,
		(stratum[0].cs.leafc + stratum[0].cs.leafc_store + stratum[0].cs.leafc_transfer)* 1000.0,
		stratum[0].cs.cpool * 1000.0,
		stratum[0].cs.dead_leafc * 1000.0,
		(stratum[0].cs.frootc + stratum[0].cs.frootc_store + stratum[0].cs.frootc_transfer)* 1000.0,
		(stratum[0].cs.live_stemc + stratum[0].cs.livestemc_store + stratum[0].cs.livestemc_transfer)* 1000.0,
		stratum[0].cs.leafc_store * 1000.0,
		(stratum[0].cs.dead_stemc + stratum[0].cs.deadstemc_store + stratum[0].cs.deadstemc_transfer)* 1000.0,
		(stratum[0].cs.live_crootc + stratum[0].cs.livecrootc_store + stratum[0].cs.livecrootc_transfer)* 1000.0,
		(stratum[0].cs.dead_crootc + stratum[0].cs.deadcrootc_store + stratum[0].cs.deadcrootc_transfer)* 1000.0,
		stratum[0].cs.cwdc * 1000.0,
		stratum[0].cdf.total_mr*1000.0,
		stratum[0].cdf.total_gr*1000.0,
		stratum[0].cdf.psn_to_cpool * 1000.0,
		stratum[0].cs.age,
		stratum[0].rootzone.depth*1000.0,
		stratum[0].phen.gwseasonday,
		stratum[0].phen.lfseasonday,
		stratum[0].phen.gsi);
	return;
} /*end output_growth_canopy_stratum*/
