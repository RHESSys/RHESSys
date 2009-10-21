/*--------------------------------------------------------------*/
/* 																*/
/*					output_csv_growth_hillslope						*/
/*																*/
/*	output_csv_growth_hillslope - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_csv_growth_hillslope - outputs current contents of a hillslope.	 */
/*																*/
/*	SYNOPSIS													*/
/*	void	output_csv_growth_hillslope(										*/
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

void	output_csv_growth_hillslope(
								int basinID,
								struct	hillslope_object	*hillslope,
								struct	date	date,
								FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int z,p,c;
	int  layer;
	double apsn;
	double alai;
	double aarea;
	struct	patch_object  *patch;
	struct	zone_object	*zone;
	/*--------------------------------------------------------------*/
	/*	Initialize Accumlating variables.								*/
	/*--------------------------------------------------------------*/
	alai = 0.0 ;
	apsn = 0.0 ;
	aarea =  0.0 ;
	for (z=0; z< hillslope[0].num_zones; z++){
		zone = hillslope[0].zones[z];
		for (p=0; p< zone[0].num_patches; p++){
			patch = zone[0].patches[p];
			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
				for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
					alai += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
						* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.proj_lai
						* patch[0].area;
					apsn += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
						* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.net_psn
						* patch[0].area;
				}
			}
			aarea +=  patch[0].area;
		}
	}
	apsn /= aarea ;
	alai /= aarea ;
	fprintf(outfile,"%d,%d,%d,%d,%d,%lf,%lf\n",
		date.day,
		date.month,
		date.year,
		basinID,
		hillslope[0].ID,
		alai,
		apsn * 1000.0,
		aarea);
	return;
} /*end output_csv_growth_hillslope*/
