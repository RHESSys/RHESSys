/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_N_leached				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_N_leached				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  void compute_N_leached(int					*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	);		*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*								*/
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"
//#define  PARTICLE_DENSITY    2.65    /* soil particle density g/cm3 (Dingman) */

double	compute_Nsat_leached(
            int verbose_flag,
            double total_nitrate, //<--- sat_solutes
            double Qout,
            double N_decay_rate, // no use
            double tmp_,
            double N_absorption_rate,
            int signal,
            struct patch_object *patch)
	{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
//    	double  compute_delta_water(
//                int,
//                double,
//                double,
//                double,
//                double,
//                double);
//

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/

        double nleached = 0.0;
        double z1 = 0.0;
        double Qout_frac = 0.0;
        double N0 = 0.0;
        double absorptionConst = 0.0;

        double critialZ = 0.0;
        double soil_water;
        double p_0 = patch[0].soil_defaults[0][0].porosity_0;
        double p_decayRate = 1.0 / patch[0].soil_defaults[0][0].porosity_decay;
        double tmp =  patch[0].soil_defaults[0][0].active_zone_z;


        z1 = (patch[0].sat_deficit_z>0? patch[0].sat_deficit_z : 0.0);
        //z1 = z1>patch[0].constraintWaterTableTopDepth? z1 : patch[0].constraintWaterTableTopDepth; // do this correction for basement
        if((patch[0].drainage_type == ROAD) && signal<0){
            z1 = patch[0].road_cut_depth<tmp? patch[0].road_cut_depth : (tmp*0.9); // tmp must be > road_cut_depth
        }

        // assume "sat_solutes" are in the sat_zone; and "soil_solutes" are above the sat_zone;
        // "sat_solutes" follows POR profile (related to PARTICLE_DENSITY profile) while "soil_solutes" follows the soil_solutes_decay

        // from text book: % pore space = porosity = (1 - BD / density) * 100 %
        // from my note: density = BD / (1 - POR)
        // BD = (1-porosity)*density * 1000; // 1000 is converting unit to kg/m3 from g/cm3
        // nabsorbed = {n_0 * (z2-z1)} * N_absorption_rate * BD;
        // nabsorbed = {integrated POR} * N_absorption_rate * BD;
	/*------------------------------------------------------*/
	/* nitrate export only occurs when Qout > 0.0		*/
	/*------------------------------------------------------*/

	    /* update sat_deficit reated variables */
    if(patch[0].sat_deficit >= 0){
        patch[0].sat_deficit = min(patch[0].sat_deficit, patch[0].soil_defaults[0][0].soil_water_cap);
        patch[0].available_soil_water = patch[0].soil_defaults[0][0].soil_water_cap - patch[0].sat_deficit;// this is how much sat water
    }else{
        // surface
        patch[0].available_soil_water = patch[0].soil_defaults[0][0].soil_water_cap;
        patch[0].sat_deficit_z = patch[0].sat_deficit;
    }

    soil_water = patch[0].available_soil_water; //this saturated zone available soil water; patch[0].soil_defaults[0][0].soil_water_cap-max(patch[0].sat_deficit, 0.0);

    if (Qout > ZERO && total_nitrate> ZERO && soil_water > ZERO) {

        Qout_frac = min(Qout/soil_water,1.0);
        absorptionConst = PARTICLE_DENSITY * 1000.0 * N_absorption_rate; //patch[0].soil_defaults[0][0].particledensity * 1000.0 * N_absorption_rate;
        if(absorptionConst>0){
            N0 = total_nitrate / soil_water;
            if(N0 >= absorptionConst){
                nleached = (N0-absorptionConst)*soil_water + absorptionConst*soil_water*soil_water; //why also leaching absorbed N because water also absorp nitrate
                nleached *= Qout_frac;
            }else{
                // (zz/decay) < log(#P0) - log(#-@); log = natural log
                critialZ = log(absorptionConst*patch[0].soil_defaults[0][0].porosity_0) - log(absorptionConst-N0);
                critialZ *= patch[0].soil_defaults[0][0].porosity_decay;
                if(critialZ>patch[0].soil_defaults[0][0].soil_depth) critialZ = patch[0].soil_defaults[0][0].soil_depth;
                if(critialZ<0 ) critialZ = 0.0; // what if log(0) it will cause -inf
                // which side?
                if(patch[0].soil_defaults[0][0].porosity_decay>0 && critialZ>z1){
                    // any z <= critialZ
                    double POR_critialZ = patch[0].soil_defaults[0][0].rtz2sat_def_0z[(int)(critialZ*1000)];
                    double POR_z1 = patch[0].soil_defaults[0][0].rtz2sat_def_0z[(int)(z1*1000)];
                    double por_critialZ = p_0 - POR_critialZ*p_decayRate; // smaller
                    double por_z1 = p_0 - POR_z1*p_decayRate; // bigger

                    nleached = (N0-absorptionConst)*(POR_critialZ-POR_z1) + 0.5*absorptionConst*patch[0].soil_defaults[0][0].porosity_decay*(por_z1*por_z1-por_critialZ*por_critialZ);
                    if(nleached>0) nleached *= Qout_frac;
                    else nleached = 0.0;

                }else if(patch[0].soil_defaults[0][0].porosity_decay<0 && critialZ < patch[0].soil_defaults[0][0].soil_depth){
                    // any z >= critialZ
                    double POR_critialZ = patch[0].soil_defaults[0][0].rtz2sat_def_0z[(int)(critialZ*1000)];
                    double POR_z1 = patch[0].soil_defaults[0][0].soil_water_cap;
                    double por_critialZ = p_0 - POR_critialZ*p_decayRate; // smaller
                    double por_z1 = p_0 - POR_z1*p_decayRate; // bigger

                    nleached = (N0-absorptionConst)*(POR_z1-POR_critialZ) + 0.5*absorptionConst*patch[0].soil_defaults[0][0].porosity_decay*(por_z1*por_z1-por_critialZ*por_critialZ); // check
                    if(nleached>0) nleached *= Qout_frac;
                    else nleached = 0.0;

                }else{
                    nleached = 0.0;
                }
            }
        }else{
            // no absorption but yes resources
            nleached = total_nitrate * Qout_frac;
            //
        }
    }else{
        // no resources
        nleached = 0.0;
    }//end of Qout > ZERO


	/*------------------------------------------------------*/
	/* there may be enough flow to leach out more than 	*/
	/*	availabe nitrate, so limit export by available	*/
	/*------------------------------------------------------*/

    if (nleached > total_nitrate)
    {
        printf("\n [patch %d] n leached is %e, > total_nitrate is %e", patch[0].ID, nleached, total_nitrate);
        nleached = total_nitrate;
        }

    if(nleached<-0.000001 || nleached!=nleached) printf("\n leaching[%d, %d]: ->(%e,%e,%e), N0(%e)=(%e), absorptionConst(%e), critialZ_ini(%e), critialZ(%e)>(%e), nleached(%e)<(%e), %e %e %e\n",
           patch[0].ID, signal,
           Qout,Qout_frac,soil_water,
           N0, N_decay_rate,
           absorptionConst,
           absorptionConst/N0,
           critialZ, z1,
           nleached,total_nitrate,
           tmp, patch[0].soil_defaults[0][0].porosity_0, patch[0].soil_defaults[0][0].porosity_decay);
    if (nleached < ZERO || nleached != nleached) nleached = 0.0;


	return(nleached);
} /* end compute_N_leached */

