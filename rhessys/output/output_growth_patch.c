/*--------------------------------------------------------------*/
/* 																*/
/*					output_growth_patch						*/
/*																*/
/*	output_growth_patch - creates output_growth files objects.		*/
/*																*/
/*	NAME														*/
/*	output_growth_patch - output_growths current contents of a patch.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_growth_patch(										*/
/*					struct	patch_object	*patch,				*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	output_growths spatial structure according to commandline			*/
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

void	output_growth_patch(
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
	double atotalN, apredaytN;

	struct	canopy_strata_object 	*strata;
	apsn = 0.0;
	alai = 0.0;
	aleafc = 0.0;
	aresp = 0.0; asoilhr = 0.0;
	awoodc = 0.0;
	afrootc = 0.0;
	aheight = 0.0;
	atotalN = 0.0;
	apredaytN = 0.0;

	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
			strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];
			apsn += strata->cover_fraction * strata->cs.net_psn;
			aleafc += strata->cover_fraction * (strata->ns.leafn 
				+ strata->ns.leafn_store + strata->ns.leafn_transfer );

			afrootc += strata->cover_fraction
				* (strata->ns.frootn + strata->ns.frootn_store
				+ strata->ns.frootn_transfer);


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

			awoodc += strata->cover_fraction * (strata->ns.live_crootn
				+ strata->ns.live_stemn + strata->ns.dead_crootn
				+ strata->ns.dead_stemn + strata->ns.livecrootn_store
				+ strata->ns.livestemn_store + strata->ns.deadcrootn_store
				+ strata->ns.deadstemn_store + strata->ns.livecrootn_transfer
				+ strata->ns.livestemn_transfer + strata->ns.deadcrootn_transfer
				+ strata->ns.deadstemn_transfer
				+ strata->ns.cwdn + strata->ns.npool+ strata->ns.retransn);

			apredaytN += strata->cover_fraction * (strata->ns.preday_totaln);
			atotalN += strata->cover_fraction * (strata->ns.totaln);

			alai += strata->cover_fraction * (strata->epv.proj_lai) ;
			aheight += strata->cover_fraction * (strata->epv.height) ;
		}
	}
	check = fprintf(outfile,
		"%d %d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
		current_date.day,
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patch[0].ID,
		alai,
		aleafc+afrootc+awoodc,
		apsn*1000,
		aresp*1000,
		asoilhr*1000,
		patch[0].litter_ns.litr1n,
		patch[0].litter_ns.litr2n,
		patch[0].litter_ns.litr3n,
		patch[0].litter_ns.litr4n,
		patch[0].litter.rain_capacity*1000.0,
		patch[0].soil_ns.soil1n,
		patch[0].soil_ns.soil2n,
		patch[0].soil_ns.soil3n,
		patch[0].soil_ns.soil4n,
		patch[0].ndf.denitrif*1000.0,
		patch[0].soil_ns.leach*1000.0,
		patch[0].soil_ns.nitrate*1000.0,
		patch[0].soil_ns.sminn*1000.0,
		patch[0].streamflow_N,
		patch[0].surface_NO3,
		patch[0].surface_NH4,
		aheight,
		patch[0].ndf.sminn_to_npool*1000.0,
		patch[0].rootzone.depth*1000.0,
		patch[0].area);
	if (check <= 0) {
		fprintf(stdout, "\nWARNING: output_growth error has occured in output_growth_patch");
	}
	return;
} /*end output_growth_patch*/
