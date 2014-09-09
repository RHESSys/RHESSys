/*--------------------------------------------------------------*/
/* 								*/
/*		compute_stage_storage			*/
/*								*/
/*	NAME							*/
/*	compute_stage_storage - estimates water volume	*/
/*		of SCM given a water depth	*/
/*		Returned value is a table w/ columns:  /*
/*  (0) stage [m] (1) volume storage [m3] (2) outflow [m3/s]  */
/*  (3) surface area [m2],  (4) outflow [m/time int in hours], (5) outflow [m/time int in days], */
/*   (6) volume [m]      */
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	compute_stage_storage(				*/
/*              int,            */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double,         */
/*              double)			*/
/*								*/
/*	returns:						*/
/*	stagestorage - 9x1001 matrix to be put pointed to as patch variable */
/*  "scm_stage_storage"	w/ columns:							*/
/*  (0) stage [m] (1) volume storage [m3] (2) outflow [m3/s]  */
/*  (3) surface area [m2], (4) outflow [m/time int in hours] [m3/s*60*60 / patch area], (5) outflow [m/time int in days] [m3/s*60*60*24 / patch area]*/
/*   (6) volume [m] (7) 2S/T+O hourly (8) 2S/T+O daily      */
/*								*/
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



double ** compute_stage_storage(
                    int scm_default_ID,
                    double scm_max_area,
                    double scm_max_H,
                    double scm_LtoW,
                    double scm_SS,
                    double orifice_n,
                    double orifice_coef,
                    double orifice_D,
                    double orifice_H,
                    double riser_L,
                    double riser_coef,
                    double riser_H,
                    double spillway_L,
                    double spillway_coef,
                    double spillway_H,
                    double orifice_D_2,
                    double orifice_coef_2,
                    double orifice_H_2,
                    double orifice_D_3,
                    double orifice_coef_3, 
                    double orifice_H_3,
                    double orifice_D_4,
                    double orifice_coef_4,
                    double orifice_H_4)
{

    /*--------------------------------------------------------------*/
    /*	Local Function Definintion                                */
    /*--------------------------------------------------------------*/   
     
    void    *alloc( size_t, char *, char *);
    
    /*--------------------------------------------------------------*/
    /*	Define  local variables                                 */
    /*--------------------------------------------------------------*/
    int l,g,k,kk,ori_count;
    double orifice_Ds[4], orifice_coefs[4], orifice_Hs[4];
    double scm_min_area, scm_max_length, scm_max_width, scm_max_volume; /* m^2, m^2, m, m, m^3 */
    double scm_min_width, scm_min_length, param_max_H;
    double htmp, vtmp, atmp; /* temporary height, volume, area - m | m^3 | m^2 */
    double pond_outflow; /* Total outflow form pond at this height - m^3/s */
    double riser_as_weir, riser_as_orifice; /* Riser discharge using either hydrulic equation, take minimum - m^3/s | m^3/s */
    double **stagestorage; //method from: http://www.cs.swarthmore.edu/~newhall/unixhelp/C_arrays.html - still have warning when returning "incompatible pointer"
    double tmp;

    stagestorage = (double **)malloc(sizeof(double *)*1001);
    for (l=0; l<1001; l++){
     stagestorage[l] = (double *)malloc(sizeof(double)*10);
    }
    
    param_max_H = scm_max_H;
    orifice_Ds[0] = orifice_D;
    orifice_Ds[1] = orifice_D_2;
    orifice_Ds[2] = orifice_D_3;
    orifice_Ds[3] = orifice_D_4;
    orifice_coefs[0] = orifice_coef;
    orifice_coefs[1] = orifice_coef_2;
    orifice_coefs[2] = orifice_coef_3;
    orifice_coefs[3] = orifice_coef_4;
    orifice_Hs[0] = orifice_H;
    orifice_Hs[1] = orifice_H_2;
    orifice_Hs[2] = orifice_H_3;
    orifice_Hs[3] = orifice_H_4;
    
    
    /*--------------------------------------------------------------*/
    /*  Compute Relevant scm Geometric Features               */
    /*--------------------------------------------------------------*/
    scm_max_width = pow((scm_max_area / scm_LtoW), 0.5);
    scm_max_length = pow((scm_max_area * scm_LtoW), 0.5);
    scm_min_length = (scm_max_length - 2*(scm_SS * scm_max_H));
    scm_min_width = (scm_max_width - 2*(scm_SS*scm_max_H));
    
    // If side slopes are too steep, max height will not be met
    if(scm_min_width <= 0){
          scm_min_area   = 0;
          scm_min_width  = 0;
          scm_max_H      = scm_max_width / (2*scm_SS);
          scm_min_length = max(scm_max_length-(2*scm_SS*scm_max_H),0);
         
         // Adjust orifice heights so they are the same distance from the top of the SCM as specified in the def. files
         
          for(g = 0; g < 4; g++) {
               
               orifice_Hs[g] = orifice_Hs[g] - (param_max_H-scm_max_H);
               if (orifice_Ds[g] > 0 && orifice_Hs[g] < 0) {
                    fprintf(stderr, "\nWARNING: Orifice %d in SCM %d is below bottom of SCM, lower SCM side slope or respecify height.  Orifice will be ignored\n", (g+1), scm_default_ID);
                   orifice_Ds[g] = 0;
               }
          }
         
         riser_H = riser_H - (param_max_H - scm_max_H);
         if(riser_H < 0) {
              riser_L = 0;
              fprintf(stderr, "\nWARNING: Riser in SCM %d is below bottom of SCM, lower SCM side slope or respecify height.  Riser will be ignored\n", scm_default_ID);
         }
         
         spillway_H = spillway_H - (param_max_H - scm_max_H);
         if(spillway_H < 0) {
              riser_L = 0;
              fprintf(stderr, "\nWARNING: Spillway in SCM %d is below bottom of SCM, lower SCM side slope or respecify height.  Spillway will be ignored\n", scm_default_ID);
         }

    // Other wise, heights will be met
    } else {
          scm_min_area = scm_min_width*scm_min_length;
    }

    scm_max_volume = scm_max_H/3 * (scm_max_area + scm_min_area + pow((scm_max_area*scm_min_area),0.5));
    

    /*--------------------------------------------------------------*/
    /*  Compute Lookup Table for Stage-Storage-Discharge Relationship     */
    /*--------------------------------------------------------------*/

    for (k = 0; k < 1001; k++) {
    
        tmp = (k*1);
        htmp               = (tmp/1000) * scm_max_H;
        atmp               = (scm_max_length - 2 * scm_SS * (-1* (htmp - scm_max_H) ))*(scm_max_width - 2 * scm_SS * (-1*(htmp-scm_max_H)));
        vtmp               = htmp/3 * (atmp + scm_min_area + pow((atmp * scm_min_area),0.5));
        stagestorage[k][0] = htmp;
        stagestorage[k][1] = vtmp;
        stagestorage[k][3] = atmp;
        
         // Initialize outflow at zero
         pond_outflow = 0;

         // Check to see if there are any orifices
         if (orifice_n > 0){
               // Loop through orifices, and determine their contribution to outflow.
               for (ori_count=0; ori_count<orifice_n; ori_count++){

                   if (orifice_Ds[ori_count] > 0){
                          // Case #1 - Water level < Orifice height
                          if (htmp <= orifice_Hs[ori_count]-(orifice_Ds[ori_count]/2)){
                              pond_outflow += 0;
                          }
                          // Case #2 - Oririce is not fully submerged (from "Elements of Urban Stormwater Design" by H. Rooney Malcolm - 1989 NCSU, Raleigh, NC)
                          // The discharge in such a pipe is proportional  to the three-halves power of depth and fitting the expression to the ofiice result at full depth
                          else if (htmp > orifice_Hs[ori_count]-(orifice_Ds[ori_count]/2) && htmp <= orifice_Hs[ori_count]+(orifice_Ds[ori_count]/2)) {
                             // pond_outflow += 0.372*orifice_coefs[ori_count]*orifice_Ds[ori_count]*pow((htmp-(orifice_Hs[ori_count]-(orifice_Ds[ori_count]/2))),(3/2))*(1/0.0254)*pow((1/0.3048),(3/2)); // the last two are unit conversion factors for the Malcolm, 1989 formula
                            pond_outflow += 0.6633*orifice_coefs[ori_count]*orifice_Ds[ori_count]*pow((htmp-(orifice_Hs[ori_count]-(orifice_Ds[ori_count]/2))),(3/2));
                          // Case #3 - Orifice is fully submerged
                          } else {
                              pond_outflow += orifice_coefs[ori_count]*PI*pow((orifice_Ds[ori_count]),2)/4*pow((2*9.81*htmp-orifice_Hs[ori_count]),(0.5));
                          }
                    }
               }
                      
         }
         
         // Now add riser discharge, if applicable
         if (riser_L > 0){
             if (htmp > riser_H){
                 // Must choose minimum of the two
                 riser_as_weir    = riser_coef*4*riser_L*pow((htmp-riser_H),(1.5))*pow((2*9.81),0.5);
                 riser_as_orifice = 0.59 * pow(riser_L,2) * pow((2*9.81*(htmp-riser_H)),0.5); // hard-code 0.59 orifice Cd for riser
                 pond_outflow    += min(riser_as_weir,riser_as_orifice);
             }
         }
         
         // Now add spillway discharge, if applicable
         if (spillway_L > 0){
             if (htmp > spillway_H){
                 pond_outflow += spillway_coef*spillway_L*pow((htmp-spillway_H),1.5)*pow((2*9.81),0.5);
             }
         }
       
        stagestorage[k][2] = pond_outflow; // in m^3/s
        stagestorage[k][4] = pond_outflow*60*60/scm_max_area; // m/h
        stagestorage[k][5] = pond_outflow*60*60*24/scm_max_area;  // m/day
        stagestorage[k][6] = stagestorage[k][1]/scm_max_area; // m
        stagestorage[k][7] = pond_outflow*60*60/scm_max_area + 2*vtmp/scm_max_area; // m/h
        stagestorage[k][8] = pond_outflow*60*60*24/scm_max_area + 2*vtmp/scm_max_area; // m/day
        stagestorage[k][9] = pond_outflow*60/scm_max_area + 2*vtmp/scm_max_area; // m/min
        
        //fprintf(stderr, "\n%f  %f  %f  %f  %f  %f  %f  %f  %f  %f", stagestorage[k][0], stagestorage[k][1],stagestorage[k][2],stagestorage[k][3],stagestorage[k][4],stagestorage[k][5],stagestorage[k][6],stagestorage[k][7],stagestorage[k][8],stagestorage[k][9]);
    }
    

    return(stagestorage); 
    
} /*compute_stage_storage*/
