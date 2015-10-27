/*--------------------------------------------------------------*/
/* 																*/
/*					output_csv_monthly_hillslope						*/
/*																*/
/*	output_csv_monthly_hillslope - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_csv_monthly_hillslope - outputs current contents of a hillslope.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_csv_monthly_hillslope(										*/
/*					struct	hillslope_object	*hillslope,				*/
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

void	output_csv_monthly_hillslope(	int basinID,
							 struct	hillslope_object	*hillslope,
							 struct	date	current_date,
							 FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int check;
	if (hillslope[0].acc_month.length == 0) hillslope[0].acc_month.length = 1;

	check = fprintf(outfile,
		"%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d\n",
		current_date.month,
		current_date.year,
		basinID,
		hillslope[0].ID,
		hillslope[0].acc_month.streamflow * 1000.0,
		hillslope[0].acc_month.stream_NO3 * 1000.0,
		hillslope[0].acc_month.snowpack/hillslope[0].acc_month.length,
		hillslope[0].acc_month.denitrif * 1000.0,
		hillslope[0].acc_month.DOC_loss * 1000.0,
		hillslope[0].acc_month.DON_loss * 1000.0,
		hillslope[0].acc_month.et * 1000.0,
		hillslope[0].acc_month.psn * 1000.0,
		hillslope[0].acc_month.lai/hillslope[0].acc_month.length ,
		hillslope[0].acc_month.nitrif * 1000.0,
		hillslope[0].acc_month.mineralized * 1000.0,
		hillslope[0].acc_month.uptake * 1000.0,
		hillslope[0].area
		);
	if (check <= 0) {
		fprintf(stdout,
			"WARNING: output error has occured in output_csv_monthly_hillslope \n");
	}
	/*--------------------------------------------------------------*/
	/*	reset accumulator variables				*/
	/*--------------------------------------------------------------*/
	hillslope[0].acc_month.snowpack = 0.0;
	hillslope[0].acc_month.streamflow = 0.0;
	hillslope[0].acc_month.stream_NO3 = 0.0;
	hillslope[0].acc_month.et = 0.0;
	hillslope[0].acc_month.psn = 0.0;
	hillslope[0].acc_month.length = 0;
	hillslope[0].acc_month.DOC_loss = 0.0;
	hillslope[0].acc_month.DON_loss = 0.0;
	hillslope[0].acc_month.denitrif= 0.0;
	hillslope[0].acc_month.lai = 0.0; 
	hillslope[0].acc_month.nitrif = 0.0; 
	hillslope[0].acc_month.mineralized = 0.0; 
	hillslope[0].acc_month.uptake = 0.0; 
	return;
} /*end output_csv_monthly_hillslope*/
