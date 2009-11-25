/*--------------------------------------------------------------*/
/* 																*/
/*					output_patch						*/
/*																*/
/*	output_patch - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_patch - outputs current contents of a patch.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_patch(										*/
/*					struct	patch_object	*patch,				*/
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

void	output_patch(
					 int basinID, int hillID, int zoneID,
					 struct	patch_object	*patch,
					 struct	date	current_date,
					 FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int check, c, layer;
	double alai, asub, apsn, litterS;

	if (patch[0].litter.rain_capacity > ZERO)
		litterS = patch[0].litter.rain_stored / patch[0].litter.rain_capacity;
	else
		litterS = 1.0;

	apsn = 0.0;
	asub = 0.0;
	alai = 0.0;
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
			apsn += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.net_psn ;
			asub += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].sublimation;
			alai += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.proj_lai;



		}
	}

	
	
	check = fprintf(outfile,"%d %d %d %d %lf\n",
		current_date.day,
		current_date.month,
		current_date.year,
		patch[0].ID,
		patch[0].snowpack.water_equivalent_depth*1000.0);
	if (check <= 0) {
		fprintf(stdout, "\nWARNING: output error has occured in output_patch");
	}
	return;
} /*end output_patch*/
