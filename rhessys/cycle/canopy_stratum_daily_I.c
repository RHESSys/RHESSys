/*--------------------------------------------------------------*/
/*								*/
/*		canopy_stratum.I				*/
/*								*/
/*	NAME							*/
/*								*/
/*	SYNOPSIS						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/* March 12, 1997	C. Tague 	*/
/* added seasonal leafon/off adjustment routine  */
/*								*/
/*	Sep 15 97 RAF						*/
/*	Took out call to compute lwp predawn and set it		*/
/*	to default at LWP_min_spring for now.			*/
/*	Due to modification of porosity with depth.		*/ 
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void	canopy_stratum_daily_I(
							   struct	world_object	*world,
							   struct	basin_object	*basin,
							   struct	hillslope_object	*hillslope, 
							   struct	zone_object		*zone,
							   struct	patch_object	*patch,
							   struct 	canopy_strata_object 	*stratum,
							   struct 	command_line_object	*command_line,
							   struct	tec_entry		*event,
							   struct 	date 			current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	double	compute_lwp_predawn(
		int,
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);
	
	
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/

	if (patch[0].sat_deficit < ZERO)
		stratum[0].rootzone.S = 1.0;

	else if (patch[0].sat_deficit_z > patch[0].rootzone.depth)  	
		stratum[0].rootzone.S = min(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);	
	
	else  
		stratum[0].rootzone.S = min((patch[0].rz_storage + patch[0].rootzone.potential_sat 
			- patch[0].sat_deficit)
			/ patch[0].rootzone.potential_sat, 1.0);							
	 /*--------------------------------------------------------------*/
	/*      Compute canopy predawn LWP        			*/
	/*	Currently defaulted at non-stressed value.		*/
	/*--------------------------------------------------------------*/
	stratum[0].epv.psi =	compute_lwp_predawn(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].theta_psi_curve,
		patch[0].Tsoil,
		stratum[0].defaults[0][0].epc.psi_open,
		stratum[0].defaults[0][0].epc.psi_close,
		patch[0].soil_defaults[0][0].psi_air_entry,
		patch[0].soil_defaults[0][0].pore_size_index,
		patch[0].soil_defaults[0][0].p3,
		patch[0].soil_defaults[0][0].p4,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		stratum[0].rootzone.S);

	stratum[0].epv.psi_ravg = (stratum[0].defaults[0][0].epc.gs_ravg_days-1)/(stratum[0].defaults[0][0].epc.gs_ravg_days)* stratum[0].epv.psi_ravg + 
	 			1.0/(stratum[0].defaults[0][0].epc.gs_ravg_days) * stratum[0].epv.psi;

	if ( command_line[0].verbose_flag > 1 )
		printf(" %8f", stratum[0].epv.psi);

	/*--------------------------------------------------------------*/
	/*	keep track of water stress days for annual allocation   */
	/*--------------------------------------------------------------*/
	if ( (command_line[0].grow_flag > 0) &&
		(stratum[0].epv.psi <= stratum[0].defaults[0][0].epc.psi_close )) {
		if (command_line[0].verbose_flag == -2)
			printf("\n%4ld %4ld %4ld -111.1 ws day %lf %lf",
			current_date.day, current_date.month, current_date.year,
			stratum[0].epv.psi ,stratum[0].defaults[0][0].epc.psi_close);
		stratum[0].epv.wstress_days += 1;
	}


	/*--------------------------------------------------------------*/
        /*  compute temperature for acclimation - currently just a running mean of air temperature */
	/*--------------------------------------------------------------*/
	if (stratum[0].defaults[0][0].epc.Tacclim_days > 0)
       	 stratum[0].cs.Tacc = stratum[0].cs.Tacc*(stratum[0].defaults[0][0].epc.Tacclim_days-1.0)/
				(stratum[0].defaults[0][0].epc.Tacclim_days) + 
                              zone[0].metv.tavg * 1.0/stratum[0].defaults[0][0].epc.Tacclim_days;
	else
		stratum[0].cs.Tacc = zone[0].metv.tavg;


	return;
} /*end canopy_stratum_I.c*/

