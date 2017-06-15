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
	int check, c, layer;
	double alai, asub, apsn, litterS, aheight;

	if (patch[0].litter.rain_capacity > ZERO)
		litterS = patch[0].litter.rain_stored / patch[0].litter.rain_capacity;
	else
		litterS = 1.0;

	apsn = 0.0;
	asub = 0.0;
	alai = 0.0;
	aheight = 0.0;
	for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
		for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
			apsn += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.net_psn ;
			asub += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].sublimation;
			alai += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.proj_lai;
			aheight += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
				* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.height;


		}
	}

	check = fprintf(outfile,"%d %d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
					current_date.day,
					current_date.month,
					current_date.year,
					basinID,
					hillID,
					zoneID,
					patch[0].ID,
					patch[0].rain_throughfall*1000.0,
					patch[0].detention_store*1000.0,
					patch[0].sat_deficit_z*1000,
					patch[0].sat_deficit*1000,
					patch[0].rz_storage*1000,
					patch[0].rootzone.potential_sat*1000,
					patch[0].rootzone.field_capacity*1000,
					patch[0].wilting_point*1000,
					patch[0].unsat_storage*1000,
					patch[0].rz_drainage*1000,
					patch[0].unsat_drainage*1000,
					(patch[0].snowpack.sublimation + asub)*1000,
					patch[0].return_flow*1000.0,
					patch[0].evaporation*1000.0,
					patch[0].evaporation_surf*1000.0,
					(patch[0].exfiltration_sat_zone + patch[0].exfiltration_unsat_zone) * 1000.0, 
					patch[0].snowpack.water_equivalent_depth*1000.0,
					patch[0].snow_melt*1000.0,
					(patch[0].transpiration_sat_zone*1000.0), (patch[0].transpiration_unsat_zone)*1000.0,
					patch[0].Qin_total * 1000.0,
					patch[0].Qout_total * 1000.0,
					apsn * 1000.0,
					patch[0].rootzone.S,
					patch[0].rootzone.depth*1000.0,
					patch[0].litter.rain_stored*1000.0,
					litterS,
					patch[0].area, (patch[0].PET)*1000.0, alai, 
					patch[0].base_flow*1000.0, 
					patch[0].streamflow*1000.0, 1000.0*(zone[0].rain+zone[0].snow), patch[0].recharge, 
					patch[0].Kdown_direct, patch[0].Kdown_diffuse, 
					patch[0].Kup_direct, patch[0].Kup_diffuse, patch[0].Lup,
					patch[0].Kdown_direct_subcanopy, patch[0].Kdown_diffuse_subcanopy, patch[0].Ldown_subcanopy, 
					patch[0].Kstar_canopy,
					patch[0].snowpack.Kstar_direct, patch[0].snowpack.Kstar_diffuse,
					patch[0].Lstar_canopy, patch[0].Lstar_snow, patch[0].Lstar_soil, 
					patch[0].wind, patch[0].windsnow, zone[0].wind, 
					patch[0].ga*1000.0, patch[0].gasnow*1000.0,
					patch[0].trans_reduc_perc,
					patch[0].field_capacity,
					patch[0].overland_flow*1000.0,
					aheight,
					patch[0].ustar,
					patch[0].snowpack.K_reflectance,
					patch[0].Kstar_soil,
					patch[0].Kdown_direct_bare, patch[0].Kdown_diffuse_bare,
					patch[0].exfiltration_unsat_zone,
					patch[0].snowpack.Rnet/86.4,
					patch[0].snowpack.Q_LE/86.4,
					patch[0].snowpack.Q_H/86.4,
					patch[0].snowpack.Q_rain/86.4,
					patch[0].snowpack.Q_melt/86.4,
					patch[0].LE_canopy,
					patch[0].snowpack.energy_deficit,
					patch[0].snowpack.surface_age);
	

	if (check <= 0) {
		fprintf(stdout, "\nWARNING: output error has occured in output_patch, file");
	}
	return;
} /*end output_patch*/
