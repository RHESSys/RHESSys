/*--------------------------------------------------------------*/
/*					                                              			*/
/*		compute_patch_family_routing		                        	*/
/*					                                               			*/
/*	compute_patch_family_routing.c - routes within patch family	*/
/*					                                              			*/
/*	NAME						                                          	*/
/*	compute_patch_family_routing.c - routes within patch family	*/
/*					                                              			*/
/*	SYNOPSIS			                                        			*/
/*	struct XXXXXXXXXXXX compute_patch_family_routing( 	  */
/*							struct command_line_object command              */
/*							struct hillslope_object *hillslopen)	          */
/*				 			int,			                                     	*/
/*							struct date *current_date)          	          */
/*										                                          */
/* 										                        	                */
/*										                        	                */
/*	OPTIONS								                    		              */
/*										                        	                */
/*										                        	                */
/*	DESCRIPTION								                    	            */
/*										                        	                */
/*										                        	                */
/*										                        	                */
/*										                        	                */
/*	PROGRAMMER NOTES							                	            */
/*										                        	                */
/*	Feb 20, 2019 William Burke						                    	*/
/*	route water between patches within a patch family       		*/
/*											                                        */
/*--------------------------------------------------------------*/
#include <stdio.h>
//#include "rhessys.h"

void compute_patch_family_routing(
  struct command_line_object *command_line,
  struct zone_object *zone) {
    
  /*--------------------------------------------------------------*/
	/*	Local function definition.	                          			*/
	/*--------------------------------------------------------------*/
  




	/*--------------------------------------------------------------*/
	/*	Local variable definition.			                          	*/
	/*--------------------------------------------------------------*/

  double wet_mean;
  double wet_mean_sat;
  double k_z_mean;
  double area_sum;
  double wp_mean;

  int skip;

  double delta_L;
  double delta_L_actual;
  double delta_L_potential;
  double delta_L_sa;
  double delta_L_sa_actual;
  double delta_L_sa_potential;

	/*--------------------------------------------------------------*/
	/*	initializations						*/
	/*--------------------------------------------------------------*/
  
  wet_mean = 0;
  wet_mean_sat = 0;
  k_z_mean = 0;
  area_sum = 0;
  wp_mean = 0;

  // should patch be skipped/included(is it connected?)? 1 no, 0 yes
  skip = 1;



	/*--------------------------------------------------------------*/
	/*	Get mean wetness - root+unsat, sat	*/
	/*--------------------------------------------------------------*/
  
  // for (patch in 1:length(patch_family$patch)) { 

/*

  for (i = 0; i < XXXpatchnumXXXX; i++) {

    // if both sh coefficients are not 0, include patch
    if (patch_family$sh_l[patch] != 0 & patch_family$sh_g[patch] != 0) { 
      // 1 = no skip
      skip = c(skip,1) 
      // wetness mean based on root and unsat zone *area
      wet_mean = wet_mean + (patch_family[patch,"rz_q"] + patch_family[patch,"un_q"])*patch_family[patch,"area"];
      // sat wetness mean
      wet_mean_sat = wet_mean_sat + (patch_family[patch,"sa_q"]) * patch_family[patch,"area"];
      k_z_mean = k_z_mean + patch_family[patch,"k"] * exp(-patch_family[patch,"sa_q"]*patch_family[patch,"m"]) * patch_family[patch,"area"];
      area_sum = area_sum + patch_family[patch,"area"];
      wp_mean = wp_mean + patch_family[patch,"rz_wp"]*patch_family[patch,"area"];
    } else {
      // 0 = skip
      skip = c(skip,0);
    }

  }

  wet_mean = wet_mean/length(patch_family$patch);
  wet_mean_sat = wet_mean_sat/length(patch_family$patch);
  
  k_z_mean = k_z_mean/sum(patch_family[patch,"area"]);
  wp_mean = wp_mean/sum(patch_family[patch,"area"]);


  

  // ----- loop 2, loop through losing (>mean) patches -----

  // temporary delta_L, since it gets revised in loop 4
  delta_L = c(0,0,0)
  delta_L_actual = 0
  delta_L_potential = 0

  delta_L_sa = c(0,0,0)
  delta_L_sa_actual = 0
  delta_L_sa_potential = 0

  for (patch in 1:length(patch_family$patch)) {
    if (skip[patch] != 0 & (patch_family[patch,"rz_q"] + patch_family[patch,"un_q"])*patch_family[patch,"area"] > wet_mean) { # rz+unsat losers
      delta_L[patch] = ((patch_family[patch,"rz_q"] + patch_family[patch,"un_q"])*patch_family[patch,"area"] - wet_mean) * patch_family[patch,"sh_l"]
      delta_L_actual = delta_L_actual +  delta_L[patch]
      delta_L_potential = delta_L_potential + ((patch_family[patch,"rz_q"] + patch_family[patch,"un_q"])*patch_family[patch,"area"] - wet_mean)
    }
    
    if (skip[patch] != 0 & patch_family[patch,"sa_q"]*patch_family[patch,"area"] > wet_mean_sat) { # sat losers
      delta_L_sa[patch] = (patch_family[patch,"sa_q"]*patch_family[patch,"area"] - wet_mean_sat) * patch_family[patch,"sh_l"]
      delta_L_sa_actual = delta_L_sa_actual + delta_L_sa[patch]
      delta_L_sa_potential = delta_L_sa_potential + patch_family[patch,"sa_q"]*patch_family[patch,"area"] - wet_mean_sat
    }
  }

  # ----- loop 3, loop through gaining (<mean) patches -----
  delta_G = c(0,0,0)
  delta_G_actual = 0
  delta_G_potential = 0
  delta_G_sa = c(0,0,0)
  delta_G_sa_actual = 0
  delta_G_sa_potential = 0
  gainers = c(0,0,0)

  for (patch in 1:length(patch_family$patch)) {
    if (skip[patch] != 0 & (patch_family[patch,"rz_q"] + patch_family[patch,"un_q"])*patch_family[patch,"area"] < wet_mean) { # rz+unsat gainers
      gainers[patch] = 1
      delta_G[patch] = (wet_mean - (patch_family[patch,"rz_q"] + patch_family[patch,"un_q"])*patch_family[patch,"area"]) * delta_L_actual/delta_L_potential * patch_family[patch,"sh_g"]
      delta_G_actual = delta_G_actual + delta_G[patch]
      delta_G_potential = delta_G_potential + (wet_mean - (patch_family[patch,"rz_q"] + patch_family[patch,"un_q"])*patch_family[patch,"area"])
      
      # Division of water (delta_G) between rz & unsat
      rz_z_pct = patch_family[patch,"rz_z"]/(patch_family[patch,"rz_z"] + patch_family[patch,"un_z"]) # percent of depth that is root zone
      delta_G_rz = min(delta_G[patch] * rz_z_pct, patch_family[patch,"rz_fc"]) # root zone gain is minimum of delta_G * root zone depth percent and field capacity
      patch_family[patch,"rz_q"] = patch_family[patch,"rz_q"] + delta_G_rz/patch_family[patch,"area"] # root zone store update (area removed, so just a depth)
      un_z_pct = patch_family[patch,"un_z"]/(patch_family[patch,"rz_z"] + patch_family[patch,"un_z"]) # unsat depth pct
      delta_G_un = delta_G[patch] * un_z_pct + max(delta_G_rz - patch_family[patch,"rz_fc"],0) # unsat gain is delta_G * unsat depth pct + max of 0 and delta_G_rz - rz field capacity
      patch_family[patch,"un_q"] = patch_family[patch,"un_q"] + delta_G_un/patch_family[patch,"area"] # unsat store update (depth)
    }
    
    if (skip[patch] != 0 & patch_family[patch,"sa_q"]*patch_family[patch,"area"] < wet_mean_sat) {# sat gainers
      delta_G_sa[patch] = (wet_mean_sat - patch_family[patch,"sa_q"]*patch_family[patch,"area"]) * delta_L_sa_actual/delta_L_sa_potential * patch_family[patch,"sh_g"]
      delta_G_sa_actual = delta_G_sa_actual + delta_G_sa[patch]
      delta_G_sa_potential = delta_G_sa_potential + (wet_mean - patch_family[patch,"sa_q"]*patch_family[patch,"area"])
      
      # change in sat store
      patch_family[patch,"sa_q"] = patch_family[patch,"sa_q"] + delta_G_sa[patch]/patch_family[patch,"area"] # sat store update
    }
  }

  # ----- loop 4, loop through and reallocate to losing patches -----
  # if loss>gain, allocating proportionately based area*delta
  delta_L_2 = c(0,0,0)
  delta_L_sa_2 = c(0,0,0)

  for (patch in 1:length(patch_family$patch)) {
    if (skip[patch] != 0 & (patch_family[patch,"rz_q"] + patch_family[patch,"un_q"])*patch_family[patch,"area"] > wet_mean) {
      # update loss values for gains less than potential, should have no impact if L actual = G actual, but tiny rounding values show up for some reason.
      delta_L_2[patch] = delta_L[patch] - (delta_L_actual - delta_G_actual) * (delta_L[patch]/delta_L_actual)
      
      # distribute delta_L_2 between rz and unsat
      # RZ - removes down to the mean wilting point of the gaining patches
      delta_L_2_rz = min(delta_L_2[patch]/patch_family[patch,"area"], patch_family[patch,"rz_q"] - mean(patch_family$rz_wp[gainers]) )
      patch_family[patch,"rz_q"] = patch_family[patch,"rz_q"] - delta_L_2_rz
      # Unsat - removes remainder (if any) thar shouldve been taken from root zone
      patch_family[patch,"un_q"] = patch_family[patch,"un_q"] - max(delta_L_2[patch]/patch_family[patch,"area"] - delta_L_2_rz ,0)
    }
    
    if (skip[patch] != 0 & patch_family[patch,"sa_q"]*patch_family[patch,"area"] > wet_mean_sat) { #sat
      delta_L_sa_2[patch] = delta_L_sa[patch] - (delta_L_sa_actual - delta_G_sa_actual) * (delta_L_sa[patch]/delta_L_sa_actual)
      
      patch_family[patch,"sa_q"] = patch_family[patch,"sa_q"] - delta_L_sa_2[patch]/patch_family[patch,"area"]
    }
  }

  <<<<<<<<<< End routing loops code >>>>>>>>>> 
  */

    }
