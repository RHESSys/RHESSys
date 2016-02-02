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
									 struct	command_line_object	*command_line,
									 FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	IF NOT SCM MODE - OUTPUT 							*/
	/*------------------------------------------------------*/
	if (command_line[0].scm_flag != 1){
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
	/*------------------------------------------------------*/
	/*	IF SCM MODE OUTPUT 							*/
	/*------------------------------------------------------*/
	} else {
		/*------------------------------------------------------*/
		/*	if stratum is algae, output algae variables		*/
		/*------------------------------------------------------*/
		if(stratum[0].defaults[0][0].epc.veg_type == ALGAE) {
			fprintf(outfile,
				"%d,%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf \n",
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
				stratum[0].rootzone.depth*1000.0,
				stratum[0].algae.Aout*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.Ain*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.chla_settle*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.Nuptake*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.Nrelease*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.Nrespire*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.Nsettle*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.Cfix*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.Crelease*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.Crespire*1000,  // SCM OUTPUT ... *1000 to convert kg to g
				stratum[0].algae.Csettle*1000);  // SCM OUTPUT ... *1000 to convert kg to g
				
		/*------------------------------------------------------*/
		/*	if patch is NOT algae, output algae variables as NaN	*/
		/*------------------------------------------------------*/
		} else {			
			fprintf(outfile,
				"%d,%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%lf,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s \n",
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
				stratum[0].rootzone.depth*1000.0,
				"NaN",  // SCM OUTPUT
				"NaN",  // SCM OUTPUT
				"NaN",  // SCM OUTPUT
				"NaN",  // SCM OUTPUT
				"NaN",  // SCM OUTPUT
				"NaN",  // SCM OUTPUT
				"NaN",  // SCM OUTPUT
				"NaN",  // SCM OUTPUT
				"NaN",  // SCM OUTPUT
				"NaN",  // SCM OUTPUT
				"NaN");  // SCM OUTPUT
		}
	}
	return;
} /*end output_csv_growth_canopy_stratum*/
