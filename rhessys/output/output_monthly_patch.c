/*--------------------------------------------------------------*/
/* 																*/
/*					output_monthly_patch						*/
/*																*/
/*	output_monthly_patch - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_monthly_patch - outputs current contents of a patch.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_monthly_patch(										*/
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

void	output_monthly_patch(
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
	struct	canopy_strata_object 	*strata;

	double aleafc, aleafn, afrootc, afrootn, awoodc, awoodn, alai, aheight, arootdepth, acanopy_N_store;
	aleafc = 0.0, aleafn = 0.0, afrootc = 0.0, afrootn = 0.0, awoodc = 0.0, awoodn = 0.0, alai = 0.0, aheight = 0.0, arootdepth = 0.0, acanopy_N_store = 0.0;

	if (patch[0].acc_month.length == 0) patch[0].acc_month.length = 1;

	if (patch[0].acc_month.leach > 0.0)
		patch[0].acc_month.leach = log(patch[0].acc_month.leach*1000.0*1000.0);

    	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
			strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];


			aleafc += strata->cover_fraction * (strata->cs.leafc + strata->cs.cpool
				+ strata->cs.leafc_store + strata->cs.leafc_transfer );

			aleafn += strata->cover_fraction * (strata->ns.leafn + strata->ns.npool
				+ strata->ns.leafn_store + strata->ns.leafn_transfer );

			afrootc += strata->cover_fraction
				* (strata->cs.frootc + strata->cs.frootc_store
				+ strata->cs.frootc_transfer);

			afrootn += strata->cover_fraction
				* (strata->ns.frootn + strata->ns.frootn_store
				+ strata->ns.frootn_transfer);

			awoodc += strata->cover_fraction * (strata->cs.live_crootc
				+ strata->cs.live_stemc + strata->cs.dead_crootc
				+ strata->cs.dead_stemc + strata->cs.livecrootc_store
				+ strata->cs.livestemc_store + strata->cs.deadcrootc_store
				+ strata->cs.deadstemc_store + strata->cs.livecrootc_transfer
				+ strata->cs.livestemc_transfer + strata->cs.deadcrootc_transfer
				+ strata->cs.deadstemc_transfer
				+ strata->cs.cwdc + strata->cs.cpool);

			awoodn += strata->cover_fraction * (strata->ns.live_crootn
				+ strata->ns.live_stemn + strata->ns.dead_crootn
				+ strata->ns.dead_stemn + strata->ns.livecrootn_store
				+ strata->ns.livestemn_store + strata->ns.deadcrootn_store
				+ strata->ns.deadstemn_store + strata->ns.livecrootn_transfer
				+ strata->ns.livestemn_transfer + strata->ns.deadcrootn_transfer
				+ strata->ns.deadstemn_transfer
				+ strata->ns.cwdn + strata->ns.npool + strata->ns.retransn);

			alai += strata->cover_fraction * (strata->epv.proj_lai) ;
			aheight += strata->cover_fraction * (strata->epv.height) ; // need
            arootdepth += strata->cover_fraction * (strata->rootzone.depth); //need

			acanopy_N_store += strata->cover_fraction * (strata->NO3_stored);

		}
	}


	check = fprintf(outfile,
		"%d %d %d %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
		current_date.month,
		current_date.year,
		basinID,
		hillID,
		zoneID,
		patch[0].ID,
		patch[0].family_ID,
		patch[0].acc_month.leach * 1000.0,
		patch[0].acc_month.denitrif * 1000.0,
		patch[0].acc_month.sm_deficit  / patch[0].acc_month.length * 1000.0,
		patch[0].acc_month.et * 1000.0,
		patch[0].acc_month.psn * 1000.0,
		patch[0].acc_month.DOC_loss * 1000.0,
		patch[0].acc_month.DON_loss * 1000.0,
		patch[0].acc_month.lai,
		patch[0].acc_month.nitrif * 1000.0,
		patch[0].acc_month.net_mineralized * 1000.0, // this is net_mineralized
		patch[0].acc_month.uptake * 1000.0,
		patch[0].acc_month.theta / patch[0].acc_month.length * 100.0, // this is root_zone.S
		patch[0].acc_month.snowpack * 1000.0 ,
		patch[0].area,
		patch[0].soil_ns.nitrate*1000, //+patch[0].surface_NO3,
		patch[0].soil_ns.sminn*1000,
        //from here is new extra output NREN 20220105
		
        patch[0].acc_month.trans *1000.0,
        patch[0].acc_month.soil_evap *1000.0,
        patch[0].acc_month.evap_surface *1000.0,
        patch[0].acc_month.streamflow *1000.0,
        patch[0].acc_month.baseflow *1000.0,
        patch[0].acc_month.recharge *1000.0,
        patch[0].acc_month.rz_drainage *1000.0,
        patch[0].acc_month.unsat_drain *1000.0,
        patch[0].acc_month.overland_flow *1000.0,
        patch[0].acc_month.theta2 / patch[0].acc_month.length, //this is theta I caclualated for denitrification
        patch[0].acc_month.streamNO3_from_surface *1000.0,
        patch[0].acc_month.streamNO3_from_sub *1000.0,
        patch[0].acc_month.streamflow_NO3 *1000.0,
        patch[0].acc_month.streamflow_NH4 *1000.0,
        patch[0].acc_month.nuptake *1000.0,
        patch[0].acc_month.potential_immob *1000.0,
        patch[0].acc_month.soil_resp *1000.0,
        patch[0].acc_month.water_rise_ratio,
        patch[0].acc_month.unsat_drain_ratio,
        patch[0].acc_month.fpi,
        patch[0].acc_month.mineralized *1000.0, // this is potential
        // pool new output
        aheight,
        arootdepth*1000.0,
        (awoodc + afrootc + aleafc), //plantc
        (awoodn + afrootn + aleafn),
        patch[0].rootzone.field_capacity*1000.0,
        acanopy_N_store*1000.0,
        patch[0].sat_NO3*1000,
        patch[0].sat_NH4*1000,
        (patch[0].surface_NO3 + patch[0].surface_NH4 + patch[0].surface_DON)*1000,
        (patch[0].soil_cs.soil1c + patch[0].soil_cs.soil2c + patch[0].soil_cs.soil3c + patch[0].soil_cs.soil4c),
        (patch[0].soil_ns.soil1n + patch[0].soil_ns.soil2n + patch[0].soil_ns.soil4n + patch[0].soil_ns.soil4n),
        (patch[0].litter_cs.litr1c + patch[0].litter_cs.litr2c + patch[0].litter_cs.litr3c + patch[0].litter_cs.litr4c),
        (patch[0].litter_ns.litr1n + patch[0].litter_ns.litr2n + patch[0].litter_ns.litr3n + patch[0].litter_ns.litr4n),
        patch[0].detention_store*1000,
        patch[0].unsat_storage*1000,
        patch[0].rz_storage*1000,
        patch[0].sat_deficit*1000,
        patch[0].field_capacity*1000,
		patch[0].acc_month.PET * 1000.0 //20230811
		);

	if (check <= 0) {
		fprintf(stdout,
			"\nWARNING: output error has occured in output_monthly_patch");
	}
	/*--------------------------------------------------------------*/
	/*	reset accumulator variables				*/
	/*--------------------------------------------------------------*/
	patch[0].acc_month.burn = 0.0;
	patch[0].acc_month.sm_deficit = 0.0;
	patch[0].acc_month.et = 0.0;
	patch[0].acc_month.psn = 0.0;
	patch[0].acc_month.lai = 0.0;
	patch[0].acc_month.length = 0;
	patch[0].acc_month.leach= 0.0;
	patch[0].acc_month.DOC_loss = 0.0;
	patch[0].acc_month.DON_loss = 0.0;
	patch[0].acc_month.denitrif= 0.0;
	patch[0].acc_month.nitrif= 0.0;
	patch[0].acc_month.mineralized = 0.0;
	patch[0].acc_month.uptake = 0.0;
	patch[0].acc_month.theta = 0.0;
	patch[0].acc_month.snowpack = 0.0;

	//new output NREN 2022
        patch[0].acc_month.trans =0.0;
        patch[0].acc_month.soil_evap = 0.0;
        patch[0].acc_month.evap_surface = 0.0;
        patch[0].acc_month.streamflow = 0.0;
        patch[0].acc_month.baseflow = 0.0;
        patch[0].acc_month.recharge = 0.0;
        patch[0].acc_month.rz_drainage = 0.0;
        patch[0].acc_month.unsat_drain = 0.0;
        patch[0].acc_month.overland_flow = 0.0;
        patch[0].acc_month.theta2 = 0.0; //this is theta I caclualated for denitrification
        patch[0].acc_month.streamNO3_from_surface = 0.0;
        patch[0].acc_month.streamNO3_from_sub = 0.0;
        patch[0].acc_month.streamflow_NO3 = 0.0;
        patch[0].acc_month.streamflow_NH4 = 0.0;
        patch[0].acc_month.nuptake = 0.0;
        patch[0].acc_month.potential_immob = 0.0;
        patch[0].acc_month.soil_resp = 0.0;
        patch[0].acc_month.water_rise_ratio = 0.0;
        patch[0].acc_month.unsat_drain_ratio = 0.0;
        patch[0].acc_month.fpi = 0.0;
        patch[0].acc_month.mineralized = 0.0; // this is potential
		patch[0].acc_month.PET = 0.0; //20230811

		return;
} /*end output_monthly_patch*/
