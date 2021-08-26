/*--------------------------------------------------------------*/
/*                                                              */
/*		update_nitrif				*/
/*                                                              */
/*  NAME                                                        */
/*		update_nitrif				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  void update_nitrif(				*/
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
/*	compute nitrification and nitrification 		*/
/*	based on soil temperature, moisture, heter. resp,	*/
/*	soil texture, and C substrate, N avaiilability		*/
/*	based on relationships derived in			*/
/*								*/
/*	effect of pH from Parton et al 2004			*/
/*      pH equation from tropical acidic soils                  */
/*      effect of excess NH4                                    */
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
#include "phys_constants.h"
#include <stdio.h>
#include <math.h>

#define  PARTICLE_DENSITY	2.65	/* soil particle density g/cm3 (Dingman) */
#define	 MAX_PERC		0.1	/* fraction of amonium that goes to nitrate */
#define  MAX_RATE		120	/* mg/N/kg/day twice groffman values for ag soils */
#define NUM_NORMAL  10 	/* resolution of normal distribution */
double NORMAL[10]= {0,0,0.253,0.524,0.842,1.283,-0.253,-0.524,-0.842,-1.283};

int update_nitrif(
				  struct  soil_c_object   *cs_soil,
				  struct  soil_n_object   *ns_soil,
				  struct cdayflux_patch_struct *cdf,
				  struct ndayflux_patch_struct *ndf,
				  struct  soil_class   soil_type,
				  double  PH,
				  double  theta,
				  double soilT,
				  double porosity,
				  double organic_soil_depth,
				  double nitrif_prop, struct patch_object *patch,
				  double std)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int ok,i;
	double nitrify;
	double a, b, c, d;
	double nh4_conc, bulk_density, kg_soil;
	double N_scalar, water_scalar, T_scalar, pH_scalar;
	double thetai;
	double max_nit_rate; /* kg/m2/day */
	double perc_sat;
	double resource_satNH4;


	ok = 1;
	max_nit_rate = 0.0;

    //----------- dynamic
        if( patch[0].soil_defaults[0][0].active_zone_z > patch[0].sat_deficit_z){
            theta = (patch[0].rz_storage + patch[0].unsat_storage + patch[0].soil_defaults[0][0].active_zone_sat_0z - patch[0].sat_deficit) * patch[0].soil_defaults[0][0].active_zone_sat_0z_1;

            perc_sat = max(0.0,min(1.0,(patch[0].soil_defaults[0][0].active_zone_sat_0z - patch[0].sat_deficit)/patch[0].available_soil_water));

        }else if(patch[0].soil_defaults[0][0].active_zone_z > patch[0].rootzone.depth){
            theta = (patch[0].rz_storage+patch[0].unsat_storage) / patch[0].sat_deficit; // approximate
            perc_sat = 0.0;
        }else{
            theta = patch[0].rz_storage/patch[0].rootzone.potential_sat;
            perc_sat = 0.0;
        }


	if ((theta <= ZERO) || (theta > 1.0))
		theta = 1.0;

    resource_satNH4 = perc_sat*patch[0].sat_NH4;

	if ((ns_soil->sminn + patch[0].sat_NH4) > ZERO) {

        //resource_satNH4 = perc_sat*patch[0].sat_NH4;
		/*--------------------------------------------------------------*/
		/* estimate bulk density from porosity and assuming a particle density */
		/* 	of 2.65 g/cm3 (Dingman)					*/
		/* and convert to kg/m3						*/
		/* really should integrate depth to account for variable porosity	*/
		/* but for now assume constant					*/
		/* and should be done once in construction of soil defaults	*/
		/*--------------------------------------------------------------*/
		bulk_density = PARTICLE_DENSITY * (1.0 - porosity) * 1000;
		kg_soil = bulk_density * organic_soil_depth;
		max_nit_rate = kg_soil * MAX_RATE * 0.000001;
		/*--------------------------------------------------------------*/
		/* compute ammonium conc. in ppm				*/
		/*--------------------------------------------------------------*/
		nh4_conc = (ns_soil->sminn + resource_satNH4) / kg_soil * 1000000.0;

		/*--------------------------------------------------------------*/
		/* effect of ammonium conc on nitrification			*/
		/*--------------------------------------------------------------*/
		N_scalar = 1.0 - exp(-0.0105 * nh4_conc);

		/*--------------------------------------------------------------*/
		/*	compute effect of water and temperature on nitrification */
		 /*--------------------------------------------------------------*/
		if (soil_type.sand > 0.5) {
			a = 0.55; b=1.7; c=-0.007; d=3.22;
		}
		else {
			a=0.6; b=1.27; c=0.0012; d=2.84;
		}

		water_scalar = 0.0;
		if (std > ZERO) {
			for (i=0; i<NUM_NORMAL; i++) {
				thetai = theta + NORMAL[i]*std;
				thetai = min(1.0, thetai);
				thetai = max(0.0, thetai);
				water_scalar  += 1.0/NUM_NORMAL * (
						 pow( ((thetai -b) / (a-b)), d*(b-a)/(a-c))
						* pow( ((thetai-c)/ (a-c)), d) );
				}
			}
		else {
			if (theta  > c)
				water_scalar  = pow( ((theta -b) / (a-b)), d*(b-a)/(a-c))
				* pow( ((theta-c)/ (a-c)), d);
			else
				water_scalar = 0.000001;
			}

		T_scalar = -0.06 + 0.13 * exp(0.07 * soilT);
		if (T_scalar < ZERO) T_scalar = 0.0;
                /*--------------------------------------------------------------*/
                /* effect of pH on nitrification                                */
                /*--------------------------------------------------------------*/
                pH_scalar = 0.56 + (atan(PI*0.45*(-5+PH))/PI);


		/*--------------------------------------------------------------*/
		/*	estimate nitrification				*/
		/* 	by scaling a maximum rate suggested by Parton et al.		*/
		/*--------------------------------------------------------------*/

		nitrify = water_scalar * T_scalar * N_scalar * pH_scalar * MAX_RATE * ns_soil->sminn * 1000.0;

	} /* end mineralized N available */
	else
		nitrify = 0.0;
	/*--------------------------------------------------------------*/
	/*	update state and flux variables				*/
	/* 	convert from g to kg					*/
	/*--------------------------------------------------------------*/
	nitrify = min(nitrify/1000, ns_soil->sminn + resource_satNH4);
	nitrify = max(nitrify, 0.0);
	nitrify = min(nitrify, max_nit_rate);
	// make sure nitrify is larger than zero and smaller sminn
	nitrify = min(nitrify, ns_soil->sminn + resource_satNH4);
	nitrify = max(nitrify, 0.0);

	ndf->sminn_to_nitrate = nitrify;

	double ratio = ns_soil->sminn / (ns_soil->sminn + resource_satNH4);
	if (ns_soil->sminn > ZERO) {
	ns_soil->sminn -= (nitrify) *ratio;
	ns_soil->sminn = max(0.0, ns_soil->sminn);
	ns_soil->nitrate += (nitrify) *ratio;}

	if (resource_satNH4 > ZERO) {

	patch[0].sat_NH4 -= nitrify * (1- ratio);
	patch[0].sat_NH4 = max(0.0, patch[0].sat_NH4);
	patch[0].sat_NO3 += nitrify * (1 - ratio);

	}



	ok = 0;
	return(ok);
} /* end update_nitrif */

