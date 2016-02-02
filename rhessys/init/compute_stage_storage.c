/*--------------------------------------------------------------*/
/* 								*/
/*		compute_stage_storage			*/
/*								*/
/*	NAME							*/
/*	compute_stage_storage - estimates water volume	*/
/*		of SCM given a water depth	*/
/*		Returned value is a table w/ columns:  /*
/*  (0) stage [m] (1) surface area [m2],  (2) volume [m], (3) outflow [m/time int in hours], (4) outflow [m/time int in days], (5) 2*S/T + O [m/min] */
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	compute_stage_storage(structure	patch ) */
/*								*/
/*	returns:						*/
/*	stagestorage - 6xnum_discrete matrix to be pointed to as patch variable */
/*  "scm_stage_storage"	w/ columns:							   */
/*  (0) stage [m] (1) surface area [m2],  (2) volume [m],                */
/*  (3) outflow [m/time int in hours], (4) outflow [m/time int in days], */
/*  (5) 2*S/T + O [m/min]                                                */
/*	DESCRIPTION						*/
/*								*/
/*	computes volume stored in scm over a range of depths */
/*  Depths run between 0 and maxH - specified in default file */
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"


double ** compute_stage_storage(struct patch_object *patch)
{

    /*--------------------------------------------------------------*/
    /*	Local Function Definintion                                */
    /*--------------------------------------------------------------*/   
     
    void    *alloc( size_t, char *, char *);
    
    /*--------------------------------------------------------------*/
    /*	Define  local variables                                 */
    /*--------------------------------------------------------------*/
    int l,g,k,kk,ori_count;
    double scm_min_area, scm_max_length, scm_max_width, scm_max_volume, scm_max_H; /* m^2, m^2, m, m, m^3, m */
    double scm_min_width, scm_min_length, param_max_H;
    double htmp, vtmp, atmp; /* temporary height, volume, area - m | m^3 | m^2 */
    double pond_outflow; /* Total outflow form pond at this height - m^3/s */
    double riser_as_weir, riser_as_orifice; /* Riser discharge using either hydrulic equation, take minimum - m^3/s | m^3/s */
    double **stagestorage; //method from: http://www.cs.swarthmore.edu/~newhall/unixhelp/C_arrays.html - still have warning when returning "incompatible pointer"
    double tmp;

    stagestorage = (double **)malloc(sizeof(double *)*(patch[0].scm_defaults[0][0].num_discrete+1));
    for (l=0; l<(patch[0].scm_defaults[0][0].num_discrete+1); l++){
     stagestorage[l] = (double *)malloc(sizeof(double)*6);
    }
	
    /*--------------------------------------------------------------*/
    /*  Compute Relevant scm Geometric Features               */
    /*--------------------------------------------------------------*/
    scm_max_width = pow((patch[0].area / patch[0].scm_defaults[0][0].LtoW), 0.5);
    scm_max_length = pow((patch[0].area * patch[0].scm_defaults[0][0].LtoW), 0.5);
    scm_min_length = (scm_max_length - 2*(patch[0].scm_defaults[0][0].SS * patch[0].scm_defaults[0][0].maxH));
    scm_min_width = (scm_max_width - 2*(patch[0].scm_defaults[0][0].SS*patch[0].scm_defaults[0][0].maxH));
    scm_max_H = patch[0].scm_defaults[0][0].maxH;
    
    // If side slopes are too steep, max height will not be met
    if(scm_min_width <= 0){
          scm_min_area   = 0;
          scm_min_width  = 0;
          scm_max_H      = scm_max_width / (2*patch[0].scm_defaults[0][0].SS);
          scm_min_length = max(scm_max_length-(2*patch[0].scm_defaults[0][0].SS*scm_max_H),0);
         
         // Adjust orifice heights so they are the same distance from the top of the SCM as specified in the def. files
         
          for(g = 0; g < patch[0].scm_defaults[0][0].orifice_n; g++) {
               
               patch[0].scm_defaults[0][0].orifice_H[g] = patch[0].scm_defaults[0][0].orifice_H[g] - (patch[0].scm_defaults[0][0].maxH - scm_max_H);
               if (patch[0].scm_defaults[0][0].orifice_D[g] > 0 && patch[0].scm_defaults[0][0].orifice_H[g] < 0) {
                    fprintf(stderr, "\nWARNING: Orifice %d in SCM %d is below bottom of SCM, lower SCM side slope or respecify height.  Orifice will be ignored\n", (g+1), patch[0].scm_defaults[0][0].ID);
                   patch[0].scm_defaults[0][0].orifice_D[g] = 0;
               }
          }
         
         patch[0].scm_defaults[0][0].riser_H = patch[0].scm_defaults[0][0].riser_H - (patch[0].scm_defaults[0][0].maxH - scm_max_H);
         if(patch[0].scm_defaults[0][0].riser_H < 0) {
              patch[0].scm_defaults[0][0].riser_L = 0;
              fprintf(stderr, "\nWARNING: Riser in SCM %d is below bottom of SCM, lower SCM side slope or respecify height.  Riser will be ignored\n", patch[0].scm_defaults[0][0].ID);
         }
         
         patch[0].scm_defaults[0][0].spillway_H = patch[0].scm_defaults[0][0].spillway_H - (patch[0].scm_defaults[0][0].maxH - scm_max_H);
         if(patch[0].scm_defaults[0][0].spillway_H < 0) {
              patch[0].scm_defaults[0][0].spillway_L = 0;
              fprintf(stderr, "\nWARNING: Spillway in SCM %d is below bottom of SCM, lower SCM side slope or respecify height.  Spillway will be ignored\n", patch[0].scm_defaults[0][0].ID);
         }

    // Other wise, heights will be met
    } else {
          scm_min_area = scm_min_width*scm_min_length;
    }

    scm_max_volume = scm_max_H/3 * (patch[0].area + scm_min_area + pow((patch[0].area*scm_min_area),0.5));
    
    /*--------------------------------------------------------------*/
    /*  Compute Lookup Table for Stage-Storage-Discharge Relationship     */
    /*--------------------------------------------------------------*/

    for (k = 0; k < (patch[0].scm_defaults[0][0].num_discrete+1); k++) {
    
        tmp = (k*1);
        htmp               = (tmp/(patch[0].scm_defaults[0][0].num_discrete+1)) * scm_max_H;
        atmp               = (scm_max_length - 2 * patch[0].scm_defaults[0][0].SS * (-1* (htmp - scm_max_H) ))*(scm_max_width - 2 * patch[0].scm_defaults[0][0].SS * (-1*(htmp-scm_max_H)));
        vtmp               = htmp/3 * (atmp + scm_min_area + pow((atmp * scm_min_area),0.5));
        stagestorage[k][0] = htmp; // m
        stagestorage[k][1] = atmp; // m^2
        stagestorage[k][2] = vtmp/patch[0].area; // m
        
          // Initialize outflow at zero
         pond_outflow = 0;

         // Check to see if there are any orifices
         if (patch[0].scm_defaults[0][0].orifice_n > 0){
               // Loop through orifices, and determine their contribution to outflow.
               for (ori_count=0; ori_count<patch[0].scm_defaults[0][0].orifice_n; ori_count++){

                   if (patch[0].scm_defaults[0][0].orifice_D[ori_count] > 0){
                          // Case #1 - Water level < Orifice height
                          if (htmp <= patch[0].scm_defaults[0][0].orifice_H[ori_count]-(patch[0].scm_defaults[0][0].orifice_D[ori_count]/2)){
                              pond_outflow += 0;
                          }
                          // Case #2 - Oririce is not fully submerged (from "Elements of Urban Stormwater Design" by H. Rooney Malcolm - 1989 NCSU, Raleigh, NC)
                          else if (htmp > patch[0].scm_defaults[0][0].orifice_H[ori_count]-(patch[0].scm_defaults[0][0].orifice_D[ori_count]/2) && htmp <= patch[0].scm_defaults[0][0].orifice_H[ori_count]+(patch[0].scm_defaults[0][0].orifice_D[ori_count]/2)) {
                             // pond_outflow += 0.372*orifice_coefs[ori_count]*orifice_Ds[ori_count]*pow((htmp-(orifice_Hs[ori_count]-(orifice_Ds[ori_count]/2))),(3/2))*(1/0.0254)*pow((1/0.3048),(3/2)); // the last two are unit conversion factors for the Malcolm, 1989 formula
                            pond_outflow += 0.6633*patch[0].scm_defaults[0][0].orifice_coef[ori_count]*patch[0].scm_defaults[0][0].orifice_D[ori_count]*pow((htmp-(patch[0].scm_defaults[0][0].orifice_H[ori_count]-(patch[0].scm_defaults[0][0].orifice_D[ori_count]/2))),(1.5));
                          // Case #3 - Orifice is fully submerged
                          } else {
                              pond_outflow += patch[0].scm_defaults[0][0].orifice_coef[ori_count]*PI*pow((patch[0].scm_defaults[0][0].orifice_D[ori_count]),2)/4*pow((2*9.81*(htmp-patch[0].scm_defaults[0][0].orifice_H[ori_count]-(patch[0].scm_defaults[0][0].orifice_D[ori_count]/2))),(0.5));
                          }
                    }
               }
                      
         }
         
         // Now add riser discharge, if applicable
         if (patch[0].scm_defaults[0][0].riser_L > 0){
             if (htmp > patch[0].scm_defaults[0][0].riser_H){
                 // Must choose minimum of the two - if the riser acts as a weir or orifice
                 riser_as_weir    = patch[0].scm_defaults[0][0].riser_weir_coef*patch[0].scm_defaults[0][0].riser_L*pow((htmp-patch[0].scm_defaults[0][0].riser_H),(1.5));
                 riser_as_orifice = patch[0].scm_defaults[0][0].riser_ori_coef * pow(patch[0].scm_defaults[0][0].riser_L/4,2) * pow((2*9.81*(htmp-patch[0].scm_defaults[0][0].riser_H)),0.5);
                 pond_outflow    += min(riser_as_weir,riser_as_orifice);
             }
         }
         
         // Now add spillway discharge, if applicable
         if (patch[0].scm_defaults[0][0].spillway_L > 0){
             if (htmp > patch[0].scm_defaults[0][0].spillway_H){
                 pond_outflow += patch[0].scm_defaults[0][0].spillway_coef*patch[0].scm_defaults[0][0].spillway_L*pow((htmp-patch[0].scm_defaults[0][0].spillway_H),1.5)*pow((2*9.81),0.5);
             }
         }
       

        stagestorage[k][3] = pond_outflow*60*60/patch[0].area; // m/h
        stagestorage[k][4] = pond_outflow*60*60*24/patch[0].area;  // m/day
        stagestorage[k][5] = pond_outflow*60/patch[0].area + 2*vtmp/patch[0].area; // m/min
        
        //fprintf(stderr, "\n%d %f  %f  %f  %f  %f  %f ", k, stagestorage[k][0], stagestorage[k][1],stagestorage[k][2],stagestorage[k][3],stagestorage[k][4],stagestorage[k][5]);
    }
    return(stagestorage); 
    
} /*compute_stage_storage*/
