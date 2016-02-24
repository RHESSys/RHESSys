/*--------------------------------------------------------------*/
/* 											*/
/*					update_drainage_land			*/
/*											*/
/*	update_drainage_scm.c - creates a patch object				*/
/*											*/
/*	NAME										*/
/*	update_drainage_scm.c - creates a patch object				*/
/*											*/
/*	SYNOPSIS									*/
/*	void update_drainage_scm( 							*/
/*				  struct zone_object *zone,
 /*                   struct patch_object *patch,			*/
/*				 			double,			 	*/
/*				 			double,			 	*/
/*				 			double,			 	*/
/*							int,				*/
/*							int)				*/
/*											*/
/* 											*/
/*											*/
/*	OPTIONS										*/
/*											*/
/*											*/
/*	DESCRIPTION									*/
/*											*/
/*											*/
/*											*/
/*											*/
/*	PROGRAMMER NOTES								*/
/*											*/
/*											*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"


void  update_drainage_scm(
					int zone_hourly_flag,
                         struct patch_object *patch,
                         struct command_line_object *command_line,
					double time_int,
					int verbose_flag,
                         struct date current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/
	double  compute_delta_water(
		int,
		double,
		double,
		double,
		double,
		double);
	
	double compute_varbased_returnflow(
		double,
		double,
		double,
		struct litter_object *);


	double compute_varbased_flow(
		int,
		double,
		double,
		double,
		double,
		double *,
		struct patch_object *);


	double compute_N_leached(int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double *);
     
     double compute_N_settled(
		double,
		double,
		double);
	
	double recompute_gamma(	
		struct patch_object *,
		double);


	double compute_infiltration( int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);
		
	double compute_infiltration_scm( int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);

	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	int k, j, d, idx, avek, mins, intused, layer, stratum, algae_flag;
	double step; /* minutes */
	double m, Ksat, std_scale;
	double NH4_leached_to_patch, NH4_leached_to_stream;
	double NO3_leached_to_patch, NO3_leached_to_stream;
	double DON_leached_to_patch, DON_leached_to_stream;
	double DOC_leached_to_patch, DOC_leached_to_stream;
	double NO3_leached_to_surface; /* kg/m2 */
	double NH4_leached_to_surface; /* kg/m2 */
	double DON_leached_to_surface; /* kg/m2 */
	double DOC_leached_to_surface; /* kg/m2 */
	double algae_leached_to_surface, algaeN_leached_to_surface, algaeC_leached_to_surface; /* kg/m2 */
	double neigh_CtoN, uphill_CtoN; /* kg-C / kg-N */
	double N_leached_total; /* kg/m2 */
	double DON_leached_total; /* kg/m2 */
	double DOC_leached_total; /* kg/m2 */
	double route_to_surface;  /* m3 */
	double return_flow,route_to_patch ;  /* m3 */
	double available_sat_water; /* m3 */
	double Qin, Qout;  /* m */
	double innundation_depth, infiltration; /* m */
	double total_gamma;
	double Nin, Nout, Aout, Cout; /* kg/m2 */ 
	double t1,t2,t3;
     double scm_ave_height; /* m */
     double out_hi, out_low, h_hi, h_low, vol_hi, vol_low, LHS, LHS_delta; /* m | m | m/time int | m/time int | m/minute | m/minute */
     double preday_scm_volume, preday_scm_inflow, preday_scm_outflow;
     double scm_H, scm_outflow, scm_outflow_tmp, scm_volume; /* m | m^3/s | m */

	struct patch_object *neigh;
     struct	canopy_strata_object	*strata;
	route_to_patch = 0.0;
	route_to_surface = 0.0;
	return_flow=0.0;

	DON_leached_to_patch = 0.0;
	DON_leached_to_stream = 0.0;
	DOC_leached_to_patch = 0.0;
	DOC_leached_to_stream = 0.0;
	NH4_leached_to_patch = 0.0;
	NH4_leached_to_stream = 0.0;
	NO3_leached_to_patch = 0.0;
	NO3_leached_to_stream = 0.0;
	NO3_leached_to_surface = 0.0;
	NH4_leached_to_surface = 0.0;
	DOC_leached_to_surface = 0.0;
	DON_leached_to_surface = 0.0;

	/*--------------------------------------------------------------*/
	/*	m and K are multiplied by sensitivity analysis variables */
	/*--------------------------------------------------------------*/
	m = patch[0].m ;
	Ksat = patch[0].soil_defaults[0][0].Ksat_0 ;
	d=0;

	/*--------------------------------------------------------------*/
	/*	recalculate gamma based on current saturation deficits  */
	/*      to account the effect of changes in water table slope 	*/
	/*--------------------------------------------------------------*/
	total_gamma = recompute_gamma(patch, patch[0].innundation_list[d].gamma);

	available_sat_water = max(((patch[0].soil_defaults[0][0].soil_water_cap
			- max(patch[0].sat_deficit,0.0))
			* patch[0].area),0.0);		
	// POTENTIALLY ADD SOMETHING HERE LIMITING AVAILABLE_SAT_WATER TO INFILTRATION + SUBSRUFACE WATER?
	//available_sat_water = 0;
	/*------------------------------------------------------------*/
	/*	calculate amuount of water output to patches			*/
	/*	this only computes subsurface flow, not overland flow	*/
	/*-----------------------------------------------------------*/

	std_scale = command_line[0].std_scale;

	route_to_patch =  time_int * compute_varbased_flow(
		patch[0].num_soil_intervals,
		patch[0].std * std_scale, 
		patch[0].sat_deficit,
		total_gamma, 
		patch[0].soil_defaults[0][0].interval_size,
		patch[0].transmissivity_profile,
		patch);
		
	if (route_to_patch < 0.0) route_to_patch = 0.0;
	if ( route_to_patch > available_sat_water) 
		route_to_patch *= (available_sat_water)/(route_to_patch);
	/*--------------------------------------------------------------*/
	/* compute Nitrogen leaching amount				*/
	/*--------------------------------------------------------------*/
	if (command_line[0].grow_flag > 0) {
		Nout = compute_N_leached(
			verbose_flag,
			patch[0].soil_ns.nitrate,
			route_to_patch / patch[0].area,
			patch[0].sat_deficit,
			patch[0].soil_defaults[0][0].soil_water_cap,
			m,
			total_gamma / patch[0].area * time_int,
			patch[0].soil_defaults[0][0].porosity_0,
			patch[0].soil_defaults[0][0].porosity_decay,
			patch[0].soil_defaults[0][0].N_decay_rate,
			patch[0].soil_defaults[0][0].active_zone_z,
			patch[0].soil_defaults[0][0].soil_depth,
			patch[0].soil_defaults[0][0].NO3_adsorption_rate,
			patch[0].transmissivity_profile);
		NO3_leached_to_patch = Nout * patch[0].area;
		patch[0].soil_ns.NO3_Qout += Nout;


		Nout = compute_N_leached(
			verbose_flag,
			patch[0].soil_ns.sminn,
			route_to_patch / patch[0].area,
			patch[0].sat_deficit,
			patch[0].soil_defaults[0][0].soil_water_cap,
			m,
			total_gamma / patch[0].area * time_int,
			patch[0].soil_defaults[0][0].porosity_0,
			patch[0].soil_defaults[0][0].porosity_decay,
			patch[0].soil_defaults[0][0].N_decay_rate,
			patch[0].soil_defaults[0][0].active_zone_z,
			patch[0].soil_defaults[0][0].soil_depth,
			patch[0].soil_defaults[0][0].NH4_adsorption_rate,
			patch[0].transmissivity_profile);
		NH4_leached_to_patch = Nout * patch[0].area;
		patch[0].soil_ns.NH4_Qout += Nout;

		Nout = compute_N_leached(
			verbose_flag,
			patch[0].soil_ns.DON,
			route_to_patch / patch[0].area,
			patch[0].sat_deficit,
			patch[0].soil_defaults[0][0].soil_water_cap,
			m,
			total_gamma / patch[0].area * time_int,
			patch[0].soil_defaults[0][0].porosity_0,
			patch[0].soil_defaults[0][0].porosity_decay,
			patch[0].soil_defaults[0][0].DOM_decay_rate,
			patch[0].soil_defaults[0][0].active_zone_z,
			patch[0].soil_defaults[0][0].soil_depth,
			patch[0].soil_defaults[0][0].DON_adsorption_rate,
			patch[0].transmissivity_profile);
		DON_leached_to_patch = Nout * patch[0].area;
		patch[0].soil_ns.DON_Qout += Nout;

		Nout = compute_N_leached(
			verbose_flag,
			patch[0].soil_cs.DOC,
			route_to_patch / patch[0].area,
			patch[0].sat_deficit,
			patch[0].soil_defaults[0][0].soil_water_cap,
			m,
			total_gamma / patch[0].area * time_int,
			patch[0].soil_defaults[0][0].porosity_0,
			patch[0].soil_defaults[0][0].porosity_decay,
			patch[0].soil_defaults[0][0].DOM_decay_rate,
			patch[0].soil_defaults[0][0].active_zone_z,
			patch[0].soil_defaults[0][0].soil_depth,
			patch[0].soil_defaults[0][0].DOC_adsorption_rate,
			patch[0].transmissivity_profile);
		DOC_leached_to_patch = Nout * patch[0].area;
		patch[0].soil_cs.DOC_Qout += Nout;


	}

	
	patch[0].Qout += (route_to_patch / patch[0].area);

	/*--------------------------------------------------------------*/
	/*	calculate any return flow associated with this patch	*/
	/*	and route any infiltration excess			*/
	/*	return flow is flow leaving patch (i.e surface_Qout)  	*/
	/*	note that return flow that becomes detention storage   */
	/*	is added to surface_Qin					*/
	/*	similarly with associated nitrogen			*/
	/* 	note we move unsat_storage into saturated storage in this case */
	/*	saturated zone will be updated in compute_subsurface_routing	*/
	/*	i.e becomes part of Qout				*/
	/*--------------------------------------------------------------*/
	if ((patch[0].sat_deficit-patch[0].rz_storage-patch[0].unsat_storage) < -1.0*ZERO) {
		return_flow = compute_varbased_returnflow(patch[0].std * std_scale, 
			patch[0].rz_storage+patch[0].unsat_storage,
			patch[0].sat_deficit, &(patch[0].litter));
		patch[0].detention_store += return_flow;  
		patch[0].sat_deficit += (return_flow - (patch[0].unsat_storage+patch[0].rz_storage));
		patch[0].unsat_storage = 0.0;
		patch[0].rz_storage = 0.0;
	}
	
	/*--------------------------------------------------------------*/
	/*	calculated any N-transport associated with return flow  */
	/*	-note available N reduced by what has already been 	*/
	/*	we assume that only nitrate follows return flow		*/
	/*	lost in subsurface flow routing				*/
	/*--------------------------------------------------------------*/
		if (command_line[0].grow_flag > 0) {
			Nout = compute_N_leached(
				verbose_flag,
				patch[0].soil_ns.nitrate - (NO3_leached_to_patch/patch[0].area),
				return_flow,
				0.0,
				0.0,
				m,
				total_gamma / patch[0].area * time_int,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].N_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].soil_depth,
				patch[0].soil_defaults[0][0].NO3_adsorption_rate,
				patch[0].transmissivity_profile);
			patch[0].surface_NO3 += Nout;
			patch[0].soil_ns.NO3_Qout += Nout;

			Nout = compute_N_leached(
				verbose_flag,
				patch[0].soil_ns.sminn - (NH4_leached_to_patch/patch[0].area),
				return_flow,
				0.0,
				0.0,
				m,
				total_gamma / patch[0].area * time_int,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].N_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].soil_depth,
				patch[0].soil_defaults[0][0].NH4_adsorption_rate,
				patch[0].transmissivity_profile);
			patch[0].surface_NH4 += Nout;
			patch[0].soil_ns.NH4_Qout += Nout;


			Nout = compute_N_leached(
				verbose_flag,
				patch[0].soil_ns.DON - (DON_leached_to_patch/patch[0].area),
				return_flow,
				0.0,
				0.0,
				m,
				total_gamma / patch[0].area * time_int,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].DOM_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].soil_depth,
				patch[0].soil_defaults[0][0].DON_adsorption_rate,
				patch[0].transmissivity_profile);
			patch[0].surface_DON += Nout;
			patch[0].soil_ns.DON_Qout += Nout;

			Nout = compute_N_leached(
				verbose_flag,
				patch[0].soil_cs.DOC - (DOC_leached_to_patch/patch[0].area),
				return_flow,
				0.0,
				0.0,
				m,
				total_gamma / patch[0].area * time_int,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].DOM_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].soil_depth,
				patch[0].soil_defaults[0][0].DOC_adsorption_rate,
				patch[0].transmissivity_profile);
			patch[0].surface_DOC += Nout;
			patch[0].soil_cs.DOC_Qout += Nout;
		}
	


	/*--------------------------------------------------------------*/
	/*	route water and nitrogen lossed due to surface routing    */
	/*--------------------------------------------------------------*/

     /*-----------------------------------------------------------*/
     /* Need to compute total outflow at a sub-houlry time step   */
     /* Use one minute                                            */
     /* Assume inflow/infil in linearly distributed across step   */
     /*-----------------------------------------------------------*/
     preday_scm_volume  = patch[0].detention_store - patch[0].surface_Qin;
     preday_scm_inflow  = patch[0].preday_scm_inflow;
     preday_scm_outflow = patch[0].preday_scm_outflow;
     /*-----------------------------------------------------------*/
     /* A) IF HOURLY                                              */
     /*-----------------------------------------------------------*/
     if (zone_hourly_flag == 1) {
          intused = 3;
          step = 60;
     } else {
          intused = 4;
          step = 60*24;
     }

     /*-----------------------------------------------------------*/
     /* SUB-HOURLY FOR LOOP */
     /*-----------------------------------------------------------*/
     scm_outflow = 0;

     for (mins=0; mins<step; mins++){
          
         scm_outflow_tmp = 0;
         
         // Calculate LHS in m/minute
         // LHS = Ii + Ij + (2*Si/t) - Oi        [t=1 minute]
         LHS = preday_scm_inflow/step + patch[0].surface_Qin/step + (2*preday_scm_volume)/1 - preday_scm_outflow/step;
         /*-----------------------------------------------------------*/
         /* calculate amuount of water output to stream (SCM routing) */
         /*-----------------------------------------------------------*/

         /* for loop to lookup scm depth and outflow based on LHS */
         for (k=0; k<(patch[0].scm_defaults[0][0].num_discrete + 1); k++) {
         
               // Check to see if LHS is above max
               if (LHS >= patch[0].scm_stage_storage[patch[0].scm_defaults[0][0].num_discrete][5]) {
                    //scm_outflow_tmp  = ((preday_scm_volume + patch[0].surface_Qin/step) - patch[0].scm_stage_storage[patch[0].scm_defaults[0][0].num_discrete][2]); // immediately route excess volume to stream
                    scm_outflow_tmp += (patch[0].scm_stage_storage[patch[0].scm_defaults[0][0].num_discrete][intused]/step) + patch[0].surface_Qin/step ;  // set outflow rate to that of the maximum depth water and short-circuit extra water out
                    scm_H = patch[0].scm_stage_storage[patch[0].scm_defaults[0][0].num_discrete][0];             // set water level to maximum
                    avek = patch[0].scm_defaults[0][0].num_discrete;
                    break;
              
               
                 } else if (LHS >= patch[0].scm_stage_storage[k][5] && LHS < patch[0].scm_stage_storage[k+1][5]) {
                    h_low = patch[0].scm_stage_storage[k][0];
                    h_hi  = patch[0].scm_stage_storage[k+1][0];
                    out_low = patch[0].scm_stage_storage[k][intused]/step;
                    out_hi  = patch[0].scm_stage_storage[k+1][intused]/step;
                    vol_low = patch[0].scm_stage_storage[k][2];
                    vol_hi  = patch[0].scm_stage_storage[k+1][2];
                    
                    LHS_delta = (LHS - patch[0].scm_stage_storage[k][5])/(patch[0].scm_stage_storage[k+1][5]-patch[0].scm_stage_storage[k][5]);
                    scm_H = h_low + (h_hi-h_low)*LHS_delta;
                    scm_outflow_tmp = out_low + (out_hi-out_low)*LHS_delta;
                    avek = k;
                    break;
               } else {
               		avek=0;
               }
             
         }
         scm_outflow_tmp    = max(scm_outflow_tmp,0);
         scm_volume         = preday_scm_volume - scm_outflow_tmp + patch[0].surface_Qin/step;
        
         preday_scm_volume  = scm_volume;
         preday_scm_outflow = scm_outflow_tmp;
         preday_scm_inflow  = patch[0].surface_Qin; // after the first step, preday inflow is just the inflow
         scm_outflow       += scm_outflow_tmp;
 			
     }
	// Make sure Qout is not greater than available water (detention store) or less than the amount required to bring the scm back to its parameterized maximum depth
     Qout = max( min(scm_outflow,patch[0].detention_store), (-patch[0].scm_stage_storage[patch[0].scm_defaults[0][0].num_discrete][2] + patch[0].detention_store) );
     patch[0].scm_H = scm_H;

	 /*-----------------------------------------------------------*/
	 /* compute constituent surface routing: Qout/detention_store */
	 /*-----------------------------------------------------------*/  
     if (command_line[0].grow_flag > 0) {        
		// Loop through layers / strata looking for ALGAE stratum type to determine how much should be routed
		algae_leached_to_surface = 0; // set to zero because its additive 
		algaeN_leached_to_surface = 0; // set to zero because its additive 
		algaeC_leached_to_surface = 0; // set to zero because its additive 

		if(patch[0].detention_store > 0 && Qout > 0){
		
		
		  Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_DOC;
          DOC_leached_to_surface = Nout * patch[0].area;
          patch[0].surface_DOC -= Nout;
          patch[0].surface_DOC_Qout = Nout;
          
          Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_DON;
          DON_leached_to_surface = Nout * patch[0].area;
          patch[0].surface_DON -= Nout;
          patch[0].surface_DON_Qout = Nout;
          
          Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_NO3;
          NO3_leached_to_surface = Nout * patch[0].area;
          patch[0].surface_NO3 -= Nout;
          patch[0].surface_NO3_Qout = Nout;
          
          Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_NH4;
          NH4_leached_to_surface = Nout * patch[0].area;
          patch[0].surface_NH4 -= Nout;
          patch[0].surface_NH4_Qout = Nout;
			
			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
				for ( stratum=0 ; stratum<patch[0].layers[layer].count; stratum++ ){
					if( patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->defaults[0][0].epc.veg_type == ALGAE ){
						Aout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.chla;	          		
						Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.chla / patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->defaults[0][0].algae.chla_to_N;
						Cout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.chla / patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->defaults[0][0].algae.chla_to_C;
						algae_leached_to_surface += Aout * patch[0].area; 		// do += in case there are more than one ALGAE layer... unlikely
						algaeN_leached_to_surface += Nout * patch[0].area; 		// do += in case there are more than one ALGAE layer... unlikely
						algaeC_leached_to_surface += Cout * patch[0].area; 		// do += in case there are more than one ALGAE layer... unlikely
						patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.chla -= Aout;
						patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.Aout = Aout;
						patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.totalN -= Nout;
						patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.Nout = Nout;
						patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.totalC -= Cout;
						patch[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.Cout = Cout;
					}       		
				}
			}
		} else {
		    patch[0].surface_DOC_Qout = 0;
		    patch[0].surface_DON_Qout = 0;
		    patch[0].surface_NO3_Qout = 0;
		    patch[0].surface_NH4_Qout = 0;
		}
    }

     patch[0].detention_store -= Qout;  // Should already have had Qin added to it
     patch[0].surface_Qout = Qout;
     route_to_surface = Qout * patch[0].area;
	 
     if(avek > 0){
          patch[0].scm_ave_height = (patch[0].detention_store*patch[0].area)/((patch[0].scm_stage_storage[0][1]+patch[0].scm_stage_storage[avek][1])/2);
     } else {
          patch[0].scm_ave_height = 0;
     }
	
	// PRINT OUT HOURLY (or daily) WATER BALANCE:
	//fprintf(stderr,"\n Day|%d|Hour|%d|det|%f|Qin|%f|Qout|%f|RTS|%f|SS_Qin|%f|SS_Qout|%f", current_date.day,current_date.hour,patch[0].detention_store,patch[0].surface_Qin,patch[0].surface_Qout,route_to_surface,patch[0].Qin,patch[0].Qout);
	
	
	
  	if (NO3_leached_to_surface < 0.0)
		printf("WARNING %d %lf",patch[0].ID, NO3_leached_to_surface);


	/*--------------------------------------------------------------*/
	/*	calculated any DOC, DON settling       */
     /* Add these settling amounts to the surface flux trackers,     */
     /* and the soil flux trackers, and the soil state variable      */
	/*--------------------------------------------------------------*/
     
     // COMMENTED OUT FOR NOW: ASSUME DISSOLVED FRACTIONS DONT SETTLE - CAN EAILY ADD I BACK LATER
     /*if (command_line[0].grow_flag > 0) {
                
          Nout = compute_N_settled(
               patch[0].surface_DON,
               patch[0].scm_defaults[0][0].DON_settling_rate,
               patch[0].scm_ave_height);
          patch[0].surface_DON -= Nout;
          patch[0].surface_DON_settled += Nout;
          patch[0].soil_ns.DON_settled += Nout;
          patch[0].soil_ns.DON_Qin += Nout;
          patch[0].soil_ns.DON += Nout;
          
          
          Nout = compute_N_settled(
               patch[0].surface_DOC,
               patch[0].scm_defaults[0][0].DOC_settling_rate,
               patch[0].scm_ave_height);
          patch[0].surface_DOC -= Nout;
          patch[0].surface_DOC_settled += Nout;
          patch[0].soil_cs.DOC_settled += Nout;
          patch[0].soil_cs.DOC_Qin += Nout;
          patch[0].soil_cs.DOC += Nout;
          
        }


	*/
	
	
	/*--------------------------------------------------------------*/
	/*	route flow to neighbours				*/
	/*	route n_leaching if grow flag specfied			*/
	/*--------------------------------------------------------------*/
 
	/*--------------------------------------------------------------*/
	/* regular downslope routing */
	/*--------------------------------------------------------------*/
	if (command_line[0].noredist_flag == 0) {
	d=0;
	for (j = 0; j < patch[0].innundation_list[d].num_neighbours; j++) {
		neigh = patch[0].innundation_list[d].neighbours[j].patch;  
		/*--------------------------------------------------------------*/
		/* first transfer subsurface water and nitrogen */
		/*--------------------------------------------------------------*/
		Qin =	(patch[0].innundation_list[d].neighbours[j].gamma * route_to_patch) / neigh[0].area;
		if (Qin < 0) printf("\n warning negative routing from patch %d with gamma %lf", patch[0].ID, total_gamma);
		if (command_line[0].grow_flag > 0) {
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * DON_leached_to_patch) 
				/ neigh[0].area;
			neigh[0].soil_ns.DON_Qin += Nin;
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * DOC_leached_to_patch) 
				/ neigh[0].area;
			neigh[0].soil_cs.DOC_Qin += Nin;
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * NO3_leached_to_patch) 
				/ neigh[0].area;
			neigh[0].soil_ns.NO3_Qin += Nin;
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * NH4_leached_to_patch) 
				/ neigh[0].area;
			neigh[0].soil_ns.NH4_Qin += Nin;
			}
		neigh[0].Qin += Qin;
		
	}

	/*--------------------------------------------------------------*/
	/* surface downslope routing */
	/*--------------------------------------------------------------*/
	

	/*--------------------------------------------------------------*/
	/* determine which innundation depth to consider		*/
	/*--------------------------------------------------------------*/
	if (patch[0].num_innundation_depths > 0) {
		innundation_depth = patch[0].detention_store + route_to_surface/patch[0].area; 
		d=0;
		while ((innundation_depth > patch[0].innundation_list[d].critical_depth) 
			&& (d < patch[0].num_innundation_depths-1)) {
			d++;}	
	}
	else d=0;
	for (j = 0; j < patch[0].surface_innundation_list[d].num_neighbours; j++) {

		neigh = patch[0].surface_innundation_list[d].neighbours[j].patch;

		/*--------------------------------------------------------------*/
		/* now transfer surface water and nitrogen 						*/
		/*	- first nitrogen											*/
		/* FOR ALGAE transfer: 											*/
		/* First, check to see if the DS patch is an SCM patch			*/
		/* If it is - add to the surface algae store					*/
		/* if it isn't - add to the most labile litter pool             */
		/*--------------------------------------------------------------*/
		if (command_line[0].grow_flag > 0) {
			Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * NO3_leached_to_surface) / neigh[0].area;
			neigh[0].surface_NO3 += Nin;
			neigh[0].surface_NO3_Qin += Nin;
			
			Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * NH4_leached_to_surface) / neigh[0].area;
			neigh[0].surface_NH4 += Nin;
			neigh[0].surface_NH4_Qin += Nin;
						
			Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * DON_leached_to_surface) / neigh[0].area;
			neigh[0].surface_DON += Nin;
			neigh[0].surface_DON_Qin += Nin;
			
			Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * DOC_leached_to_surface) / neigh[0].area;
			neigh[0].surface_DOC += Nin;
			neigh[0].surface_DOC_Qin += Nin;
		
			/*--------------------------------------------------------------*/
			/* LOOP THROUGH NEIGHBOR LAYERS LOOKING FOR ALGAE 				*/
			/* If there are more than one ALGAE layer, too bad - it gets added to first one it finds */
			/* Also - if the algae C/N ratios are different - it maximizes algae mass transfer, puts the rest in patch.DON/DOC */
			/*--------------------------------------------------------------*/
				if(algae_leached_to_surface > 0 ){
					algae_flag = 0;
					if(command_line[0].scm_flag == 1) {
						for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
							for ( stratum=0 ; stratum<patch[0].layers[layer].count; stratum++ ){
								if(neigh[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->defaults[0][0].epc.veg_type == ALGAE ){						
									algae_flag = 1;
									// Find MASS ratios											
									neigh_CtoN = (neigh[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->defaults[0][0].algae.chla_to_N)/(neigh[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->defaults[0][0].algae.chla_to_C);
									uphill_CtoN = (algaeC_leached_to_surface) / (algaeN_leached_to_surface);
									// Are  C/N ratios approximately the same?
									// If so - add all of algae flux to DS algae store
									if ((neigh_CtoN-uphill_CtoN) < 0.1 && (neigh_CtoN-uphill_CtoN) > -0.1) {
										Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * algae_leached_to_surface) / neigh[0].area;
										neigh[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.chla += Nin;
										neigh[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.Ain += Nin;
										// break loop so it only adds to one layer
										break;
									} 
									// Does the downhill SCM have a smaller MOLAR C/N ratio?
									// If so - add extra C to DS DOC pool, adjust algae flux by ratio of DOC to US C
							
									else if((neigh_CtoN-uphill_CtoN) <= -0.01) {
										Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * (algae_leached_to_surface * neigh_CtoN/uphill_CtoN)) / neigh[0].area;
										neigh[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.chla += Nin;
										neigh[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.Ain += Nin;
										neigh[0].surface_DOC += ((uphill_CtoN-neigh_CtoN)/uphill_CtoN*algaeC_leached_to_surface) / neigh[0].area;
										neigh[0].surface_DOC_Qin += ((uphill_CtoN-neigh_CtoN)/uphill_CtoN*algaeC_leached_to_surface) / neigh[0].area;
										// break loop so it only adds to one layer
										break;
									}
									// Does the downhill SCM have a greater C/N ratio?
									// If so - add extra N to DS DON pool, adjust algae flux by ratio of DON to US N
							 
									else {
										Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * (algae_leached_to_surface - (   ((-uphill_CtoN+neigh_CtoN)/uphill_CtoN) / (neigh_CtoN/uphill_CtoN)  * algae_leached_to_surface))) / neigh[0].area;
										neigh[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.chla += Nin;
										neigh[0].canopy_strata[(patch[0].layers[layer].strata[stratum])]->algae.Ain += Nin;
										neigh[0].surface_DON += ((-uphill_CtoN+neigh_CtoN)/uphill_CtoN*algaeN_leached_to_surface) / neigh[0].area;
										neigh[0].surface_DON_Qin += ((-uphill_CtoN+neigh_CtoN)/uphill_CtoN*algaeN_leached_to_surface) / neigh[0].area;									
										// break loop so it only adds to one layer
										break;
									}
						
								} 
							}						
						}
					// If no algae stratum were found
					if(algae_flag == 0) {
						// If neighbor is a stream, add algae to DOC/DON
						if(neigh[0].drainage_type == STREAM){
							neigh[0].surface_DON += (patch[0].surface_innundation_list[d].neighbours[j].gamma * algaeN_leached_to_surface) / neigh[0].area;
							neigh[0].surface_DON_Qin += (patch[0].surface_innundation_list[d].neighbours[j].gamma * algaeN_leached_to_surface) / neigh[0].area;

							neigh[0].surface_DOC += (patch[0].surface_innundation_list[d].neighbours[j].gamma * algaeC_leached_to_surface) / neigh[0].area;
							neigh[0].surface_DOC_Qin += (patch[0].surface_innundation_list[d].neighbours[j].gamma * algaeC_leached_to_surface) / neigh[0].area;
						} 
						// Add to labile litter stores if it is a land or road
						else {
							neigh[0].litter_ns.litr1n += (patch[0].surface_innundation_list[d].neighbours[j].gamma * algaeN_leached_to_surface) / neigh[0].area;
							neigh[0].litter_cs.litr1c +=  (patch[0].surface_innundation_list[d].neighbours[j].gamma * algaeC_leached_to_surface) / neigh[0].area;
						}
					}

					}
				}
		}			
		
		
		/*--------------------------------------------------------------*/
		/*	- now surface water 					*/
		/*	surface stores should be updated to facilitate transfer */
		/* added net surface water transfer to detention store		*/
		/*--------------------------------------------------------------*/

		Qin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * route_to_surface) / neigh[0].area;
		//if(patch[0].ID==2){
		//if(neigh[0].ID==1){
		//fprintf(stderr, "\n Surface from 2: %f  ", Qin);
		//}
		// }
		neigh[0].detention_store += Qin;
		neigh[0].surface_Qin += Qin;
		
		
		/*--------------------------------------------------------------*/
		/* try to infiltrate this water					*/ 
		/* use time_int as duration */
		/*--------------------------------------------------------------*/
		

		/*--------------------------------------------------------------*/
		/* If SCM Flag is on - call different logic for infiltration	*/
		/*   a bit repetitive, but there may not be an "scm.default.ID" */
		/*    without flag												*/
		/*--------------------------------------------------------------*/	
		if(command_line[0].scm_flag == 1){
			
			// IF NOT AN SCM NEIGHBOR  (default ID = 0)- follow regular routing
			if(neigh[0].scm_defaults[0][0].ID == 0) {
			
				if (neigh[0].detention_store > ZERO) {
					if (neigh[0].rootzone.depth > ZERO) {
						infiltration = compute_infiltration(
							verbose_flag,
							neigh[0].sat_deficit_z,
							neigh[0].rootzone.S,
							neigh[0].Ksat_vertical,
							neigh[0].soil_defaults[0][0].Ksat_0_v,
							neigh[0].soil_defaults[0][0].mz_v,
							neigh[0].soil_defaults[0][0].porosity_0,
							neigh[0].soil_defaults[0][0].porosity_decay,
							(neigh[0].detention_store),	
							time_int,
							neigh[0].soil_defaults[0][0].psi_air_entry);
					}
					else {
					infiltration = compute_infiltration(
						verbose_flag,
						neigh[0].sat_deficit_z,
						neigh[0].S,
						neigh[0].Ksat_vertical,
						neigh[0].soil_defaults[0][0].Ksat_0_v,
						neigh[0].soil_defaults[0][0].mz_v,
						neigh[0].soil_defaults[0][0].porosity_0,
						neigh[0].soil_defaults[0][0].porosity_decay,
						(neigh[0].detention_store),	
						time_int,
						neigh[0].soil_defaults[0][0].psi_air_entry);
					}
				}
				else infiltration = 0.0;
			
			// IF NEIGHBOR IS AN SCM - INFILTRATE WATER BASED ON PARAMETERIZED RATE
			} else {
				if (neigh[0].detention_store > ZERO) {
					if (neigh[0].rootzone.depth > ZERO) {
						infiltration = compute_infiltration_scm(
							verbose_flag,
							neigh[0].sat_deficit_z,
							neigh[0].rootzone.S,
							neigh[0].Ksat_vertical,
							neigh[0].soil_defaults[0][0].Ksat_0_v,
							neigh[0].soil_defaults[0][0].mz_v,
							neigh[0].soil_defaults[0][0].porosity_0,
							neigh[0].soil_defaults[0][0].porosity_decay,
							(neigh[0].detention_store),	
							time_int,
							neigh[0].soil_defaults[0][0].psi_air_entry,
							neigh[0].scm_defaults[0][0].infil_rate);
					}
					else {
						infiltration = compute_infiltration_scm(
							verbose_flag,
							neigh[0].sat_deficit_z,
							neigh[0].S,
							neigh[0].Ksat_vertical,
							neigh[0].soil_defaults[0][0].Ksat_0_v,
							neigh[0].soil_defaults[0][0].mz_v,
							neigh[0].soil_defaults[0][0].porosity_0,
							neigh[0].soil_defaults[0][0].porosity_decay,
							(neigh[0].detention_store),	
							time_int,
							neigh[0].soil_defaults[0][0].psi_air_entry,
							neigh[0].scm_defaults[0][0].infil_rate);
					}
				}
				else infiltration = 0.0;
			}

		/*--------------------------------------------------------------*/
		/* if no scm, follow regular infiltration logic					*/
		/*--------------------------------------------------------------*/	

		} else {

			if (neigh[0].detention_store > ZERO) {
				if (neigh[0].rootzone.depth > ZERO) {
				infiltration = compute_infiltration(
					verbose_flag,
					neigh[0].sat_deficit_z,
					neigh[0].rootzone.S,
					neigh[0].Ksat_vertical,
					neigh[0].soil_defaults[0][0].Ksat_0_v,
					neigh[0].soil_defaults[0][0].mz_v,
					neigh[0].soil_defaults[0][0].porosity_0,
					neigh[0].soil_defaults[0][0].porosity_decay,
					(neigh[0].detention_store),	
					time_int,
					neigh[0].soil_defaults[0][0].psi_air_entry);
				}
				else {
				infiltration = compute_infiltration(
					verbose_flag,
					neigh[0].sat_deficit_z,
					neigh[0].S,
					neigh[0].Ksat_vertical,
					neigh[0].soil_defaults[0][0].Ksat_0_v,
					neigh[0].soil_defaults[0][0].mz_v,
					neigh[0].soil_defaults[0][0].porosity_0,
					neigh[0].soil_defaults[0][0].porosity_decay,
					(neigh[0].detention_store),	
					time_int,
					neigh[0].soil_defaults[0][0].psi_air_entry);
				}
			}
			else infiltration = 0.0;
		}
		
		/*--------------------------------------------------------------*/
		/* added an surface N flux to surface N pool	and		*/
		/* allow infiltration of surface N				*/
		/*--------------------------------------------------------------*/
		if ((command_line[0].grow_flag > 0 ) && (infiltration > ZERO)) {
			neigh[0].soil_cs.DOC_Qin += ((infiltration / neigh[0].detention_store) * neigh[0].surface_DOC);
			neigh[0].surface_DOC -= ((infiltration / neigh[0].detention_store) * neigh[0].surface_DOC);
			neigh[0].soil_ns.DON_Qin += ((infiltration / neigh[0].detention_store) * neigh[0].surface_DON);
			neigh[0].surface_DON -= ((infiltration / neigh[0].detention_store) * neigh[0].surface_DON);
			neigh[0].soil_ns.NO3_Qin += ((infiltration / neigh[0].detention_store) * neigh[0].surface_NO3);
			neigh[0].surface_NO3 -= ((infiltration / neigh[0].detention_store) * neigh[0].surface_NO3);
			neigh[0].soil_ns.NH4_Qin += ((infiltration / neigh[0].detention_store) * neigh[0].surface_NH4);
			neigh[0].surface_NH4 -= ((infiltration / neigh[0].detention_store) * neigh[0].surface_NH4);
		}

		if (infiltration > neigh[0].sat_deficit - neigh[0].unsat_storage - neigh[0].rz_storage) {
			neigh[0].sat_deficit -= (infiltration + neigh[0].unsat_storage + neigh[0].rz_storage);
			neigh[0].unsat_storage = 0.0; 
			neigh[0].rz_storage = 0.0; 
			neigh[0].field_capacity = 0.0; 
			neigh[0].rootzone.field_capacity = 0.0; 
		}

		else if ((neigh[0].sat_deficit > neigh[0].rootzone.potential_sat) &&
			(infiltration > neigh[0].rootzone.potential_sat - neigh[0].rz_storage)) {
		/*------------------------------------------------------------------------------*/
		/*		Just add the infiltration to the rz_storage and unsat_storage	*/
		/*------------------------------------------------------------------------------*/
			neigh[0].unsat_storage += infiltration - (neigh[0].rootzone.potential_sat - neigh[0].rz_storage);
			neigh[0].rz_storage = neigh[0].rootzone.potential_sat;
		}								
		/* Only rootzone layer saturated - perched water table case */
		else if ((neigh[0].sat_deficit > neigh[0].rootzone.potential_sat) &&
			(infiltration <= neigh[0].rootzone.potential_sat - neigh[0].rz_storage)) {
			/*--------------------------------------------------------------*/
			/*		Just add the infiltration to the rz_storage	*/
			/*--------------------------------------------------------------*/
			neigh[0].rz_storage += infiltration;
		}
		else if ((neigh[0].sat_deficit <= neigh[0].rootzone.potential_sat) &&
			(infiltration <= neigh[0].sat_deficit - neigh[0].rz_storage - neigh[0].unsat_storage)) {
			neigh[0].rz_storage += neigh[0].unsat_storage;		
			/* transfer left water in unsat storage to rootzone layer */
			neigh[0].unsat_storage = 0;
			neigh[0].rz_storage += infiltration;
			neigh[0].field_capacity = 0;
		}
		neigh[0].detention_store -= infiltration;

	}

	}
     /* end if redistribution flag */
	return;

} /*end update_drainage_scm.c*/

