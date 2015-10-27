/*--------------------------------------------------------------*/
/* 																*/
/*					output_monthly_basin						*/
/*																*/
/*	output_monthly_basin - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_monthly_basin - outputs current contents of a basin.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_monthly_basin(										*/
/*					struct	basin_object	*basin,				*/
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

void	output_monthly_basin(
							 struct	basin_object	*basin,
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
	if (basin[0].acc_month.length == 0) basin[0].acc_month.length = 1;
	
	if (basin->route_list->num_patches > 0)
		basin[0].acc_month.length /= (basin->route_list->num_patches);
	check = fprintf(outfile,
		"%d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
		current_date.month,
		current_date.year,
		basin[0].ID,
		basin[0].acc_month.streamflow * 1000.0,
		basin[0].acc_month.stream_NO3 * 1000.0,
		basin[0].acc_month.denitrif * 1000.0,
		basin[0].acc_month.DOC_loss * 1000.0,
		basin[0].acc_month.DON_loss * 1000.0,
		basin[0].acc_month.et * 1000.0,
		basin[0].acc_month.psn * 1000.0,
		basin[0].acc_month.lai/basin[0].acc_month.length ,
		basin[0].acc_month.nitrif * 1000.0,
		basin[0].acc_month.mineralized * 1000.0,
		basin[0].acc_month.uptake * 1000.0
		);
	if (check <= 0) {
		fprintf(stdout,
			"WARNING: output error has occured in output_monthly_basin \n");
	}
	/*--------------------------------------------------------------*/
	/*	reset accumulator variables				*/
	/*--------------------------------------------------------------*/
	basin[0].acc_month.streamflow = 0.0;
	basin[0].acc_month.stream_NO3 = 0.0;
	basin[0].acc_month.et = 0.0;
	basin[0].acc_month.psn = 0.0;
	basin[0].acc_month.length = 0;
	basin[0].acc_month.DOC_loss = 0.0;
	basin[0].acc_month.DON_loss = 0.0;
	basin[0].acc_month.denitrif= 0.0;
	basin[0].acc_month.lai = 0.0; 
	basin[0].acc_month.nitrif = 0.0; 
	basin[0].acc_month.mineralized = 0.0; 
	basin[0].acc_month.uptake = 0.0; 
	return;
} /*end output_monthly_basin*/
