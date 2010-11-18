/*--------------------------------------------------------------*/
/* 											*/
/*					update_drainage_road			*/
/*											*/
/*	update_drainage_road.c - creates a patch object				*/
/*											*/
/*	NAME										*/
/*	update_drainage_road.c - creates a patch object				*/
/*											*/
/*	SYNOPSIS									*/
/*	void update_drainage_road( 							*/
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
/*	PROGRAMMER NOTES								*/
/*											*/
/*											*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"


void  update_drainage_road(
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
		double, double *);
	
	double compute_varbased_flow(
		int,
		double,
		double,
		double,
		double *);

	double recompute_gamma(	
		struct patch_object *,
		double);


	double compute_varbased_returnflow(
		double,
		double,
		double,
		struct litter_object *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	int i, j,k,d;
	double m, Ksat, return_flow;
	double N_leached_to_patch, N_leached_to_stream, N_surface_leached_to_stream; /* kg/m2 */
	double N_leached_total; /* kg/m2 */
	double route_to_stream;  /* m3 */
	double route_to_patch;  /* m3 */
	double road_int_depth;  /* m of H2O */
	double available_sat_water, route_total; /* m3 */
	double  Qin, Qout, Qstr_total;  /* m */
	double total_gamma, percent_loss;
	double Nin, Nout; /* kg/m2 */ 
	double percent_tobe_routed;

	struct patch_object *neigh;

	N_leached_to_patch = 0.0;
	N_leached_to_stream = 0.0;
	N_surface_leached_to_stream = 0.0;
	route_to_stream = 0.0;
	route_to_patch = 0.0;


	/*--------------------------------------------------------------*/
	/*	m and K are multiplied by sensitivity analysis variables */
	/*--------------------------------------------------------------*/
	m = patch[0].soil_defaults[0][0].m  ;
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

	/*--------------------------------------------------------------*/
	/*	for roads, saturated throughflow beneath road cut	*/
	/*	is routed to downslope patches; saturated throughflow	*/
	/*	above the cut and overland flow is routed to the stream	*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/*	calculate water_equivalent depth of road		*/
	/*--------------------------------------------------------------*/
	road_int_depth = compute_delta_water(
		verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		patch[0].road_cut_depth,
		0.0);
	if (road_int_depth > patch[0].sat_deficit) {
	/*------------------------------------------------------------*/
	/*	calculate amuount of water output to patches			*/
	/*-----------------------------------------------------------*/
		route_to_patch =  time_int * compute_varbased_flow(
			patch[0].num_soil_intervals,
			patch[0].std * command_line[0].std_scale, 
			road_int_depth,
			total_gamma, 
			patch[0].transmissivity_profile);

		/*-----------------------------------------------------------*/
		/*	calculate amuount of water output to stream		*/
		/*-----------------------------------------------------------*/
		route_to_stream =  time_int * compute_varbased_flow(
			patch[0].num_soil_intervals,
			patch[0].std * command_line[0].std_scale, 
			patch[0].sat_deficit,
			total_gamma, 
			patch[0].transmissivity_profile) - route_to_patch;

		if (route_to_patch < 0.0) route_to_patch = 0.0;
		if (route_to_stream < 0.0) route_to_stream = 0.0;
		if ((route_to_stream + route_to_patch) > available_sat_water) {
			route_to_patch *= (available_sat_water)/(route_to_patch + route_to_stream);
			route_to_stream *= (available_sat_water)/(route_to_patch + route_to_stream);
		}
		/*--------------------------------------------------------------*/
		/* compute Nitrogen leaching amount				*/
		/*--------------------------------------------------------------*/
		if (command_line[0].grow_flag > 0) {
			N_leached_to_patch = compute_N_leached(
				verbose_flag,
				patch[0].soil_ns.nitrate,
				route_to_patch / patch[0].area,
				road_int_depth,
				patch[0].soil_defaults[0][0].soil_water_cap,
				m,
				total_gamma / patch[0].area * time_int,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].N_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].soil_depth,
				patch[0].soil_defaults[0][0].mobile_N_proportion,
				patch[0].transmissivity_profile);
			N_leached_to_stream = compute_N_leached(
				verbose_flag,
				patch[0].soil_ns.nitrate,
				route_to_stream / patch[0].area,
				patch[0].sat_deficit,
				patch[0].soil_defaults[0][0].soil_water_cap,
				m,
				total_gamma / patch[0].area * time_int,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].N_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].soil_depth,
				patch[0].soil_defaults[0][0].mobile_N_proportion,
				patch[0].transmissivity_profile) -
				N_leached_to_patch;
			patch[0].soil_ns.Qout += (N_leached_to_patch + N_leached_to_stream);
					 
		}
		patch[0].Qout += ((route_to_patch + route_to_stream) / patch[0].area);
	}
	/*--------------------------------------------------------------*/
	/* if road is below the water_table - no routing to the stream  */
	/*--------------------------------------------------------------*/
	else {
		route_to_stream = 0.0;
		route_to_patch =  time_int * compute_varbased_flow(
			patch[0].num_soil_intervals,
			patch[0].std * command_line[0].std_scale, 
			patch[0].sat_deficit,
			total_gamma, 
			patch[0].transmissivity_profile);

		if (route_to_patch < 0.0) route_to_patch = 0.0;
		if (route_to_patch > available_sat_water) 
			route_to_patch = available_sat_water;
	/*--------------------------------------------------------------*/
	/* compute Nitrogen leaching amount				*/
	/*--------------------------------------------------------------*/
		if (command_line[0].grow_flag > 0) {
			N_leached_to_patch = compute_N_leached(
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
				patch[0].soil_defaults[0][0].mobile_N_proportion,
				patch[0].transmissivity_profile);
			N_leached_to_stream = 0.0;
			patch[0].soil_ns.Qout += (N_leached_to_patch + N_leached_to_stream);
		}

		patch[0].Qout += ((route_to_patch + route_to_stream) / patch[0].area);
	}

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
		return_flow = compute_varbased_returnflow(patch[0].std * command_line[0].std_scale, 
			patch[0].rz_storage+patch[0].unsat_storage,
			patch[0].sat_deficit, &(patch[0].litter));
		patch[0].detention_store += return_flow;  
		patch[0].sat_deficit += (return_flow - (patch[0].unsat_storage+patch[0].rz_storage));;
		patch[0].unsat_storage = 0.0;
		patch[0].rz_storage = 0.0;
	}
	/*--------------------------------------------------------------*/
	/*	calculated any N-transport associated with return flow  */
	/*	-note available N reduced by what has already been 	*/
	/*	lost in subsurface flow routing				*/
	/*	- note only nitrate is assumed to follow return flow	*/
	/*--------------------------------------------------------------*/
	if (command_line[0].grow_flag > 0) {
		Nout = compute_N_leached(
			verbose_flag,
			patch[0].soil_ns.nitrate - N_leached_to_patch,
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
			patch[0].soil_defaults[0][0].mobile_N_proportion,
			patch[0].transmissivity_profile);
		patch[0].surface_NO3 += Nout;
		patch[0].soil_ns.Qout += Nout;
		}
	
	/*--------------------------------------------------------------*/
	/*	route water and nitrogen lossed due to infiltration excess */
	/*	note we assume that this happens before return_flow losses */
	/*--------------------------------------------------------------*/
	if ((patch[0].detention_store > patch[0].soil_defaults[0][0].detention_store_size) &&
		(patch[0].detention_store > ZERO) ) {
		Qout = (patch[0].detention_store - patch[0].soil_defaults[0][0].detention_store_size);
		if (command_line[0].grow_flag > 0) {
			Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_NO3;
			patch[0].surface_NO3  -= Nout;
			patch[0].next_stream[0].streamflow_N += (Nout * patch[0].area / patch[0].next_stream[0].area);
			Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_NH4;
			patch[0].surface_NH4  -= Nout;
			patch[0].next_stream[0].streamflow_N += (Nout * patch[0].area / patch[0].next_stream[0].area);
			}
		patch[0].next_stream[0].streamflow += (Qout * patch[0].area / patch[0].next_stream[0].area);
		patch[0].detention_store -= Qout;
		}



	/*--------------------------------------------------------------*/
	/*	route flow to neighbours				*/
	/*	route n_leaching if grow flag specfied			*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/* routing to stream i.e. diversion routing */
	/*	note all surface flows go to the stream			*/
	/*--------------------------------------------------------------*/
	patch[0].next_stream[0].streamflow += (route_to_stream) / patch[0].next_stream[0].area;
	patch[0].next_stream[0].surface_Qin  += (route_to_stream) / patch[0].next_stream[0].area;

	if (patch[0].next_stream[0].ID == 14)
			printf("\n in is %lf", route_to_stream/ patch[0].next_stream[0].area);
	if (command_line[0].grow_flag > 0) {
		Nin = (N_leached_to_stream * patch[0].area) / patch[0].next_stream[0].area;
		patch[0].next_stream[0].streamflow_N += Nin;
		}

	/*--------------------------------------------------------------*/
	/*	route flow to neighbours				*/
	/*	route n_leaching if grow flag specfied			*/
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/* regular downslope routing */
	/*--------------------------------------------------------------*/
	d=0;
	for (j = 0; j < patch[0].innundation_list[d].num_neighbours; j++) {
		neigh = patch[0].innundation_list[d].neighbours[j].patch;  
		/*--------------------------------------------------------------*/
		/* first transfer subsurface water and nitrogen */
		/*--------------------------------------------------------------*/
		Qin =	(patch[0].innundation_list[d].neighbours[j].gamma * route_to_patch) / neigh[0].area;
		if (command_line[0].grow_flag > 0) {
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * N_leached_to_patch * patch[0].area) 
				/ neigh[0].area;
			neigh[0].soil_ns.Qin += Nin;
			neigh[0].ndf.sminn_leached -= Nin;
			}
		neigh[0].Qin += Qin;

			if ((neigh[0].ID == 49) && (neigh[0].detention_store < -ZERO))
				printf("\n  Qin %lf  dt %d, sat_def %lf, detention store %lf, dts %lf, returnflow %lf Qout %lf",
					Qin,
					neigh[0].drainage_type,
					neigh[0].sat_deficit,
					neigh[0].detention_store,
					neigh[0].soil_defaults[0][0].detention_store_size,
					neigh[0].return_flow,
					neigh[0].Qout);

	}

	N_leached_total = (N_leached_to_patch + N_leached_to_stream  );
	patch[0].soil_ns.nleached_snk += N_leached_total;
	patch[0].ndf.sminn_leached +=  N_leached_total; 


} /*end update_drainage_road.c*/

