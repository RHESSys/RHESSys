/*--------------------------------------------------------------*/
/* 																*/
/*					output_hillslope						*/
/*																*/
/*	output_hillslope - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_hillslope - outputs current contents of a hillslope.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_hillslope(										*/
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

void	output_hillslope(				int basinID,
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
	int layer;
	double arain_throughfall;
	double asnow_throughfall;
	double asat_deficit_z;
	double asat_deficit;
	double aunsat_storage;
	double aunsat_drainage;
	double acap_rise;
	double areturn_flow;
	double aevaporation;
	double asnowpack;
	double atranspiration;
	double astreamflow;
	double abase_flow;
	double apsn, alai;
	double u20, au20; 
	double aarea;
	struct	patch_object  *patch;
	struct	zone_object	*zone;

	/*--------------------------------------------------------------*/
	/*	Initialize Accumlating variables.								*/
	/*--------------------------------------------------------------*/
	arain_throughfall =  0.0;
	asnow_throughfall = 0.0 ;
	asat_deficit_z = 0.0 ;
	asat_deficit = 0.0 ;
	aunsat_storage = 0.0 ;
	aunsat_drainage = 0.0 ;
	acap_rise = 0.0 ;
	areturn_flow = 0.0 ;
	au20 = 0.0;
	aevaporation = 0.0 ;
	asnowpack = 0.0  ;
	atranspiration = 0.0  ;
	astreamflow = 0.0;
	abase_flow = 0.0;
	apsn = 0.0 ;
	alai = 0.0;
	aarea =  0.0 ;
	for (z=0; z<hillslope[0].num_zones; z++){
		zone = hillslope[0].zones[z];
		for (p=0; p< zone[0].num_patches; p++){
			patch = zone[0].patches[p];
			arain_throughfall += patch[0].rain_throughfall * patch[0].area;
			asnow_throughfall += patch[0].snow_throughfall * patch[0].area;
			asat_deficit_z += patch[0].sat_deficit_z * patch[0].area;
			asat_deficit += patch[0].sat_deficit * patch[0].area;

			/* determine actual amount in upper 20cm */
			if (patch[0].sat_deficit_z > 0.020)
				u20 = patch[0].unsat_storage * 0.020/patch[0].sat_deficit_z;
			else
				u20 = patch[0].unsat_storage + (0.020 - patch[0].sat_deficit_z)*
						patch[0].soil_defaults[0][0].porosity_0;         
			au20 += u20 * patch[0].area;
			aunsat_storage += patch[0].unsat_storage * patch[0].area;
			aunsat_drainage += patch[0].unsat_drainage * patch[0].area;
			acap_rise += patch[0].cap_rise * patch[0].area;
			abase_flow += patch[0].base_flow * patch[0].area;
			areturn_flow += patch[0].return_flow * patch[0].area;
			aevaporation += patch[0].evaporation * patch[0].area;
			aarea += patch[0].area;
			asnowpack += patch[0].snowpack.water_equivalent_depth  *  patch[0].area;
			atranspiration += (patch[0].transpiration_sat_zone
				+ patch[0].transpiration_unsat_zone)  *  patch[0].area;
			if (patch[0].drainage_type == STREAM)  {
				astreamflow += patch[0].streamflow*patch[0].area;
			}
			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
				for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
					apsn += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
						* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.net_psn
						* patch[0].area;
					alai += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
						* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.proj_lai
						* patch[0].area;
				}
			}
		}
	}

	arain_throughfall /=  aarea;
	asnow_throughfall /= aarea ;
	asat_deficit_z /= aarea ;
	asat_deficit /= aarea ;
	aunsat_storage /= aarea ;
	aunsat_drainage /= aarea ;
	acap_rise /= aarea ;
	areturn_flow /= aarea ;
	aevaporation /= aarea ;
	abase_flow /= aarea;
	asnowpack /= aarea  ;
	atranspiration /= aarea  ;
	astreamflow /= aarea;
	apsn /= aarea ;
	alai /= aarea ;
	au20 /= aarea;

	abase_flow += hillslope[0].base_flow;


	fprintf(outfile,"%d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
		date.day,
		date.month,
		date.year,
		basinID,
		hillslope[0].ID,
		arain_throughfall * 1000.0,
		asnow_throughfall * 1000.0,
		asat_deficit_z * 1000.0,
		asat_deficit * 1000.0,
		aunsat_storage * 1000.0,
		aunsat_drainage * 1000.0,
		acap_rise * 1000.0,
		aevaporation * 1000.0,
		asnowpack * 1000.0,
		atranspiration * 1000.0,
		abase_flow * 1000.0,
		areturn_flow * 1000.0,
		(astreamflow + abase_flow)* 1000.0,
		apsn,
		alai,
		hillslope[0].gw.Qout *1000.0,
		hillslope[0].gw.storage *1000.0,
		hillslope[0].area
		);
	return;
} /*end output_hillslope*/
