/*--------------------------------------------------------------*/
/* 																*/
/*					output_monthly_canopy_stratum						*/
/*																*/
/*	output_monthly_canopy_stratum - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_monthly_canopy_stratum - outputs current contents of a canopy_stratu*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_monthly_canopy_stratum(										*/
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

void	output_monthly_canopy_stratum( int basinID, int hillID,
									  int zoneID,
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
	if (stratum[0].acc_month.length == 0) stratum[0].acc_month.length = 1;
	/*--------------------------------------------------------------*/
	/*	output variables					*/
	/*--------------------------------------------------------------*/
	fprintf(outfile,"%4d %4d %d %d %d %d %d %lf \n",
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patchID,
		stratum[0].ID,
		(stratum[0].acc_month.leafc+stratum[0].acc_month.stemc) / stratum[0].acc_month.length);
	/*--------------------------------------------------------------*/
	/*	reset accumulator variables				*/
	/*--------------------------------------------------------------*/
	stratum[0].acc_month.lai = 0.0;
	stratum[0].acc_month.gpsn = 0.0;
	stratum[0].acc_month.resp = 0.0;
	stratum[0].acc_month.height = 0.0;
	stratum[0].acc_month.totalc = 0.0;
	stratum[0].acc_month.totaln = 0.0;
	stratum[0].acc_month.lwp = 0.0;
	stratum[0].acc_month.length = 0;
	stratum[0].acc_month.rootc = 0.0;
	stratum[0].acc_month.stemc = 0.0;
	stratum[0].acc_month.cpool = 0.0;
	stratum[0].acc_month.leafc = 0.0;
	stratum[0].acc_month.minNSC = 0.0;
	return;
} /*end output_monthly_canopy_stratum*/
