/*--------------------------------------------------------------*/
/* 																*/
/*					output_csv_yearly_growth_patch						*/
/*																*/
/*	output_csv_yearly_growth_patch - creates output_csv_growth files objects.*/
/*																*/
/*	NAME														*/
/*	output_csv_yearly_growth_patch - output_csv_growths current contents of a patch.*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_csv_yearly_growth_patch(int basinID, int hillID, int zoneID,	*/
/*					struct	patch_object	*patch,				*/
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

void	output_csv_yearly_growth_patch(
				int basinID, int hillID, int zoneID,
				struct	patch_object	*patch,
				struct	date	current_date,
				FILE *outfile)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/

     fprintf(outfile,
	"%4d,%4d,%4d,%4d,%3d,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
        current_date.year,
        basinID,
        hillID,
        zoneID,
        patch[0].ID,
	patch[0].litter_cs.litr1c + patch[0].litter_cs.litr2c + patch[0].litter_cs.litr3c + patch[0].litter_cs.litr4c,
        patch[0].soil_cs.soil1c + patch[0].soil_cs.soil2c + patch[0].soil_cs.soil3c + patch[0].soil_cs.soil4c,
	patch[0].litter_ns.litr1n + patch[0].litter_ns.litr2n + patch[0].litter_ns.litr3n + patch[0].litter_ns.litr4n,
        patch[0].soil_ns.soil1n + patch[0].soil_ns.soil2n + patch[0].soil_ns.soil3n + patch[0].soil_ns.soil4n, 
	patch[0].soil_ns.nitrate,
	patch[0].soil_ns.sminn, 1000.0*patch[0].rootzone.depth);

	return;
} /*end output_csv_yearly_growth_patch*/
