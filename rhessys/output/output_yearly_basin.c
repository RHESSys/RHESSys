/*--------------------------------------------------------------*/
/* 																*/
/*					output_yearly_basin						*/
/*																*/
/*	output_yearly_basin - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_yearly_basin - outputs current contents of a basin.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_yearly_basin(										*/
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

void	output_yearly_basin(
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


	if (basin->route_list->num_patches > 0)
		basin[0].acc_year.length /= basin->route_list->num_patches;
	if (basin[0].acc_year.length == 0) basin[0].acc_year.length = 1;


	check = fprintf(outfile,
		"%d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %lf\n",
		current_date.year,
		basin[0].ID,
		basin[0].acc_year.streamflow * 1000.0,
		basin[0].acc_year.stream_NO3 * 1000.0,
		basin[0].acc_year.denitrif * 1000.0,
		basin[0].acc_year.DOC_loss * 1000.0,
		basin[0].acc_year.DON_loss * 1000.0,
		basin[0].acc_year.et * 1000.0,
		basin[0].acc_year.psn * 1000.0,
		basin[0].acc_year.lai/ basin[0].acc_year.length,
		basin[0].acc_year.nitrif * 1000.0,
		basin[0].acc_year.mineralized * 1000.0,
		basin[0].acc_year.uptake * 1000.0,
		basin[0].acc_year.num_threshold,
		basin[0].acc_year.PET * 1000.0
		);
	if (check <= 0) {
		fprintf(stdout,
			"\nWARNING: output error has occured in output_yearly_basin");
	}
	/*--------------------------------------------------------------*/
	/*	reset accumulator variables				*/
	/*--------------------------------------------------------------*/
	basin[0].acc_year.streamflow = 0.0;
	basin[0].acc_year.stream_NO3 = 0.0;
	basin[0].acc_year.et = 0.0;
	basin[0].acc_year.PET = 0.0;
	basin[0].acc_year.psn = 0.0;
	basin[0].acc_year.lai = 0.0;
	basin[0].acc_year.length = 0;
	basin[0].acc_year.DOC_loss = 0.0;
	basin[0].acc_year.DON_loss = 0.0;
	basin[0].acc_year.denitrif= 0.0;
	basin[0].acc_year.nitrif= 0.0;
	basin[0].acc_year.num_threshold = 0;
	basin[0].acc_year.lai = 0.0;
	basin[0].acc_year.mineralized = 0.0;
	basin[0].acc_year.uptake = 0.0;
	return;
} /*end output_yearly_basin*/
