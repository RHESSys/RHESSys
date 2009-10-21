/*--------------------------------------------------------------*/
/* 																*/
/*					output_csv_growth_zone						*/
/*																*/
/*	output_csv_growth_zone - creates output_csv files objects.		*/
/*																*/
/*	NAME														*/
/*	output_csv_growth_zone - output_csvs current contents of a zone.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_csv_growth_zone(										*/
/*					struct	zone_object	*zone,				*/
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

void	output_csv_growth_zone(	int basinID, int hillID,
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
	
	fprintf(outfile,
		"%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f\n,",
		current_date.day,
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zone[0].ID,
		zone[0].rain * 1000.0,
		zone[0].snow * 1000.0,
		zone[0].metv.tday,
		zone[0].metv.tavg,
		zone[0].metv.vpd,
		zone[0].Kdown_direct,
		zone[0].Kdown_diffuse,
		zone[0].PAR_direct,
		zone[0].PAR_diffuse);
	return;
} /*end output_csv_growth_zone*/
