/*--------------------------------------------------------------*/
/* 																*/
/*					output_csv_yearly_canopy_stratum				*/
/*																*/
/*	output_csv_yearly_canopy_stratum - creates output_csv files objects.*/
/*																*/
/*	NAME														*/
/*	output_csv_yearly_canopy_stratum - output_csvs current contents of a canopy_stratum*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_csv_yearly_canopy_stratum(int bainsID, int hillID,*/
/*					int zoneID, int patchID,
/*					struct	canopy_stratum_object	*canopy_stratum,				*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	output_csvs spatial structure according to commandline			*/
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

void	output_csv_yearly_canopy_stratum( int basinID, int hillID,
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
	
	if (stratum[0].acc_year.length == 0)
		stratum[0].acc_year.length = 1;
	/*--------------------------------------------------------------*/
	/*	output_csv variables					*/
	/*--------------------------------------------------------------*/

	fprintf(outfile,"%4d,%d,%d,%d,%d,%d,%lf,%lf,%lf\n",
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patchID,
		stratum[0].ID,
		(stratum[0].acc_year.psn) / stratum[0].acc_year.length,
		(stratum[0].acc_year.lwp) / stratum[0].acc_year.length,
		1000.0*stratum[0].rootzone.depth);
	/*--------------------------------------------------------------*/
	/*	reset accumulator variables				*/
	/*--------------------------------------------------------------*/
	stratum[0].acc_year.psn = 0.0;
	stratum[0].acc_year.lwp = 0.0;
	stratum[0].acc_year.length = 0;
	return;
} /*end output_csv_yearly_canopy_stratum*/
