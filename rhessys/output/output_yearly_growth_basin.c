/*--------------------------------------------------------------*/
/* 																*/
/*					output_yearly_growth_basin						*/
/*																*/
/*	output_yearly_growth_basin - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_yearly_growth_basin - outputs current contents of a basin.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_yearly_growth_basin(										*/
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

void	output_yearly_growth_basin(
								   struct	basin_object	*basin,
								   struct	date	date,
								   FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int h,z,p,c;
	int  layer;
	double amort, ard, agpsn, aresp;
	double anewc, asoilhr;
	double aarea, hill_area;
	double astreamflow_N;
	double adenitrif;
	struct	patch_object  *patch;
	struct	zone_object	*zone;
	struct hillslope_object *hillslope;
	struct  canopy_strata_object    *strata;
	/*--------------------------------------------------------------*/
	/*	Initialize Accumlating variables.								*/
	/*--------------------------------------------------------------*/
	amort = 0.0; ard=0.0; agpsn = 0.0; aresp=0.0;
	aarea =  0.0 ;
	anewc = 0.0;
	asoilhr = 0.0;
	astreamflow_N = 0.0; adenitrif = 0.0;
	for (h=0; h < basin[0].num_hillslopes; h++){
		hillslope = basin[0].hillslopes[h];
		hill_area = 0.0;
		for (z=0; z< hillslope[0].num_zones; z++){
			zone = hillslope[0].zones[z];
			for (p=0; p< zone[0].num_patches; p++){
				patch = zone[0].patches[p];
				adenitrif += (patch[0].soil_ns.nvolatilized_snk) * patch[0].area;

				asoilhr += (
					patch[0].litter_cs.litr1c_hr_snk + 
					patch[0].litter_cs.litr2c_hr_snk + 
					patch[0].litter_cs.litr4c_hr_snk + 
					patch[0].soil_cs.soil1c_hr_snk + 
					patch[0].soil_cs.soil2c_hr_snk + 
					patch[0].soil_cs.soil3c_hr_snk + 
					patch[0].soil_cs.soil4c_hr_snk) * patch[0].area;
				patch[0].litter_cs.litr1c_hr_snk  = 0.0; 
				patch[0].litter_cs.litr2c_hr_snk  = 0.0; 
				patch[0].litter_cs.litr4c_hr_snk  = 0.0;
				patch[0].soil_cs.soil1c_hr_snk  = 0.0; 
				patch[0].soil_cs.soil2c_hr_snk  = 0.0; 
				patch[0].soil_cs.soil3c_hr_snk  = 0.0; 
				patch[0].soil_cs.soil4c_hr_snk  = 0.0; 

				for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
						strata =
							patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];
						agpsn += strata->cover_fraction
							* strata->cs.gpsn_src
							* patch[0].area;
						amort += strata->cover_fraction
							* strata->cs.mortality_fract
							* patch[0].area;
						ard += strata->cover_fraction
							* strata->rootzone.depth
							* patch[0].area;
						aresp += strata->cover_fraction
							* (strata->cs.leaf_mr_snk + strata->cs.leaf_gr_snk
							+ strata->cs.livestem_mr_snk+strata->cs.livestem_gr_snk
							+strata->cs.livecroot_mr_snk+strata->cs.livecroot_gr_snk
							+ strata->cs.deadstem_gr_snk+strata->cs.deadcroot_gr_snk
							+ strata->cs.froot_mr_snk + strata->cs.froot_gr_snk)
							* patch[0].area;
						anewc += strata->cover_fraction
							* (strata->cs.leafc_store + strata->cs.leafc_transfer
							+ strata->cs.frootc_store + strata->cs.frootc_transfer
							+ strata->cs.livecrootc_store
							+ strata->cs.livecrootc_transfer
							+ strata->cs.deadcrootc_store
							+ strata->cs.deadcrootc_transfer
							+ strata->cs.livestemc_store
							+ strata->cs.livestemc_transfer
							+ strata->cs.deadstemc_store
							+ strata->cs.deadstemc_transfer )
							* patch[0].area;
					}
				}
				aarea +=  patch[0].area;
				hill_area += patch[0].area;
			}
		}
	}
	amort /= aarea ;
	agpsn /= aarea ;
	aresp /= aarea ;
	anewc /= aarea;
	asoilhr /= aarea;
	astreamflow_N /= aarea;
	adenitrif /= aarea;
	ard /= aarea;
	fprintf(outfile,"%d %d %lf %lf %lf %lf %lf %lf %lf %lf \n",
		date.year,
		basin[0].ID,
		agpsn,
		aresp,
		anewc,
		asoilhr,
		basin[0].acc_year.stream_NO3*1000.0,
		adenitrif*1000.0,
		ard*1000.0,
		amort*100.0
		);
	return;
} /*end output_yearly_growth_basin*/
