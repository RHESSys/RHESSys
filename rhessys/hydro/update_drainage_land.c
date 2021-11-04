/*--------------------------------------------------------------*/
/* 											*/
/*					update_drainage_land			*/
/*											*/
/*	update_drainage_land.c - creates a patch object				*/
/*											*/
/*	NAME										*/
/*	update_drainage_land.c - creates a patch object				*/
/*											*/
/*	SYNOPSIS									*/
/*	void update_drainage_land( 							*/
/*					struct patch_object *patch			*/
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
/*	PROGRAMMER NOTES	    							*/
/*											*/
/*											*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"


void  update_drainage_land(
					struct patch_object *patch,
					 struct command_line_object *command_line,
					 double time_int,
					 int verbose_flag)
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


	/*double compute_N_leached(int,
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
		double *);*/

	double compute_Nsat_leached(
         int verbose_flag,
         double total_nitrate,
         double Qout,
         double N_decay_rate,
         double activedepthz,
         double N_absorption_rate,
         int signal,
         struct patch_object *patch);

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

	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	int j, d, idx;
	double tmp;
	double m, Ksat, std_scale;
	double NH4_leached_to_patch, NH4_leached_to_stream;
	double NO3_leached_to_patch, NO3_leached_to_stream;
	double DON_leached_to_patch, DON_leached_to_stream;
	double DOC_leached_to_patch, DOC_leached_to_stream;
	double NO3_leached_to_surface; /* kg/m2 */
	double NH4_leached_to_surface; /* kg/m2 */
	double DON_leached_to_surface; /* kg/m2 */
	double DOC_leached_to_surface; /* kg/m2 */
	double N_leached_total; /* kg/m2 */
	double DON_leached_total; /* kg/m2 */
	double DOC_leached_total; /* kg/m2 */
	double route_to_surface;  /* m3 */
	double return_flow,route_to_patch ;  /* m3 */
	double available_sat_water; /* m3 */
	double Qin, Qout;  /* m */
	double innundation_depth, infiltration; /* m */
	double total_gamma;
	double Nin, Nout; /* kg/m2 */
	double t1,t2,t3;

	struct patch_object *neigh;
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
	double sat_to_gw_coeff = patch[0].soil_defaults[0][0].sat_to_gw_coeff; //REN do I need to divide it by 24
	/*--------------------------------------------------------------*/
	/*	recalculate gamma based on current saturation deficits  */
	/*      to account the effect of changes in water table slope 	*/
	/*--------------------------------------------------------------*/
	total_gamma = recompute_gamma(patch, patch[0].innundation_list[d].gamma);

	available_sat_water = max(((patch[0].soil_defaults[0][0].soil_water_cap
			- max(patch[0].sat_deficit,0.0))
			* patch[0].area),0.0);

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
		/* add a hotspot to no-veg patches, only route water if precip 2mm */
		//rain_throughfall NREN
		/*if (patch[0].theta < patch[0].soil_defaults[0][0].rain_threshold && patch[0].soil_defaults[0][0].water_film == 1 && patch[0].canopy_strata[0][0].defaults[0][0].epc.hot_spot == 1) { // &&
			//printf("\n 2before zero patch lateral flow of [patchID]: %d, [route_to_patch] %lf", patch[0].ID, route_to_patch);
			route_to_patch = 0;
			if (patch[0].ID == 37704) printf("\n zero patch lateral flow of patchID: %d", patch[0].ID);
		} */

	/*--------------------------------------------------------------*/
	/* compute Nitrogen leaching amount		some code orignally developed by Laurence Lin		*/
	/*--------------------------------------------------------------*/
	if (command_line[0].grow_flag > 0) {

 Nout = compute_Nsat_leached(
             verbose_flag,
             patch[0].sat_NO3, //total solute <--- projected total_sat_solute
             route_to_patch / patch[0].area,//Qout (already time_int adjusted)
             patch[0].soil_defaults[0][0].N_decay_rate,
             patch[0].soil_defaults[0][0].active_zone_z, //activedepthz
             patch[0].soil_defaults[0][0].NO3_adsorption_rate, // N_absorption_rate
             1,patch); // signal, patch
		NO3_leached_to_patch = Nout * patch[0].area;
		patch[0].soil_ns.NO3_Qout += Nout;

		Nout = compute_Nsat_leached(
			verbose_flag,
			patch[0].sat_NH4,
			route_to_patch / patch[0].area,
			patch[0].soil_defaults[0][0].N_decay_rate,
            patch[0].soil_defaults[0][0].active_zone_z,
			patch[0].soil_defaults[0][0].NH4_adsorption_rate,
            1,patch);
		NH4_leached_to_patch = Nout * patch[0].area;
		patch[0].soil_ns.NH4_Qout += Nout;
        if(Nout<0 || Nout!=Nout ) printf("update_drainage_land[%d,%e]: soil NH4 (%e) flux (%e)\n", patch[0].ID,route_to_patch,patch[0].soil_ns.sminn, Nout);

		Nout = compute_Nsat_leached(
			verbose_flag,
            patch[0].sat_DON,
			route_to_patch / patch[0].area,
			patch[0].soil_defaults[0][0].DOM_decay_rate,
			patch[0].soil_defaults[0][0].active_zone_z,
			patch[0].soil_defaults[0][0].DON_adsorption_rate,
			1,patch);
		DON_leached_to_patch = Nout * patch[0].area;
		patch[0].soil_ns.DON_Qout += Nout;
        if(Nout<0 || Nout!=Nout) printf("update_drainage_land[%d,%e]: soil DON (%e) flux (%e)\n", patch[0].ID,route_to_patch,patch[0].soil_ns.DON, Nout);


		Nout = compute_Nsat_leached(
			verbose_flag,
			patch[0].sat_DOC,
			route_to_patch / patch[0].area,
			patch[0].soil_defaults[0][0].DOM_decay_rate,
			patch[0].soil_defaults[0][0].active_zone_z,
			patch[0].soil_defaults[0][0].DOC_adsorption_rate,
			14,patch);
		DOC_leached_to_patch = Nout * patch[0].area;
		patch[0].soil_cs.DOC_Qout += Nout;
        if(Nout<0 || Nout!=Nout) printf("update_drainage_land[%d,%e]: soil DOC (%e) flux (%e)\n", patch[0].ID,route_to_patch,patch[0].soil_cs.DOC, Nout);


	}


	patch[0].Qout += (route_to_patch / patch[0].area);// here change NREN 2021104


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


		/* new nitrate leaching process, originally developed by Laurence Lin */
    // leaching from surface;
    // when return_flow>0, then extrawater = patch[0].rz_storage+patch[0].unsat_storage - patch[0].sat_deficit - route_to_patch/patch[0].area + patch[0].constraintWaterTableTopDepth_def >0
		if (command_line[0].grow_flag > 0 && return_flow > ZERO) {
			Nout = compute_Nsat_leached(
				verbose_flag,
                /// problem re-project!! && "soil_ns -= going2sat_NO3"
				patch[0].sat_NO3 - patch[0].soil_ns.NO3_Qout,
				return_flow,
				patch[0].soil_defaults[0][0].N_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].NO3_adsorption_rate,
                17,patch);
			patch[0].surface_NO3 += Nout;
			patch[0].soil_ns.NO3_Qout += Nout;
            if(Nout<0 || Nout!=Nout || patch[0].soil_ns.nitrate!=patch[0].soil_ns.nitrate || patch[0].soil_ns.nitrate<0 || NO3_leached_to_patch<0 || NO3_leached_to_patch!=NO3_leached_to_patch) printf("update_drainage_land[%d,%e]: return NO3 (%e,%e) flux (%e)\n", patch[0].ID,return_flow,patch[0].soil_ns.nitrate - (NO3_leached_to_patch/patch[0].area),NO3_leached_to_patch, Nout);

			Nout = compute_Nsat_leached(
				verbose_flag,
				patch[0].sat_NH4 - patch[0].soil_ns.NH4_Qout,
				return_flow,
				patch[0].soil_defaults[0][0].N_decay_rate,
                patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].NH4_adsorption_rate,
				20,patch);
			patch[0].surface_NH4 += Nout;
			patch[0].soil_ns.NH4_Qout += Nout;
            if(Nout<0 || Nout!=Nout) printf("update_drainage_land[%d,%e]: return NH4 (%e) flux (%e)\n", patch[0].ID,return_flow,patch[0].soil_ns.sminn - (NH4_leached_to_patch/patch[0].area), Nout);

			Nout = compute_Nsat_leached(
				verbose_flag,
				patch[0].sat_DON - patch[0].soil_ns.DON_Qout,
				return_flow,
				patch[0].soil_defaults[0][0].DOM_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].DON_adsorption_rate,
				23,patch);
			patch[0].surface_DON += Nout;
			patch[0].soil_ns.DON_Qout += Nout;
            if(Nout<0 || Nout!=Nout) printf("update_drainage_land[%d,%e]: return DON (%e) flux (%e)\n", patch[0].ID,return_flow,patch[0].soil_ns.DON - (DON_leached_to_patch/patch[0].area), Nout);

			Nout = compute_Nsat_leached(
				verbose_flag,
				patch[0].sat_DOC - patch[0].soil_cs.DOC_Qout,
				return_flow,
				patch[0].soil_defaults[0][0].DOM_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].DOC_adsorption_rate,
				26,patch);
			patch[0].surface_DOC += Nout;
			patch[0].soil_cs.DOC_Qout += Nout;
            if(Nout<0 || Nout!=Nout) printf("update_drainage_land[%d,%e]: return DOC (%e) flux (%e)\n", patch[0].ID,return_flow,patch[0].soil_cs.DOC - (DOC_leached_to_patch/patch[0].area), Nout);
		}//

    patch[0].overland_flow += return_flow; //TODO: should add this to overland_flow NREN?

	/*--------------------------------------------------------------*/
	/*	route water and nitrogen lossed due to infiltration excess */
	/*--------------------------------------------------------------*/
	if ( (patch[0].detention_store > patch[0].soil_defaults[0][0].detention_store_size) &&
		(patch[0].detention_store > ZERO) ){

		patch[0].overland_flow += patch[0].detention_store - patch[0].soil_defaults[0][0].detention_store_size;

		Qout = (patch[0].detention_store - patch[0].soil_defaults[0][0].detention_store_size);
		if (command_line[0].grow_flag > 0) {
			Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_DOC;
			DOC_leached_to_surface = Nout * patch[0].area;
			patch[0].surface_DOC -= Nout;
			Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_DON;
			DON_leached_to_surface = Nout * patch[0].area;
			patch[0].surface_DON -= Nout;
			Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_NO3;
			NO3_leached_to_surface = Nout * patch[0].area;
			patch[0].surface_NO3 -= Nout;
			Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_NH4;
			NH4_leached_to_surface = Nout * patch[0].area;
			patch[0].surface_NH4 -= Nout;
			}
		route_to_surface = (Qout *  patch[0].area);
		patch[0].detention_store -= Qout;
		patch[0].surface_Qout += Qout;

		}


	/*if (NO3_leached_to_surface < 0.0)
		printf("WARNING N leaching is smaller than zero for %d %lf",patch[0].ID, NO3_leached_to_surface);*/

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
		/* first transfer subsurface water and nitrogen */  // --------- subsurface
		/*--------------------------------------------------------------*/
		/* some "transmissivity_flux2neighbour" is loss to GW_storage  */
		/*if(command_line[0].gw_flag > 0 && patch[0].soil_defaults[0][0].actionGWDRAIN == 1){
				//how do we know how fill is the GW?
				patch[0].gw_drainage += route_to_patch * sat_to_gw_coeff;// has multiplied patch[0].area // reset to zero every patch_daily_I()
				route_to_patch *= 1.0 - sat_to_gw_coeff;
		}//end of if */

		/*--------------------------------------------------------------*/
		/* first transfer subsurface water and nitrogen */
		/*--------------------------------------------------------------*/
		Qin =	(patch[0].innundation_list[d].neighbours[j].gamma * route_to_patch) / neigh[0].area;
		// hotspot patch when neighour patch sm < TH, water not go to that patch Qin =0 and Qout -Qin
		//patch is the hotspot patch and neigh is their neighbour patch
		if (neigh[0].theta < patch[0].soil_defaults[0][0].rain_threshold && patch[0].soil_defaults[0][0].water_film == 1 && patch[0].canopy_strata[0][0].defaults[0][0].epc.hot_spot == 1)
        {
            patch[0].Qout -= Qin; // first remove it from Qout
            Qin = 0.0;          //then if theta of neigh < TH make it zero
        }
		/* creat a hotspot for no-veg patches */


		if (Qin < 0) printf("\n warning negative routing from patch %d with gamma %lf", patch[0].ID, total_gamma);
		if (command_line[0].grow_flag > 0)
		{
			 /* add water more from sat zone to deep groundwater NREN 20210714*/
			 double coef;
	    /* if(command_line[0].gw_flag > 0 && patch[0].soil_defaults[0][0].actionGWDRAIN == 1)
	     {
	                     coef = 1.0 - sat_to_gw_coeff;
	                     coef /= neigh[0].area;
	                     patch[0].gw_drainage_DON += DON_leached_to_patch * sat_to_gw_coeff;// has multiplied patch[0].area
	                     patch[0].gw_drainage_DOC += DOC_leached_to_patch * sat_to_gw_coeff;// has multiplied patch[0].area
	                     patch[0].gw_drainage_NO3 += NO3_leached_to_patch * sat_to_gw_coeff;// has multiplied patch[0].area
	                     patch[0].gw_drainage_NH4 += NH4_leached_to_patch * sat_to_gw_coeff;// has multiplied patch[0].area
	       }else
	       {
	                     coef = 1.0/neigh[0].area; } */
            coef = 1.0/neigh[0].area;

           /* add extra conditions to make no nitrate route to no-veg&hotspot patches, since it is easily accumulate in there */
        if(neigh[0].canopy_strata[0][0].defaults[0][0].rout_N == 1) // make sure for no-veg default is rout_N is 0
			{
             // first check the threshold of hotspot, if neigh'sm < threshold, then Nin is zero
            if (neigh[0].theta < patch[0].soil_defaults[0][0].rain_threshold && patch[0].soil_defaults[0][0].water_film == 1 && patch[0].canopy_strata[0][0].defaults[0][0].epc.hot_spot == 1)
            {
                //DON -- be careful about the unit, is it area average or total
                patch[0].soil_ns.DON_Qout -= (patch[0].innundation_list[d].neighbours[j].gamma * DON_leached_to_patch)/patch[0].area; // first return it back
                patch[0].soil_ns.DON_Qout = max(0.0, patch[0].soil_ns.DON_Qout);
                DON_leached_to_patch = 0.0;
                //DOC
                patch[0].soil_cs.DOC_Qout -= (patch[0].innundation_list[d].neighbours[j].gamma * DOC_leached_to_patch)/patch[0].area;
                patch[0].soil_cs.DOC_Qout = max(0.0, patch[0].soil_cs.DOC_Qout);
                DOC_leached_to_patch = 0.0;
                //NO3
                patch[0].soil_ns.NO3_Qout -= (patch[0].innundation_list[d].neighbours[j].gamma * NO3_leached_to_patch)/patch[0].area;
                patch[0].soil_ns.NO3_Qout = max(0.0, patch[0].soil_ns.NO3_Qout);
                NO3_leached_to_patch = 0.0;
                //NH4
                patch[0].soil_ns.NH4_Qout -= (patch[0].innundation_list[d].neighbours[j].gamma * NH4_leached_to_patch)/patch[0].area;
                patch[0].soil_ns.NH4_Qout = max(0.0, patch[0].soil_ns.NH4_Qout);
                NH4_leached_to_patch = 0.0;
              } // end if theta < TH


			// secondly normal nitrogen routing pathway
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * DON_leached_to_patch) * coef;
			neigh[0].soil_ns.DON_Qin += Nin;
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * DOC_leached_to_patch) * coef;
			neigh[0].soil_cs.DOC_Qin += Nin;
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * NO3_leached_to_patch) * coef;
			neigh[0].soil_ns.NO3_Qin += Nin;
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * NH4_leached_to_patch) * coef;
			neigh[0].soil_ns.NH4_Qin += Nin;
			} // end if rout_N
        else if (neigh[0].canopy_strata[0][0].defaults[0][0].rout_N == 0 && patch[0].area > ZERO) // this is to control no-veg patch accumulating of nitrate
			{
			Nin = 0.0;//(patch[0].innundation_list[d].neighbours[j].gamma * DON_leached_to_patch) * coef;
			neigh[0].soil_ns.DON_Qin += Nin;
			// reduce the corrsponding Nout too
			patch[0].soil_ns.DON_Qout -= (patch[0].innundation_list[d].neighbours[j].gamma * DON_leached_to_patch)/patch[0].area;
            patch[0].soil_ns.DON_Qout = max(0.0, patch[0].soil_ns.DON_Qout);

			Nin = 0.0;//(patch[0].innundation_list[d].neighbours[j].gamma * DOC_leached_to_patch) * coef;
			neigh[0].soil_cs.DOC_Qin += Nin;
			patch[0].soil_cs.DOC_Qout -= (patch[0].innundation_list[d].neighbours[j].gamma * DOC_leached_to_patch)/patch[0].area;
            patch[0].soil_cs.DOC_Qout = max(0.0, patch[0].soil_cs.DOC_Qout);

			Nin = 0.0;//(patch[0].innundation_list[d].neighbours[j].gamma * NO3_leached_to_patch) * coef;
			neigh[0].soil_ns.NO3_Qin += Nin;
            patch[0].soil_ns.NO3_Qout -= (patch[0].innundation_list[d].neighbours[j].gamma * NO3_leached_to_patch)/patch[0].area;
            patch[0].soil_ns.NO3_Qout = max(0.0, patch[0].soil_ns.NO3_Qout);

			Nin = 0.0; //(patch[0].innundation_list[d].neighbours[j].gamma * NH4_leached_to_patch) * coef;
			neigh[0].soil_ns.NH4_Qin += Nin;
			patch[0].soil_ns.NH4_Qout -= (patch[0].innundation_list[d].neighbours[j].gamma * NH4_leached_to_patch)/patch[0].area;
            patch[0].soil_ns.NH4_Qout = max(0.0, patch[0].soil_ns.NH4_Qout);

			}


        }// 548 grow flag
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
		/* now transfer surface water and nitrogen */
		/*	- first nitrogen					*/
		/*--------------------------------------------------------------*/
		if (command_line[0].grow_flag > 0 && neigh[0].canopy_strata[0][0].defaults[0][0].rout_N == 1) {
			Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * NO3_leached_to_surface) / neigh[0].area;
			neigh[0].surface_NO3 += Nin;
			Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * NH4_leached_to_surface) / neigh[0].area;
			neigh[0].surface_NH4 += Nin;
			Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * DON_leached_to_surface) / neigh[0].area;
			neigh[0].surface_DON += Nin;
			Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * DOC_leached_to_surface) / neigh[0].area;
			neigh[0].surface_DOC += Nin;
			}
        else if (command_line[0].grow_flag > 0 && neigh[0].canopy_strata[0][0].defaults[0][0].rout_N == 0 && patch[0].area > ZERO){

 			//Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * NO3_leached_to_surface) / neigh[0].area;
			neigh[0].surface_NO3 += 0.0;
			patch[0].surface_NO3 += (patch[0].surface_innundation_list[d].neighbours[j].gamma * NO3_leached_to_surface)/patch[0].area; //add back

			//Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * NH4_leached_to_surface) / neigh[0].area;
			neigh[0].surface_NH4 += 0.0;
			patch[0].surface_NH4 += (patch[0].surface_innundation_list[d].neighbours[j].gamma * NH4_leached_to_surface)/patch[0].area;

			//Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * DON_leached_to_surface) / neigh[0].area;
			neigh[0].surface_DON += 0.0;
			patch[0].surface_DON += (patch[0].surface_innundation_list[d].neighbours[j].gamma * DON_leached_to_surface)/patch[0].area;

			//Nin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * DOC_leached_to_surface) / neigh[0].area;
			neigh[0].surface_DOC += 0.0;
            patch[0].surface_DON += (patch[0].surface_innundation_list[d].neighbours[j].gamma * DOC_leached_to_surface)/patch[0].area;



            }

		/*--------------------------------------------------------------*/
		/*	- now surface water 					*/
		/*	surface stores should be updated to facilitate transfer */
		/* added net surface water transfer to detention store		*/
		/*--------------------------------------------------------------*/

		Qin = (patch[0].surface_innundation_list[d].neighbours[j].gamma * route_to_surface) / neigh[0].area;
		neigh[0].detention_store += Qin;// need fix this
		neigh[0].surface_Qin += Qin;

		/*--------------------------------------------------------------*/
		/* try to infiltrate this water					*/
		/* use time_int as duration */
		/*--------------------------------------------------------------*/
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
		/*--------------------------------------------------------------*/
		/* added an surface N flux to surface N pool	and		*/
		/* allow infiltration of surface N				*/
		/*--------------------------------------------------------------*/
		if ((command_line[0].grow_flag > 0 ) && (infiltration > ZERO) && (neigh[0].canopy_strata[0][0].defaults[0][0].rout_N == 1)) {
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

	} /* end if redistribution flag */

	return;

} /*end update_drainage_land.c*/
