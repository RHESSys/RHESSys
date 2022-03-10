/*--------------------------------------------------------------*/
/*                                                              */
/*		update_denitrif				*/
/*                                                              */
/*  NAME                                                        */
/*		update_denitrif				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  void update_denitrif(				*/
/*                                                              */
/*			struct  soil_c_object   *               */
/*                      struct  soil_n_object   *               */
/*                      struct  cdayflux_patch_object *         */
/*                      struct  ndayflux_patch_object *         */
/*			struct	soil_class 			*/
/*			double					*/
/*			double					*/
/*			double					*/
/*                              )                               */
/*  OPTIONS                                                     */
/*                                                              */
/*                                                              */
/*  DESCRIPTION                                                 */
/*	compute nitrification and denitrification 		*/
/*	based on soil temperature, moisture, heter. resp,	*/
/*	soil texture, and C substrate, N avaiilability		*/
/*	based on relationships derived in			*/
/*	effect of PH currently and excess NH4			*/
/*      currently ignored					*/
/*								*/
/*	Parton et al. 1996. Generalized model of N2 and N20 	*/
/*	production, Global Biogeochemical cycles, 10:3		*/
/*	401-412							*/
/*                                                              */
/*								*/
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include "rhessys.h"
#include <stdio.h>
#include <math.h>

