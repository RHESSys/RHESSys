/*--------------------------------------------------------------*/
/* 																*/
/*					output_yearly_hillslope						*/
/*																*/
/*	output_yearly_hillslope - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_yearly_hillslope - outputs current contents of a hillslope.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_yearly_hillslope(										*/
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

void	output_yearly_hillslope(	int basinID,
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


	if (hillslope[0].acc_year.length == 0) hillslope[0].acc_year.length = 1;


	check = fprintf(outfile,
		"%d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d\n",
		current_date.year-1,
		basinID,
		hillslope[0].ID,
		hillslope[0].acc_year.streamflow * 1000.0,
		hillslope[0].acc_year.stream_NO3 * 1000.0,
		hillslope[0].acc_year.denitrif * 1000.0,
		hillslope[0].acc_year.DOC_loss * 1000.0,
		hillslope[0].acc_year.DON_loss * 1000.0,
		hillslope[0].acc_year.et * 1000.0,
		hillslope[0].acc_year.psn * 1000.0,
		hillslope[0].acc_year.lai/ hillslope[0].acc_year.length,
		hillslope[0].acc_year.num_threshold,
		hillslope[0].acc_year.nitrif * 1000.0,
		hillslope[0].acc_year.mineralized * 1000.0,
		hillslope[0].acc_year.uptake * 1000.0,
		hillslope[0].area
		);
	if (check <= 0) {
		fprintf(stdout,
			"WARNING: output error has occured in output_yearly_hillslope \n");
	}
	/*--------------------------------------------------------------*/
	/*	reset accumulator variables				*/
	/*--------------------------------------------------------------*/
	hillslope[0].acc_year.streamflow = 0.0;
	hillslope[0].acc_year.stream_NO3 = 0.0;
	hillslope[0].acc_year.et = 0.0;
	hillslope[0].acc_year.psn = 0.0;
	hillslope[0].acc_year.lai = 0.0;
	hillslope[0].acc_year.length = 0;
	hillslope[0].acc_year.DOC_loss = 0.0;
	hillslope[0].acc_year.DON_loss = 0.0;
	hillslope[0].acc_year.denitrif= 0.0;
	hillslope[0].acc_year.nitrif= 0.0;
	hillslope[0].acc_year.num_threshold = 0;
	hillslope[0].acc_year.lai = 0.0;
	hillslope[0].acc_year.mineralized = 0.0;
	hillslope[0].acc_year.uptake = 0.0;
	return;
} /*end output_yearly_hillslope*/
