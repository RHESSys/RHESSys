/*--------------------------------------------------------------*/
/* 																*/
/*					output_yearly_growth_canopy_stratum						*/
/*																*/
/*	output_yearly_growth_canopy_stratum - creates output_growth files objects.		*/
/*																*/
/*	NAME														*/
/*	output_yearly_growth_canopy_stratum - output_growths  */
/*			current contents of a canopy_stratum.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_yearly_growth_canopy_stratum(int basinID,	*/
/*					int hillID, int zoneID,int patchID,         */
/*					struct	canopy_stratum_object	*canopy_stratum,				*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	output_growths spatial structure according to commandline			*/
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

void	output_yearly_growth_canopy_stratum( int basinID, int hillID, int zoneID,
			int patchID,
			struct	canopy_strata_object	*stratum,
			struct	date	current_date,
			FILE *outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/

  	fprintf(outfile,
       		 "%d %d %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f  \n",
		
        	current_date.year,
        	basinID,
        	hillID,
        	zoneID,
        	patchID,
        	stratum[0].ID,
        	(stratum[0].cs.leafc + stratum[0].cs.leafc_store + stratum[0].cs.leafc_transfer + stratum[0].cs.dead_leafc)
			* stratum[0].defaults[0][0].epc.proj_sla,
        	stratum[0].cs.leafc + stratum[0].cs.leafc_store + stratum[0].cs.leafc_transfer + stratum[0].cs.dead_leafc,
        	stratum[0].ns.leafn + stratum[0].ns.leafn_store + stratum[0].ns.leafn_transfer + stratum[0].ns.dead_leafn,
        	stratum[0].cs.frootc + stratum[0].cs.frootc_store + stratum[0].cs.frootc_transfer,
        	stratum[0].ns.frootn + stratum[0].ns.frootn_store + stratum[0].ns.frootn_transfer,
        	(stratum[0].cs.live_crootc + stratum[0].cs.livecrootc_store + stratum[0].cs.livecrootc_transfer +
        	stratum[0].cs.live_stemc + stratum[0].cs.livestemc_store + stratum[0].cs.livestemc_transfer +
        	stratum[0].cs.dead_crootc + stratum[0].cs.deadcrootc_store + stratum[0].cs.deadcrootc_transfer +
        	stratum[0].cs.dead_stemc + stratum[0].cs.deadstemc_store + stratum[0].cs.deadstemc_transfer) ,
        	(stratum[0].ns.live_crootn + stratum[0].ns.livecrootn_store + stratum[0].ns.livecrootn_transfer +
        	stratum[0].ns.live_stemn + stratum[0].ns.livestemn_store + stratum[0].ns.livestemn_transfer +
        	stratum[0].ns.dead_crootn + stratum[0].ns.deadcrootn_store + stratum[0].ns.deadcrootn_transfer +
        	stratum[0].ns.dead_stemn + stratum[0].ns.deadstemn_store + stratum[0].ns.deadstemn_transfer) ,
        	stratum[0].cs.cwdc,
        	stratum[0].ns.cwdn,
		stratum[0].acc_year.psn,
		stratum[0].acc_year.minNSC,
		stratum[0].cs.mortality_fract);

	stratum[0].acc_year.psn = 0.0;
	stratum[0].acc_year.minNSC = -999;

	return;
} /*end output_yearly_growth_canopy_stratum*/
