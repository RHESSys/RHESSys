/*--------------------------------------------------------------*/
/* 																*/
/*					output_csv_growth_patch						*/
/*																*/
/*	output_csv_growth_patch - creates output_csv_growth files objects.		*/
/*																*/
/*	NAME														*/
/*	output_csv_growth_patch - output_csv_growths current contents of a patch.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_csv_growth_patch(										*/
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

void	output_csv_growth_patch(
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
	double apsn;
	double aheight;
	double alai, aresp, asoilhr;
	double aleafc, afrootc, awoodc;
	struct	canopy_strata_object 	*strata;
	apsn = 0.0;
	alai = 0.0;
	aleafc = 0.0;
	aresp = 0.0; asoilhr = 0.0;
	awoodc = 0.0;
	afrootc = 0.0;
	aheight = 0.0;

	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
			strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];
			apsn += strata->cover_fraction * strata->cs.net_psn;
			aleafc += strata->cover_fraction * (strata->cs.leafc_store
				+ strata->cs.leafc_store + strata->cs.leafc_transfer );
			afrootc += strata->cover_fraction
				* (strata->cs.frootc + strata->cs.frootc_store
				+ strata->cs.frootc_transfer);

			asoilhr += (
					patch[0].cdf.litr1c_hr + 
					patch[0].cdf.litr2c_hr + 
					patch[0].cdf.litr4c_hr + 
					patch[0].cdf.soil1c_hr + 
					patch[0].cdf.soil2c_hr + 
					patch[0].cdf.soil3c_hr + 
					patch[0].cdf.soil4c_hr);

			aresp += strata->cover_fraction
					* (strata->cdf.leaf_day_mr + strata->cdf.cpool_leaf_gr
					+ strata->cdf.leaf_night_mr +	strata->cdf.livestem_mr
					+ strata->cdf.cpool_livestem_gr + strata->cdf.livecroot_mr
					+ strata->cdf.cpool_livecroot_gr
					+ strata->cdf.cpool_deadcroot_gr
					+ strata->cdf.froot_mr + strata->cdf.cpool_froot_gr
					+ strata->cdf.cpool_to_gresp_store);

			awoodc += strata->cover_fraction * (strata->cs.live_crootc
				+ strata->cs.live_stemc + strata->cs.dead_crootc
				+ strata->cs.dead_stemc + strata->cs.livecrootc_store
				+ strata->cs.livestemc_store + strata->cs.deadcrootc_store
				+ strata->cs.deadstemc_store + strata->cs.livecrootc_transfer
				+ strata->cs.livestemc_transfer + strata->cs.deadcrootc_transfer
				+ strata->cs.deadstemc_transfer
				+ strata->cs.cwdc + strata->cs.cpool);
			alai += strata->cover_fraction * (strata->epv.proj_lai) ;
			aheight += strata->cover_fraction * (strata->epv.height) ;
		}
	}
	check = fprintf(outfile,
		"%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d \n",
		current_date.day,
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patch[0].ID,
		alai,
		aleafc+afrootc+awoodc,
		apsn*1000.0,
		aresp*1000.0,
		asoilhr*1000.0,
		patch[0].litter_cs.litr1c,
		patch[0].litter_cs.litr2c,
		patch[0].litter_cs.litr3c,
		patch[0].litter_cs.litr4c,
		patch[0].litter.rain_capacity*1000.0,
		patch[0].soil_cs.soil1c,
		patch[0].soil_cs.soil2c,
		patch[0].soil_cs.soil3c,
		patch[0].soil_cs.soil4c,
		patch[0].ndf.denitrif*1000.0,
		(patch[0].soil_ns.Qin - patch[0].soil_ns.Qout)*1000.0,
		patch[0].soil_ns.nitrate*1000.0,
		patch[0].streamflow_N,
		patch[0].surface_NO3,
		aheight,
		patch[0].ndf.sminn_to_npool*1000.0,
		patch[0].rootzone.depth*1000.0,
		patch[0].area);
	if (check <= 0) {
		fprintf(stdout, "WARNING: output_csv_growth error has occured in output_csv_growth_patch \n");
	}
	return;
} /*end output_csv_growth_patch*/
