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
	double arain_throughfall;
	double asnow_throughfall;
	double alitter_store;
	double asat_deficit_z;
	double asat_deficit;
	double aunsat_storage, arz_storage;
	double aunsat_drainage, arz_drainage;
	double acap_rise;
	double arecharge;
	double areturn_flow;
	double aevaporation;
	double asnowpack, aperc_snow, asnowmelt;
	double atranspiration;
	double astreamflow;
	double asublimation, acanopysubl;
	double asat_area, adetention_store;
	double apsn, alai, acrain, acsnow;
	double abase_flow, hbase_flow,  hstreamflow_NO3, hstreamflow_NH4;
	double	aacctrans, var_acctrans, var_trans;
	double aPET, adC13, amortality_fract, apcp, apcpassim;
	double	hgw;
	double atmin, atmax, atavg, avpd, asnow;
	double	hgwQout;
	double aarea, hill_area, zone_area, basin_area;
	double agpsn, aresp, ags, arootdepth, aleafc, afrootc, awoodc;
	double aheight;
	double aevap_can, aevap_lit, aevap_soil, alitrc;
	double aKdown, aLdown;
	double aKstar_can, aKstar_soil, aKstar_snow;
	double aLstar_can, aLstar_soil, aLstar_snow;
	double aKup, aLup;
	double aLE_can, aLE_soil, aLE_snow;
	double acLstar;
	double acdrip;
	double acga;	
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
	arecharge = 0.0;
	apsn = 0.0 ;
	aPET = 0.0;
	aarea =  0.0 ;
	abase_flow = 0.0;
	hbase_flow = 0.0;
	hstreamflow_NO3 = 0.0;
	hstreamflow_NH4 = 0.0;
	hgwQout = 0.0;
	hgw = 0.0;
	alai = 0.0;
	adetention_store = 0.0;
	aacctrans = 0.0; 
	basin_area = 0.0;
	zone_area = 0.0;
	adC13 = 0.0;
	amortality_fract = 0.0;
	apcp = 0.0;
	apcpassim = 0.0;
    atmin = 0.0;
    atmax = 0.0;
    atavg = 0.0;
    avpd = 0.0;
    asnow = 0.0;
	
	agpsn=0.0;
	aresp=0.0;
	ags=0.0;
	arootdepth = 0.0;
	aleafc = 0.0;
	afrootc = 0.0;
	awoodc = 0.0;
	asnowmelt = 0.0;
	acanopysubl = 0.0;
	acsnow = 0.0;
	aheight = 0.0;
	
	aevap_can = 0.0;
	aevap_lit = 0.0;
	aevap_soil = 0.0;
	
	aKdown = 0.0;
	aLdown = 0.0;
	aKup = 0.0;
	aLup = 0.0;
	aKstar_can = 0.0;
	aKstar_soil = 0.0;
	aKstar_snow = 0.0;
	aLstar_can = 0.0;
	aLstar_soil = 0.0;
	aLstar_snow = 0.0;
	aLE_can = 0.0;
	aLE_soil = 0.0;
	aLE_snow = 0.0;
	acLstar = 0.0;
	
	alitrc = 0.0;
	acdrip = 0.0;
	acga = 0.0;
	

	for (h=0; h < basin[0].num_hillslopes; h++){
		hillslope = basin[0].hillslopes[h];
		hill_area = 0.0;
		for (z=0; z< hillslope[0].num_zones; z++){
			zone = hillslope[0].zones[z];
			apcp += (zone[0].rain_hourly_total+zone[0].rain+zone[0].snow)*zone[0].area;
			atmin += zone[0].metv.tmin * zone[0].area;
			atmax += zone[0].metv.tmax * zone[0].area;
			atavg += zone[0].metv.tavg * zone[0].area;
			avpd += zone[0].metv.vpd * zone[0].area;
			aKdown += (zone[0].Kdown_diffuse + zone[0].Kdown_direct) * zone[0].area;
			aLdown += zone[0].Ldown * zone[0].area;
			asnow += zone[0].snow * zone[0].area;
			zone_area += zone[0].area;
			for (p=0; p< zone[0].num_patches; p++){
				patch = zone[0].patches[p];
				arain_throughfall += (patch[0].rain_throughfall_24hours + patch[0].rain_throughfall) * patch[0].area;
				asnow_throughfall += patch[0].snow_throughfall * patch[0].area;
				apcpassim += patch[0].precip_with_assim * patch[0].area;
				asat_deficit_z += patch[0].sat_deficit_z * patch[0].area;
				asat_deficit += patch[0].sat_deficit * patch[0].area;
				arecharge += patch[0].recharge * patch[0].area;
				arz_storage += patch[0].rz_storage * patch[0].area;		
				aunsat_storage += patch[0].unsat_storage * patch[0].area;
				arz_drainage += patch[0].rz_drainage * patch[0].area;	
				aunsat_drainage += patch[0].unsat_drainage * patch[0].area;
				acap_rise += patch[0].cap_rise * patch[0].area;
				aevaporation += (patch[0].evaporation + patch[0].evaporation_surf
					+ patch[0].exfiltration_sat_zone
					+ patch[0].exfiltration_unsat_zone) * patch[0].area;
				aevap_can += (patch[0].evaporation) * patch[0].area;
				aevap_lit += (patch[0].evaporation_surf) * patch[0].area;
				aevap_soil += (patch[0].exfiltration_sat_zone
							   + patch[0].exfiltration_unsat_zone) * patch[0].area;
				asublimation += patch[0].snowpack.sublimation * patch[0].area;
				asnowpack += patch[0].snowpack.water_equivalent_depth*patch[0].area;
				if (patch[0].snowpack.water_equivalent_depth > 0.001)
					aperc_snow += patch[0].area;
				asnowmelt += patch[0].snow_melt*patch[0].area;
				aPET += (patch[0].PET) * patch[0].area;
				alitter_store += patch[0].litter.rain_stored * patch[0].area;
				adetention_store += patch[0].detention_store*patch[0].area;
				aacctrans += patch[0].acc_year_trans * patch[0].area; 
				atranspiration += (patch[0].transpiration_sat_zone
					+ patch[0].transpiration_unsat_zone)  *  patch[0].area;
				alitrc += (patch[0].litter_cs.litr1c + patch[0].litter_cs.litr2c
						   + patch[0].litter_cs.litr3c + patch[0].litter_cs.litr4c)
				* patch[0].area;
				aKup += (patch[0].Kup_direct + patch[0].Kup_diffuse) * patch[0].area;
				aLup += (patch[0].Lup) * patch[0].area;
				aKstar_can += patch[0].Kstar_canopy * patch[0].area;
				aKstar_soil += patch[0].Kstar_soil * patch[0].area;
				aKstar_snow += (patch[0].snowpack.Kstar_direct + patch[0].snowpack.Kstar_diffuse) * patch[0].area;
				aLstar_can += patch[0].Lstar_canopy * patch[0].area;
				aLstar_soil += patch[0].Lstar_soil * patch[0].area;
				aLstar_snow += patch[0].Lstar_snow * patch[0].area;
				aLE_can += patch[0].LE_canopy * patch[0].area;
				aLE_soil += patch[0].LE_soil * patch[0].area;
				aLE_snow += (-1 * patch[0].snowpack.Q_LE + patch[0].snowpack.Q_melt) * patch[0].area;
				if (patch[0].sat_deficit <= ZERO)
					asat_area += patch[0].area;
				if (routing_flag == 1) {
					if (patch[0].drainage_type == STREAM  )  
						astreamflow += patch[0].streamflow*patch[0].area;
						areturn_flow += patch[0].return_flow * patch[0].area;
						abase_flow += patch[0].base_flow * patch[0].area;
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
						acanopysubl += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].sublimation
							* patch[0].area;
						adC13 += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].dC13
							* patch[0].area;
						amortality_fract += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.mortality_fract
							* patch[0].area;
						agpsn += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction * patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.psn_to_cpool
							* patch[0].area;
						aresp += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.leaf_day_mr + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.cpool_leaf_gr
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.leaf_night_mr +	patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.livestem_mr
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.cpool_livestem_gr + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.livecroot_mr
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.cpool_livecroot_gr
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.cpool_deadcroot_gr
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.froot_mr + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.cpool_froot_gr
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cdf.cpool_to_gresp_store)	* patch[0].area;
						ags += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction 
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].gs * patch[0].area;
						arootdepth += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction 
							* (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].rootzone.depth)
							* patch[0].area;
						aleafc += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction	* (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.leafc
							+ patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.leafc_store + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.leafc_transfer ) 
							* patch[0].area;
						afrootc += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction * (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.frootc
							+ patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.frootc_store + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.frootc_transfer)
							* patch[0].area;
						awoodc += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction	* (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.live_crootc
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.live_stemc + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.dead_crootc
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.dead_stemc + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.livecrootc_store
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.livestemc_store + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.deadcrootc_store
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.deadstemc_store
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.livecrootc_transfer
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.livestemc_transfer
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.deadcrootc_transfer
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.deadstemc_transfer
						   + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.cwdc + patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.cpool) 
							* patch[0].area;
						acsnow += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* (	patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].snow_stored )
							* patch[0].area;
						aheight += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction 
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.height 
							* patch[0].area;
						acLstar += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* (	patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].Lstar )
							* patch[0].area;
						acdrip += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* (	patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].canopy_drip )
							* patch[0].area;
						acga += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].ga * patch[0].area;
					}
				}
				aarea +=  patch[0].area;
				hill_area += patch[0].area;
			}
		}
		hbase_flow += hillslope[0].base_flow * hill_area;
		hgw += hillslope[0].gw.storage * hill_area;
		hgwQout += hillslope[0].gw.Qout * hill_area;
		basin_area += hill_area;
	}
	adC13 /=  aarea;
	amortality_fract /=  aarea;
	apcp /= zone_area;
	atmin /= zone_area;
	atmax /= zone_area;
	atavg /= zone_area;
	avpd /= zone_area;
    asnow /= zone_area;
	aKdown /= zone_area;
	aLdown /= zone_area;	

	apcpassim /=  aarea;
	aPET /=  aarea;
	acrain /=  aarea;
	arecharge /= aarea;
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
	asat_area /= aarea;
	aacctrans /= aarea; 

	agpsn /= aarea;
	aresp /= aarea;
	ags /= aarea;
	arootdepth /= aarea;
	aleafc /= aarea;
	afrootc /= aarea;
	awoodc /= aarea;
	asnowmelt /= aarea;
	acanopysubl /= aarea;
	aheight /= aarea;
	acsnow /= aarea;
	
	aevap_can /= aarea ;
	aevap_lit /= aarea ;
	aevap_soil /= aarea ;
	
	alitrc /= aarea;
	
	aKup /= aarea;
	aLup /= aarea;
	aKstar_can /= aarea;
	aKstar_soil /= aarea;
	aKstar_snow /= aarea;
	aLstar_can /= aarea;
	aLstar_soil /= aarea;
	aLstar_snow /= aarea;
	aLE_can /= aarea;
	aLE_soil /= aarea;
	aLE_snow /= aarea;
	acLstar /= aarea;
	acdrip /= aarea;
	acga /= aarea;
	
	hgw = hgw / basin_area;
	hgwQout = hgwQout / basin_area;
	abase_flow += (hbase_flow / basin_area);
	astreamflow += (hbase_flow / basin_area);
    
	if (routing_flag == 0)
		astreamflow += areturn_flow;

	var_flag = 1;
	var_trans = 0; 
	var_acctrans = 0;
	if (var_flag == 1) {
        #pragma omp parallel for reduction(+ : var_acctrans,var_trans)           //160628LML
        for (int h=0; h < basin[0].num_hillslopes; h++){
        struct hillslope_object *hillslope = basin[0].hillslopes[h];
        for (int z=0; z< hillslope[0].num_zones; z++){
            struct zone_object *zone = hillslope[0].zones[z];
            for (int p=0; p< zone[0].num_patches; p++){
                struct patch_object *patch = zone[0].patches[p];
				var_acctrans += pow( 1000*(patch[0].acc_year_trans - aacctrans), 2.0)  *  patch[0].area;
				var_trans += pow( 1000*(patch[0].transpiration_sat_zone
					+ patch[0].transpiration_unsat_zone - atranspiration), 2.0)  *  patch[0].area;
			}
		}
		}
	}

	var_trans /= aarea;
	var_acctrans /= aarea;
				

	fprintf(outfile,"%d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
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
		hgw *1000.0,
		adetention_store * 1000,
		asat_area * 100,
		alitter_store * 1000,
		acrain * 1000.0, 
		aperc_snow *100,
		asublimation * 1000.0,
		var_trans,
		aacctrans*1000,
		var_acctrans,
		aPET*1000,
		adC13, 
		apcp*1000.0,
		apcpassim*1000.0, 
		amortality_fract*100,
	  	atmax, 
		atmin, 
		atavg,
		avpd, 
		asnow*1000.0,
		arecharge*1000.0,
		agpsn * 1000,
		aresp * 1000,
		ags * 1000,
		arootdepth*1000.0,
		(aleafc + awoodc + afrootc),
		asnowmelt * 1000.0,
		acanopysubl * 1000.0, 
		/* for now we do not route deep groundwater just add it in at the end */
		(basin[0].stream_list.streamflow*86400/aarea + (hbase_flow / basin_area))*1000.0,
		acsnow * 1000.0, 
		aheight,
		aevap_can * 1000.0,
		aevap_lit * 1000.0,
		aevap_soil * 1000.0,
		alitrc, 
		aKdown,
		aLdown,
		aKup,
		aLup,
		aKstar_can,
		aKstar_soil,
		aKstar_snow, 
		aLstar_can,
		aLstar_soil,
		aLstar_snow,
		aLE_can,
		aLE_soil,
		aLE_snow,
		acLstar,
		acdrip*1000,
		acga*1000
		);
	return;
} /*end output_basin*/
