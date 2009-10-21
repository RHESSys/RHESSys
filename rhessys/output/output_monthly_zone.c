/*--------------------------------------------------------------*/
/* 																*/
/*					output_monthly_zone						*/
/*																*/
/*	output_monthly_zone - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_monthly_zone - outputs current contents of a zone.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_monthly_zone(										*/
/*					struct	zone_object	*zone,				*/
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

void	output_monthly_zone(	int basinID, int hillID,
							struct	zone_object	*zone,
							struct	date	current_date,
							FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*	output variables					*/
	/*--------------------------------------------------------------*/
	if (zone[0].acc_month.length == 0) zone[0].acc_month.length = 1;
	fprintf(outfile,"%4d %4d %3d %3d %3d %8.5f %8.5f %8.5f %8.3f %8.3f \n ",
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zone[0].ID,
		zone[0].acc_month.precip,
		zone[0].acc_month.K_direct / zone[0].acc_month.length,
		zone[0].acc_month.K_diffuse / zone[0].acc_month.length,
		zone[0].acc_month.tmax / zone[0].acc_month.length,
		zone[0].acc_month.tmin / zone[0].acc_month.length);
	/*--------------------------------------------------------------*/
	/*	reset accumulator variables				*/
	/*--------------------------------------------------------------*/
	zone[0].acc_month.K_direct = 0.0;
	zone[0].acc_month.K_diffuse = 0.0;
	zone[0].acc_month.precip = 0.0;
	zone[0].acc_month.tmax = 0.0;
	zone[0].acc_month.tmin = 0.0;
	zone[0].acc_month.length = 0;
	return;
} /*end output_monthly_zone*/
