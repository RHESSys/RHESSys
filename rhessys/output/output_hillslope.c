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
	/* add more output varibale by NREN 2018/121/7 */
	double aprecip, aevap_surface, asoil_evap, arz_stor, arz_stor_flux, adetention_stor, adetention_stor_flux, acanopy_stor, acanopy_stor_flux, alitter_stor;
    double asat_deficit_flux, aunsat_stor_flux, asnowpack_flux, aunsat_stor_preday;
	double aarea;
	struct	patch_object  *patch;
	struct	zone_object	*zone;
	double aPET, asublimation, acanopysubl, aheight, awoodc, alai_red;
	double agsurf, apotential_exfil, arootzoneS, ags, aga; //NREN 20200202

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

    /* add more output variables NREN 2018/12/7*/
	aprecip =0.0;
	aevap_surface =0.0;
	asoil_evap =0.0;
	arz_stor =0.0;
	alitter_stor =0.0;

	adetention_stor=0.0; // this detention_store, canopy_store
	acanopy_stor=0.0;

	adetention_stor_flux=0.0;
	acanopy_stor_flux=0.0;

	asat_deficit_flux =0.0;
	aunsat_stor_flux =0.0;
	asnowpack_flux =0.0;
	arz_stor_flux =0.0;
	aPET = 0.0; //add new output 20191129 NREN
	asublimation = 0.0;
	acanopysubl = 0.0;
	aheight = 0.0;
	awoodc = 0.0;
	alai_red = 0.0;

	agsurf = 0.0;
	apotential_exfil = 0.0;
	arootzoneS = 0.0;
	ags = 0.0;
	aga =0.0;





	for (z=0; z<hillslope[0].num_zones; z++){
		zone = hillslope[0].zones[z];
		for (p=0; p< zone[0].num_patches; p++){
			patch = zone[0].patches[p];
			arain_throughfall += patch[0].rain_throughfall * patch[0].area;
			asnow_throughfall += patch[0].snow_throughfall * patch[0].area;
			asat_deficit_z += patch[0].sat_deficit_z * patch[0].area;
			asat_deficit += patch[0].sat_deficit * patch[0].area;




			/* add the other output to make the waterbuget works*/
			/* by Ning Ren 2018/12/7*/
			aprecip += (zone[0].rain+zone[0].snow) * patch[0].area;
			aevap_surface += patch[0].evaporation_surf * patch[0].area;
			asoil_evap += (patch[0].exfiltration_sat_zone + patch[0].exfiltration_unsat_zone) * patch[0].area;//soil evap is a little different
			arz_stor += patch[0].rz_storage * patch[0].area;

			adetention_stor += patch[0].detention_store * patch[0].area ;
			acanopy_stor += (patch[0].rain_stored + patch[0].snow_stored) * patch[0].area;

			alitter_stor += (patch[0].litter.rain_stored)* patch[0].area;

			/* add more gs ga gsurf rootzone.S potential exfiltration NREN 20200202 */

			agsurf += patch[0].gsurf*patch[0].area;
			apotential_exfil += patch[0].potential_exfiltration*patch[0].area;
			arootzoneS += patch[0].rootzone.S*patch[0].area;


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
			aPET += (patch[0].PET) * patch[0].area; //output pet
			asublimation += patch[0].snowpack.sublimation * patch[0].area; //output snow sublimation
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
                    alai_red += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
						* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.proj_lai_when_red
						* patch[0].area;
                    acanopysubl += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
                        * patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].sublimation //output snow sublmiation
                        * patch[0].area;
                    aheight += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].epv.height
							* patch[0].area; //output height
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
							* patch[0].area; //output plant carbon
                    ags += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].gs
							* patch[0].area; //output gs NREN 20200202
                    aga += patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
							* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].ga
							* patch[0].area; //output ga 20200202
				}
			}
		}
	}

	arain_throughfall /=  aarea;
	asnow_throughfall /= aarea ;
	asat_deficit_z /= aarea ;
	asat_deficit /= aarea ;
	aunsat_storage /= aarea ;
	aunsat_stor_preday /=aarea;
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


		/* add new output by ning ning 2018/12/7*/
	aprecip /=aarea;
	aevap_surface /= aarea;
	asoil_evap /=aarea;
	arz_stor  /=aarea;

	adetention_stor /=aarea;
	acanopy_stor /=aarea;
	alitter_stor /=aarea;

    /* add new output */
    aPET /=aarea;
    asublimation /=aarea;
    acanopysubl /=aarea;
    aheight /=aarea;
    awoodc /=aarea;
    alai_red /=aarea;

    //new NREN 20200202
    agsurf /= aarea;
    apotential_exfil /= aarea;
    arootzoneS /= aarea;
    ags /= aarea;
    aga /= aarea;



	fprintf(outfile,"%d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
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

		        /* NREN add more */
		aprecip*1000.0,
		aevap_surface *1000.0, //evap including canopy sublimation
		asoil_evap *1000.0,
		arz_stor *1000.0,
		adetention_stor*1000.0,
		acanopy_stor *1000.0,
		alitter_stor *1000.0,
		hillslope[0].area,
		// add more output
		aPET*1000.0,
		asublimation*1000.0, // this snowpack ground sublimitation
		acanopysubl*1000.0, // this is canopy snow sublimation
		aheight, //what is the unit
		awoodc,
		alai_red,

		//more 2020
		agsurf,
		apotential_exfil,
		arootzoneS,
		ags,
		aga);
	return;
} /*end output_hillslope*/