int update_denitrif(
					struct  soil_c_object   *cs_soil,
					struct  soil_n_object   *ns_soil,
					struct cdayflux_patch_struct *cdf,
					struct ndayflux_patch_struct *ndf,
					struct  soil_class   soil_type,
					double  theta, struct patch_object *patch,
					double std)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok,i;
	double denitrify, denitrify_soil, denitrify_sat;
	double a, b, c, d;
	double water_scalar, thetai, water_scalari;
	double fnitrate_total, fnitrate_soil, fCO2;
	double hr, total_nitrate_ratio, soil_nitrate_ratio, perc_sat;
	double resource_satNO3;
	resource_satNO3 = 0.0;
	perc_sat = 0.0;


	#define NUM_NORMAL  10 	/* resolution of normal distribution */
	double NORMAL[10]= {0,0,0.253,0.524,0.842,1.283,-0.253,-0.524,-0.842,-1.283};

	ok = 1;
    if (patch[0].sat_NO3 < ZERO) patch[0].sat_NO3 = 0.0;
	        //----------- dynamic means part of water table is inside of active_zone. 0z is z converted to water content
        if( patch[0].soil_defaults[0][0].active_zone_z > patch[0].sat_deficit_z){
            theta = (patch[0].rz_storage + patch[0].unsat_storage + patch[0].soil_defaults[0][0].active_zone_sat_0z - patch[0].sat_deficit) * patch[0].soil_defaults[0][0].active_zone_sat_0z_1;

            perc_sat = max(0.0,min(1.0,(patch[0].soil_defaults[0][0].active_zone_sat_0z - patch[0].sat_deficit)/patch[0].available_soil_water));
            // change here

        }else if(patch[0].soil_defaults[0][0].active_zone_z > patch[0].rootzone.depth){ // means water table not reach the active zone
            theta = (patch[0].rz_storage+patch[0].unsat_storage) / patch[0].sat_deficit; // approximate
            perc_sat = 0.0;
        }else{
            theta = patch[0].rz_storage/patch[0].rootzone.potential_sat;
            perc_sat = 0.0;
        }

       // resource_satNO3 = perc_sat*patch[0].sat_NO3;
       patch[0].theta = theta;
       patch[0].perc_sat = perc_sat;

	if ((theta <= ZERO) || (theta > 1.0)) theta = 1.0;
    if (patch[0].sat_deficit <= ZERO) theta = 1;

	if ((ns_soil->nitrate + patch[0].sat_NO3) > ZERO) {

		/*--------------------------------------------------------------*/
		/*	maximum denitrfication (kg/ha) based on available	*/
		/*	carbon substrate - estimated from heter. respiration    */
		/*--------------------------------------------------------------*/
		hr = (cdf->soil1c_hr + cdf->soil2c_hr + cdf->soil3c_hr + cdf->soil4c_hr);
		if (hr > ZERO)
			fCO2 = 0.0024 / (1+ 200.0/exp(0.35*hr*10000.0)) - 0.00001;

		else
			fCO2 = 0.0;
		/*--------------------------------------------------------------*/
		/*	compute denitrification rate				*/
		/*	- assuming a constant nitrification rate		*/
		/*--------------------------------------------------------------*/
		if (soil_type.sand > 0.5) {
			a = 1.56; b=12.0; c=16.0; d=2.01;
		}
		else if (soil_type.clay > 0.5) {
			a = 60.0; b=18.0; c=22.0; d=1.06;
		}
		else {
			a=4.82; b=14.0; c=16.0; d=1.39;
		}

		water_scalar = 0.0;
		if (std > 0) {
			for (i =1; i< NUM_NORMAL; i++) {
				thetai = theta + std*NORMAL[i];
				thetai = min(1.0, thetai);
				thetai = max(0.0, thetai);
				if (thetai > ZERO)
				water_scalari = min(1.0,a / pow(b,  (c / pow(b, (d*thetai) )) ));
				water_scalar += 1.0/NUM_NORMAL * water_scalari;
				}
			}
		else
				water_scalar = min(1.0,a / pow(b,  (c / pow(b, (d*theta) )) ));

        //double resource_satNO3 = perc_sat*patch[0].sat_NO3; // NEW NREN
       // resource_satNO3 = perc_sat*patch[0].sat_NO3;

       total_nitrate_ratio = (ns_soil->nitrate + patch[0].sat_NO3)/ (cs_soil->totalc + ns_soil->totaln) * 1e6;
       soil_nitrate_ratio = ns_soil->nitrate / (cs_soil->totalc + ns_soil->totaln) * 1e6;

		/*nitrate_ratio = (ns_soil->nitrate + patch[0].sat_NO3)
			/ (cs_soil->totalc + ns_soil->totaln) * 1e6; //NREN totalN doesn't include in patch_daily_F.c 2496 n20210903 maybe here is the bug*/
        //printf("\n nitrate ratio %lf", nitrate_ratio);
		/*--------------------------------------------------------------*/
		/*	maximum denitrfication (kg/ha) based on available	*/
		/*		N03							*/
		/*--------------------------------------------------------------*/
		fnitrate_total = atan(PI*0.002*(total_nitrate_ratio - 180)) * 0.004 / PI + 0.0011;
		fnitrate_soil = atan(PI*0.002*(soil_nitrate_ratio - 180)) * 0.004 / PI + 0.0011;

		denitrify = min(ns_soil->nitrate + patch[0].sat_NO3, fnitrate_total*water_scalar); // total nitrate
		denitrify_soil = min(ns_soil->nitrate, fnitrate_soil*water_scalar); //ns_soil->nitrate
		denitrify_sat = min(patch[0].sat_NO3, max(0, denitrify - denitrify_soil)); //patch_sat_NO3

		if(denitrify_sat + denitrify_soil < denitrify)
		{
            denitrify_soil = max(0.0, min(ns_soil->nitrate, denitrify - denitrify_sat));
		}
        if(denitrify<0 || denitrify_soil<0 || denitrify_sat<0)
        {
            printf("denitrification negative %d,%f,%f,%f\n", patch[0].ID, denitrify,denitrify_soil,denitrify_sat);
        }//debug



		//printf("\n nitrate scale factor %lf", fnitrate);
        denitrify = denitrify_soil + denitrify_sat; // nitrate ratio * water_scalar
        ndf->Pot_denitrif_CO2 = fCO2 * water_scalar; // respiration
        ndf->Pot_denitrif_SS = denitrify; // resource

        if(ndf->Pot_denitrif_SS>0.0 && ndf->Pot_denitrif_CO2>0.0 && ndf->Pot_denitrif_CO2 < ndf->Pot_denitrif_SS){
            // limited by respiration factor
            denitrify_soil *= ndf->Pot_denitrif_CO2 / ndf->Pot_denitrif_SS; // here
            denitrify_sat *= ndf->Pot_denitrif_CO2 / ndf->Pot_denitrif_SS;
            denitrify = ndf->Pot_denitrif_CO2;

            ns_soil->nvolatilized_snk += denitrify;
            ndf->sminn_to_nvol = denitrify;
            if(ns_soil->nitrate > 0.0) ns_soil->nitrate *= max(0.0, 1.0-denitrify_soil/ns_soil->nitrate); //here learn
            if(patch[0].sat_NO3 > 0.0) patch[0].sat_NO3 *= max(0.0, 1.0-denitrify_sat/patch[0].sat_NO3);
            ndf->denitrif = denitrify;

        }else if(ndf->Pot_denitrif_SS>0.0 && ndf->Pot_denitrif_CO2>0.0){
            //limited by resource
            ns_soil->nvolatilized_snk += denitrify;
            ndf->sminn_to_nvol = denitrify;
            if(ns_soil->nitrate>0.0) ns_soil->nitrate *= max(0.0, 1.0-denitrify_soil/ns_soil->nitrate);
            if(patch[0].sat_NO3>0.0) patch[0].sat_NO3 *= max(0.0, 1.0-denitrify_sat/patch[0].sat_NO3);
            ndf->denitrif = denitrify;
        }else{
            denitrify = 0.0;
            ndf->Pot_denitrif_CO2 = 0.0;
            ndf->Pot_denitrif_SS = 0.0;
            ndf->sminn_to_nvol = 0.0;
            ndf->denitrif = 0.0;
        }


		/*--------------------------------------------------------------*/
		/*	estimate denitrification				*/
		/*--------------------------------------------------------------*/
		//denitrify = min(fCO2, fnitrate) * water_scalar;
	} /* end mineralized N available */
	else {
        // no nitrate
        denitrify = 0.0;
        ndf->Pot_denitrif_CO2 = 0.0;
        ndf->Pot_denitrif_SS = 0.0;
        ndf->sminn_to_nvol = 0.0;
        ndf->denitrif = 0.0;
    }//if else

	/*--------------------------------------------------------------*/
	/*	update state and flux variables				*/
	/*--------------------------------------------------------------*/
	/*denitrify = min(denitrify, (ns_soil->nitrate + patch[0].sat_NO3));
	denitrify = max(0.0, denitrify);
	ns_soil->nvolatilized_snk += denitrify;
	ndf->sminn_to_nvol = denitrify;

	if(ns_soil->nitrate > ZERO) {
	 ns_soil->nitrate = ns_soil->nitrate - denitrify * ns_soil->nitrate/(patch[0].sat_NO3 + ns_soil->nitrate);
	 ns_soil->nitrate = max(0.0, ns_soil->nitrate);} //New NREN

	if(patch[0].sat_NO3 > ZERO) {
	 patch[0].sat_NO3 = patch[0].sat_NO3 - denitrify * patch[0].sat_NO3/(patch[0].sat_NO3 + ns_soil->nitrate);
	 patch[0].sat_NO3 = max(0.0, patch[0].sat_NO3);}

	ndf->denitrif = denitrify; */
    if(ndf->denitrif<ndf->Pot_denitrif_CO2 && ndf->denitrif<ndf->Pot_denitrif_SS){
        printf("denitrification prob %d,%f,%f, %f\n", patch[0].ID,denitrify_soil,denitrify_sat,
               ndf->denitrif, ndf->Pot_denitrif_CO2, ndf->Pot_denitrif_SS );
    }


	ok = 0;
	return(ok);
} /* end update_denitrif */