/*--------------------------------------------------------------*/
/* 																*/
/*					output_basin						*/
/*																*/
/*	output_basin - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_basin - outputs current contents of a basin.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_basin( int routing_flag,										*/	
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

void	output_basin(			int routing_flag,
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
	int var_flag, layer;
	double astreamflow_N;
	double arain_throughfall;
	double asnow_throughfall;
	double alitter_store;
	double asat_deficit_z;
	double asat_deficit;
	double aunsat_storage, arz_storage;
	double aunsat_drainage, arz_drainage;
	double acap_rise;
	double areturn_flow;
	double aevaporation;
	double asnowpack, aperc_snow;
	double atranspiration;
	double astreamflow;
	double asublimation;
	double asat_area, adetention_store;
	double apsn, alai, acrain;
	double abase_flow, hbase_flow,  hstreamflow_N;
	double	aacctrans, var_acctrans, var_trans;
	double aPET, adC13, amortality_fract, apcp;
	double	hgw, hgwN;
	double atmin, atmax, asnow;
	double	hgwQout, hgwNout;
	double aarea, hill_area, zone_area, basin_area;
	struct	patch_object  *patch;
	struct	zone_object	*zone;
	struct hillslope_object *hillslope;
	/*--------------------------------------------------------------*/
	/*	Initialize Accumlating variables.								*/
	/*--------------------------------------------------------------*/
	arain_throughfall =  0.0;
	asnow_throughfall = 0.0 ;
	acrain = 0.0;
	asat_deficit_z = 0.0 ;
	asat_deficit = 0.0 ;
	arz_storage = 0.0;	
	aunsat_storage = 0.0 ;
	arz_drainage = 0.0;		
	aunsat_drainage = 0.0 ;
	acap_rise = 0.0 ;
	areturn_flow = 0.0 ;
	aevaporation = 0.0 ;
	asat_area = 0.0;
	asnowpack = 0.0  ;
	asublimation = 0.0  ;
	aperc_snow = 0.0  ;
	alitter_store = 0.0  ;
	atranspiration = 0.0  ;
	astreamflow = 0.0;
	apsn = 0.0 ;
	aPET = 0.0;
	aarea =  0.0 ;
	abase_flow = 0.0;
	hbase_flow = 0.0;
	hstreamflow_N = 0.0;
	hgwQout = 0.0;
	hgwNout = 0.0;
	hgw = 0.0;
	hgwN = 0.0;
	alai = 0.0;
	adetention_store = 0.0;
	astreamflow_N = 0.0;
	aacctrans = 0.0; 
	basin_area = 0.0;
	zone_area = 0.0;
	adC13 = 0.0;
	amortality_fract = 0.0;
	apcp = 0.0;
  atmin = 0.0;
  atmax = 0.0;
  asnow = 0.0;



	for (h=0; h < basin[0].num_hillslopes; h++){
		hillslope = basin[0].hillslopes[h];
		hill_area = 0.0;
		for (z=0; z< hillslope[0].num_zones; z++){
			zone = hillslope[0].zones[z];
			apcp += (zone[0].rain+zone[0].snow)*zone[0].area;
      atmin += zone[0].metv.tmin * zone[0].area;
      atmax += zone[0].metv.tmax * zone[0].area;
      asnow += zone[0].snow * zone[0].area;
			zone_area += zone[0].area;
			for (p=0; p< zone[0].num_patches; p++){
				patch = zone[0].patches[p];
				arain_throughfall += patch[0].rain_throughfall * patch[0].area;
				asnow_throughfall += patch[0].snow_throughfall * patch[0].area;
				asat_deficit_z += patch[0].sat_deficit_z * patch[0].area;
				asat_deficit += patch[0].sat_deficit * patch[0].area;
				arz_storage += patch[0].rz_storage * patch[0].area;		
				aunsat_storage += patch[0].unsat_storage * patch[0].area;
				arz_drainage += patch[0].rz_drainage * patch[0].area;	
				aunsat_drainage += patch[0].unsat_drainage * patch[0].area;
				acap_rise += patch[0].cap_rise * patch[0].area;
				aevaporation += (patch[0].evaporation + patch[0].evaporation_surf
					+ patch[0].exfiltration_sat_zone
					+ patch[0].exfiltration_unsat_zone) * patch[0].area;
				asublimation += patch[0].snowpack.sublimation * patch[0].area;
				asnowpack += patch[0].snowpack.water_equivalent_depth*patch[0].area;
				if (patch[0].snowpack.water_equivalent_depth > 0.001)
					aperc_snow += patch[0].area;

				aPET += (patch[0].PET) * patch[0].area;
				alitter_store += patch[0].litter.rain_stored * patch[0].area;
				adetention_store += patch[0].detention_store*patch[0].area;
				aacctrans += patch[0].acc_year_trans * patch[0].area; 
				atranspiration += (patch[0].transpiration_sat_zone
					+ patch[0].transpiration_unsat_zone)  *  patch[0].area;
				if (patch[0].sat_deficit <= ZERO)
					asat_area += patch[0].area;
				if (routing_flag == 1) {
					if (patch[0].drainage_type == STREAM  )  
						astreamflow += patch[0].streamflow*patch[0].area;
						areturn_flow += patch[0].return_flow * patch[0].area;
						abase_flow += patch[0].base_flow * patch[0].area;
						astreamflow_N += patch[0].streamflow_N * patch[0].area;
				}
				else {
						/* for Topmodel version compute only return flow and later added to streamflow */
						areturn_flow += patch[0].return_flow * patch[0].area;
				}
				for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
						acrain += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].rain_stored +
							 patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].snow_stored )
							* patch[0].area;
						apsn += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.net_psn
							* patch[0].area;
						alai += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.proj_lai
							* patch[0].area;
						asublimation += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].sublimation
							* patch[0].area;
						adC13 += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].dC13
							* patch[0].area;
						amortality_fract += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.mortality_fract
							* patch[0].area;
					}
				}
				aarea +=  patch[0].area;
				hill_area += patch[0].area;
			}
		}
		hbase_flow += hillslope[0].base_flow * hill_area;
		hstreamflow_N += hillslope[0].streamflow_N * hill_area;
		hgw += hillslope[0].gw.storage * hill_area;
		hgwN += hillslope[0].gw.NO3 * hill_area;
		hgwQout += hillslope[0].gw.Qout * hill_area;
		hgwNout += hillslope[0].gw.Nout * hill_area;
		basin_area += hill_area;
	}
	adC13 /=  aarea;
	amortality_fract /=  aarea;
	apcp /= zone_area;
  atmin /= zone_area;
  atmax /= zone_area;
  asnow /= zone_area;

	aPET /=  aarea;
	acrain /=  aarea;
	arain_throughfall /=  aarea;
	adetention_store /= aarea;
	asnow_throughfall /= aarea ;
	asat_deficit_z /= aarea ;
	asat_deficit /= aarea ;
	arz_storage /= aarea ;
	aunsat_storage /= aarea ;
	arz_drainage /= aarea ;	
	aunsat_drainage /= aarea ;
	acap_rise /= aarea ;
	areturn_flow /= aarea ;
	aevaporation /= aarea ;
	asnowpack /= aarea  ;
	asublimation /= aarea  ;
	aperc_snow /= aarea  ;
	alitter_store /= aarea;
	atranspiration /= aarea  ;
	astreamflow /= aarea;
	apsn /= aarea ;
	alai /= aarea;
	abase_flow /= aarea;
	astreamflow_N /= aarea;
	asat_area /= aarea;
	aacctrans /= aarea; 

	hgwN = hgwN / basin_area;
	hgw = hgw / basin_area;
	hgwNout = hgwNout / basin_area;
	hgwQout = hgwQout / basin_area;
	abase_flow += (hbase_flow / basin_area);
	astreamflow += (hbase_flow / basin_area);

	if (routing_flag == 0)
		astreamflow += areturn_flow;

	var_flag = 1;
	var_trans = 0; 
	var_acctrans = 0;
	if (var_flag == 1) {
		for (h=0; h < basin[0].num_hillslopes; h++){
		hillslope = basin[0].hillslopes[h];
		for (z=0; z< hillslope[0].num_zones; z++){
			zone = hillslope[0].zones[z];
			for (p=0; p< zone[0].num_patches; p++){
				patch = zone[0].patches[p];
				var_acctrans += pow( 1000*(patch[0].acc_year_trans - aacctrans), 2.0)  *  patch[0].area;
				var_trans += pow( 1000*(patch[0].transpiration_sat_zone
					+ patch[0].transpiration_unsat_zone - atranspiration), 2.0)  *  patch[0].area;
			}
		}
		}
	}

	var_trans /= aarea;
	var_acctrans /= aarea;
				

	fprintf(outfile,"%d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
		date.day,
		date.month,
		date.year,
		basin[0].ID,
		arain_throughfall * 1000.0,
		asnow_throughfall * 1000.0,
		asat_deficit_z * 1000.0,
		asat_deficit * 1000.0,
		arz_storage * 1000.0,
		aunsat_storage * 1000.0,
		arz_drainage * 1000.0,
		aunsat_drainage * 1000.0,
		acap_rise * 1000.0,
		aevaporation * 1000.0,
		asnowpack * 1000.0,
		atranspiration * 1000.0,
		abase_flow * 1000.0,
		areturn_flow * 1000.0,
		astreamflow * 1000.0,
		apsn,
		alai,
		hgwQout *1000.0,
		hgwNout * 1000.0,
		hgw *1000.0,
		hgwN * 1000.0,
		adetention_store * 1000,
		asat_area * 100,
		alitter_store * 1000,
		acrain * 1000.0, 
		aperc_snow *100,
		asublimation * 1000.0,
		var_trans,
		aacctrans*1000,
		var_acctrans,
		aPET*1000,adC13, apcp*1000.0, amortality_fract*100,
	  atmax, atmin, asnow*1000.0 
		);

	return;
} /*end output_basin*/
