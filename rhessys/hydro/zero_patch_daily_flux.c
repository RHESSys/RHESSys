/*--------------------------------------------------------------*/
/* 								*/
/*		zero_patch_daily_flux				*/
/*								*/
/*								*/
/*	NAME							*/
/*		zero_patch_daily_flux
/*								*/
/*	SYNOPSIS						*/
/*	double	zero_patch_daily_flux(				*/
/*			struct cdayflux_patch_struct *,		*/
/*			struct ndayflux_patch_struct *,		*/
/*				)				*/
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	zeros all daily patch fluxes (carbon and nitrogen)	*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int zero_patch_daily_flux( struct patch_object *patch,
				struct cdayflux_patch_struct* cdf,
						  struct ndayflux_patch_struct* ndf)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok=1;

	/*--------------------------------------------------------------*/
	/* daily hydro  & N fluxes					*/
	/*--------------------------------------------------------------*/
	patch[0].return_flow = 0.0;
	patch[0].streamflow = 0.0;
	patch[0].streamflow_NO3 = 0.0;
	patch[0].streamflow_NH4 = 0.0;
	patch[0].base_flow = 0.0;
	patch[0].gw_drainage = 0.0;
	patch[0].snowpack.sublimation = 0.0;
	patch[0].evaporation_surf = 0.0;
	patch[0].Kup_direct = 0.0;
	patch[0].Kup_diffuse = 0.0;

	patch[0].abc_to_litrc = 0.0;
	patch[0].bgc_to_litrc = 0.0;  //NREN 20190927
	patch[0].flux_litterc_out = 0.0;
	patch[0].cap_rise_ratio = 0.0;
	patch[0].unsat_drain_ratio = 0.0;
	patch[0].water_drop_ratio = 0.0;
	patch[0].water_rise_ratio = 0.0;
	patch[0].theta = 0.0;
	patch[0].perc_sat = 0.0;

	/* fire effect */
	//patch[0].litterc_burned = 0.0;
	//patch[0].burn = 0.0;

	/*--------------------------------------------------------------*/
	/* daily carbon fluxes */
	/*--------------------------------------------------------------*/
	/* daily  DOC fluxes */
	cdf->do_litr1c_loss = 0.0;
	cdf->do_litr2c_loss = 0.0;
	cdf->do_litr3c_loss = 0.0;
	cdf->do_litr4c_loss = 0.0;
	cdf->do_soil1c_loss = 0.0;
	cdf->do_soil2c_loss = 0.0;
	cdf->do_soil3c_loss = 0.0;
	cdf->do_soil4c_loss = 0.0;
	cdf->total_DOC_loss = 0.0;
	/* daily turnover fluxes */
	cdf->leafc_to_litr1c = 0.0;
	cdf->leafc_to_litr2c = 0.0;
	cdf->leafc_to_litr3c = 0.0;
	cdf->leafc_to_litr4c = 0.0;
	cdf->frootc_to_litr1c = 0.0;
	cdf->frootc_to_litr2c = 0.0;
	cdf->frootc_to_litr3c = 0.0;
	cdf->frootc_to_litr4c = 0.0;
	/* decomposition and heterotroph respiration fluxes */
	cdf->litr1c_hr = 0.0;
	cdf->litr1c_to_soil1c = 0.0;
	cdf->litr2c_hr = 0.0;
	cdf->litr2c_to_soil2c = 0.0;
	cdf->litr3c_hr = 0.0;
	cdf->litr3c_to_litr2c = 0.0;
	cdf->litr4c_hr = 0.0;
	cdf->litr4c_to_soil3c = 0.0;
	cdf->soil1c_hr = 0.0;
	cdf->soil1c_to_soil2c = 0.0;
	cdf->soil2c_hr = 0.0;
	cdf->soil2c_to_soil3c = 0.0;
	cdf->soil3c_hr = 0.0;
	cdf->soil3c_to_soil4c = 0.0;
	cdf->soil4c_hr = 0.0;
	/* some daily flux for calculating the above ground litter proportion NREN 20190926 */
	cdf->stemc_to_litr1c = 0.0;
	cdf->cwdc_to_litr2c = 0.0;
	cdf->cwdc_to_litr3c = 0.0;
	cdf->cwdc_to_litr4c = 0.0;
	cdf->stemc_to_cwdc = 0.0;
	cdf->rootc_to_cwdc = 0.0;
	cdf->litterc_to_atmos = 0.0;
	cdf->litterc_to_soilc = 0.0;
	/*--------------------------------------------------------------*/
	/* daily nitrogen fluxes */
	/*--------------------------------------------------------------*/
	/* daily  DON fluxes */
	ndf->do_litr1n_loss = 0.0;
	ndf->do_litr2n_loss = 0.0;
	ndf->do_litr3n_loss = 0.0;
	ndf->do_litr4n_loss = 0.0;
	ndf->do_soil1n_loss = 0.0;
	ndf->do_soil2n_loss = 0.0;
	ndf->do_soil3n_loss = 0.0;
	ndf->do_soil4n_loss = 0.0;
	ndf->total_DON_loss = 0.0;
	/* daily turnover fluxes */
	ndf->leafn_to_litr1n = 0.0;
	ndf->leafn_to_litr2n = 0.0;
	ndf->leafn_to_litr3n = 0.0;
	ndf->leafn_to_litr4n = 0.0;
	ndf->frootn_to_litr1n = 0.0;
	ndf->frootn_to_litr2n = 0.0;
	ndf->frootn_to_litr3n = 0.0;
	ndf->frootn_to_litr4n = 0.0;
	/* daily N immobilization & mineralization fluxes */
	ndf->litr1n_to_soil1n = 0.0;
	ndf->sminn_to_soil1n_l1 = 0.0;
	ndf->litr2n_to_soil2n = 0.0;
	ndf->sminn_to_soil2n_l2 = 0.0;
	ndf->litr3n_to_litr2n = 0.0;
	ndf->sminn_to_soil2n_l3 = 0.0;
	ndf->litr4n_to_soil3n = 0.0;
	ndf->sminn_to_soil3n_l4 = 0.0;
	ndf->soil1n_to_soil2n = 0.0;
	ndf->sminn_to_soil2n_s1 = 0.0;
	ndf->soil2n_to_soil3n = 0.0;
	ndf->sminn_to_soil3n_s2 = 0.0;
	ndf->soil3n_to_soil4n = 0.0;
	ndf->sminn_to_soil4n_s3 = 0.0;
	ndf->soil4n_to_sminn = 0.0;
	ndf->sminn_to_npool = 0.0;
	ndf->plant_potential_ndemand = 0.0;
	/* deposition, fixation and leaching fluxes */
	ndf->nfix_to_sminn = 0.0;
	ndf->ndep_to_sminn = 0.0;
	/* denitrification fluxes */
	ndf->sminn_to_nvol = 0.0;
	ndf->denitrif = 0.0;
	ndf->sminn_to_nitrate = 0.0;
	ndf->nfix_to_sminn = 0.0;
	ndf->DON_to_gw = 0.0;
	ndf->N_to_gw = 0.0;
    ndf->Pot_denitrif_CO2 = 0.0;
	ndf->Pot_denitrif_SS = 0.0;

	/*-------------------------------
	ndf->sminn_to_nvol_l1s1 = 0.0;
		  ndf->sminn_to_nvol_l2s2 = 0.0;
		  ndf->sminn_to_nvol_l4s3 = 0.0;
		  ndf->sminn_to_nvol_s1s2 = 0.0;
		  ndf->sminn_to_nvol_s2s3 = 0.0;
		  ndf->sminn_to_nvol_s3s4 = 0.0;
		  ndf->sminn_to_nvol_s4 = 0.0;
	--------------------------------*/
	return (!ok);
}/* end zero_patch_daily_flux */





