/*--------------------------------------------------------------*/
/* 								*/
/*		 	top_model2				*/
/*								*/
/*	NAME							*/
/*			top_model				*/
/*	distributes saturation deficit 				*/
/*	to all zones and patches within them in 		*/
/*	a hillslope						*/
/*								*/
/*								*/
/*	SYNOPSIS				 		*/
/*	double	top_model(					*/
/*			int,					*/
/*			double,					*/
/*			double,					*/
/*			struct hillslope_object *,		*/
/*			struct zone_object **)			*/
/*								*/
/*	returns:						*/
/*	base_flow - ( m/s water ) flux from sat zone of hillslope*/
/*			water flux boundary.			*/
/*								*/
/*	OPTIONS							*/
/*	int	verbose_flag					*/
/*	struct  hillslope_object *hillslope - points hillslopes	*/
/*	struct  zone_object **zones - points to hillslope zones	*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*      This routine calculates base_flow according to TOPMODEL  */
/*      given a exponential decline in Ksat with depth.         */
/*                                                              */
/*	As a side effect the routine also updates hillslope	*/
/*	patch's depth to the water table in consistency with	*/
/*	the base_flow.  However certain strong assumtpions are	*/
/*	used which may mean the the water table estimates will	*/
/*	be inappropriate.  The two main assumptions are:	*/
/*								*/
/*	i) uniform unsat to sat zone recharge at all patches	*/
/*	ii) fixed, known upslope contributing areas for patches	*/
/*								*/
/*      see: Beven, K., Runoff production and flood frequency in*/
/*      catchments of order n: an alternative approach,         */
/*      In Gupta, V.K. et al (eds.), Scale Problems in          */
/*      Hydrology, D. Reidel Publishing Company, 107-131, 1986. */
/*                                                              */
/*      Sivapalan, M., K. Beven, E.F. Wood, On hydrologic       */
/*      similarity 2. a scaled model of storm runoff production,*/
/*      Water Resources Research, 23, 2266- 2278, 1987.         */
/*                                                              */
/*      If a different decline in Ksat with depth is used refer */
/*      to :                                                    */
/*                                                              */
/*      Ambroise, B., Beven, K. and Freer, J.   (1996)          */
/*      Towards a generalization of the TOPMODEL concepts:      */
/*      Topographic indices of hydrological similarity.         */
/*                                                              */
/*      Note that in assuming that we can apply TOPMODEL even   */
/*      with different surface conditions given by patches and  */
/*      different climate forcings given by zones one may be    */
/*      making it very difficult to accurately fit base_flow     */
/*	and/or retain realistic estimates of local water table	*/
/*	depth.							*/
/*								*/
/*      Dont blame the varying climate or varying surface -     */
/*      blame the TOPMODEL assumptions.                         */
/*                                                              */
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*  March 4/97  C. Tague                                        */
/*      added base_flow update of sat_storages                   */
/*								*/
/*	Sep 15 1997	RAF					*/
/*	base_flow is computed here BUT as per the TOPMODEL	*/
/*	references provided above is not actually subtracted	*/
/*	from the hillslope sat deficits since TOPMODEL includes	*/
/*	this in the lateral sat zone flux 'q' from pixels at	*/
/*	the edges of the hillslope.				*/
/*								*/
/*	Sep 15 1997	RAF					*/
/*	Actually the base_flow should be stubtracted from the	*/
/*	hillslope mean sat deficit.  The reason is that the	*/
/*	mean sat deficit should represent the new value of 	*/
/*	the mean of the watertables after recharge has been	*/
/*	occurring and base_flow has been going on (after the	*/
/*	steady state conditions for this time step have been	*/
/*	achieved).  We have been changing the water table in	*/
/*	patches in this timestep already so now we have to 	*/
/*	change it due to the base_flow as well.  		*/
/*	Added a routine to estimate true depth change due	*/
/*	to base_flow of estimated water equivalent depth.	*/
/*	Note this assumes a representative hillslope 		*/
/*	porosity profile and thus may contradict reality and	*/
/*	worlds where patches in a hillslope have varying 	*/
/*	porosity.						*/
/*								*/
/*	at present there is no nitrogen leaching with 		*/
/*	TOPMODEL, but to manage a single patch we include	*/
/*	calculation of nitrogen leached for the entire		*/
/*	hillslope						*/
/*	to deal with single patch scenarios			*/
/*								*/
/*--------------------------------------------------------------*/
#include "rhessys.h"

double	top_model(
				  int	verbose_flag,
				  int	grow_flag,
				  int	num_timesteps,
				  double	sen1,
				  double  sen2,
				  double  std_scale,
				  struct	command_line_object *command_line,
				  struct	basin_object *basin,
				  struct	hillslope_object *hillslope,
				  struct 	zone_object **zones,
				  struct	date	current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	double	 compute_z_final(
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

	double	compute_layer_field_capacity(
		int,
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);

	double	compute_unsat_zone_drainage(
		int,
		int,
		double,
		double,
		double,
		double,
		double,
		double);
		
	double  compute_delta_water(
                int,
                double,
                double,
                double,
                double,
                double);	
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int i,j,k;
	double	base_flow, total_baseflow;
	double  mean_sat_deficit, mean_sat_deficit_z, up_flow, down_flow;			/* Taehee Hwang */
	double  new_mean_sat_deficit, new_mean_rz_storage, new_mean_unsat_storage;		/* Taehee Hwang */
	double  preday_mean_sat_deficit, preday_mean_unsat_storage, preday_mean_rz_storage; 	/* Taehee Hwang */
	double  mean_hillslope_lna;
	double  new_total_litter_store, new_total_detention_store;
	double  total_detention_store, total_litter_store;
	double  preday_total_detention_store;
	double  preday_total_litter_store;
	double  effective_sat_deficit;
	double	Q_0;
	double	rz_drainage, unsat_drainage;							/* Taehee Hwang */
	double area, scale;
	double  water_balance, total_new_return_flow;
	double	return_flow;
	double  mean_N_leached, mean_nitrate;
	double	preday_sat_deficit_z, add_field_capacity;
	struct	patch_object *patch;
	

	/*--------------------------------------------------------------*/
	/* allow for iteration at sub-daily time steps			*/
	/*--------------------------------------------------------------*/
	total_baseflow = 0.0;
	preday_mean_sat_deficit = 0.0;
	preday_mean_unsat_storage = 0.0;
	preday_mean_rz_storage = 0.0;		/* Taehee Hwang */
	preday_total_detention_store = 0.0;
	preday_total_litter_store = 0.0;
	/*--------------------------------------------------------------*/
	/* compute initial conditions					*/
	/*--------------------------------------------------------------*/

	for (	i=0 ; i<hillslope[0].num_zones; i++ ){
		for ( j=0; j< zones[i][0].num_patches; j++ ){
			patch =  zones[i][0].patches[j];
			preday_mean_sat_deficit += zones[i][0].patches[j][0].sat_deficit
				* zones[i][0].patches[j][0].area;
			preday_mean_unsat_storage += zones[i][0].patches[j][0].unsat_storage
				* zones[i][0].patches[j][0].area;
			preday_mean_rz_storage += zones[i][0].patches[j][0].rz_storage
				* zones[i][0].patches[j][0].area;			/* Taehee Hwang */
			preday_total_detention_store += zones[i][0].patches[j][0].detention_store
				* zones[i][0].patches[j][0].area;
			preday_total_litter_store += patch[0].litter.rain_stored
				* zones[i][0].patches[j][0].area;
				
		}
	}

	for ( k=0; k< num_timesteps; ++k) { 

	mean_sat_deficit = 0.0;
	mean_hillslope_lna = 0.0;
	mean_N_leached = 0.0;
	mean_nitrate = 0.0;
	new_total_detention_store = 0.0;
	total_detention_store = 0.0;
	new_total_litter_store = 0.0;
	total_litter_store = 0.0;
	area = 0.0;

	/*--------------------------------------------------------------*/
	/*  Compute Mean Hillslope Saturation Deficit                   */
	/*--------------------------------------------------------------*/
	for (	i=0 ; i<hillslope[0].num_zones; i++ ){
		for ( j=0; j< zones[i][0].num_patches; j++ ){
	/*--------------------------------------------------------------*/
	/*	get rid of any intial return flow from infiltration excess	*/
	/*--------------------------------------------------------------*/
			
			patch =  zones[i][0].patches[j];
			return_flow = 0.0;
			total_detention_store += patch[0].detention_store * patch[0].area;
			total_litter_store += patch[0].litter.rain_stored * patch[0].area;
			
			if ((patch[0].sat_deficit - patch[0].rz_storage - patch[0].unsat_storage) < -1.0*ZERO)  {
				return_flow = compute_varbased_returnflow(
					patch[0].std * std_scale, 
					patch[0].rz_storage + patch[0].unsat_storage,
					patch[0].sat_deficit, &(patch[0].litter));
				patch[0].detention_store += return_flow; 
				patch[0].sat_deficit += (return_flow-(patch[0].unsat_storage+patch[0].rz_storage)) ;
				patch[0].unsat_storage = 0.0;
				patch[0].rz_storage = 0.0;
			}
			
			/*------------------------------------------------------------------*/
			/*  Distribute excess water between transmission and rootzone layer */
			/*------------------------------------------------------------------*/
			/* Only transmission layer saturated (SD changed) */
			else if (patch[0].sat_deficit_z > patch[0].rootzone.depth)  {
				if (patch[0].sat_deficit - patch[0].rootzone.potential_sat - patch[0].unsat_storage < -1.0*ZERO) {
					patch[0].sat_deficit -= patch[0].unsat_storage;
					patch[0].unsat_storage = 0.0;	
				}
			
				if (patch[0].rootzone.potential_sat - patch[0].rz_storage < -1.0*ZERO)  {
					patch[0].unsat_storage += patch[0].rz_storage - patch[0].rootzone.potential_sat;
					patch[0].rz_storage = patch[0].rootzone.potential_sat;	/* fully saturated */
				}
			}
			
			else {
				patch[0].sat_deficit -= patch[0].unsat_storage;
				patch[0].unsat_storage = 0.0;
			}			
			
			if (patch[0].detention_store > patch[0].soil_defaults[0][0].detention_store_size) {
				return_flow = patch[0].detention_store -
						patch[0].soil_defaults[0][0].detention_store_size;
				patch[0].detention_store -= return_flow;
				patch[0].return_flow += return_flow;
			}
			
	/*--------------------------------------------------------------*/
	/*	compute mean soil moisture				*/
	/*--------------------------------------------------------------*/
			mean_sat_deficit += zones[i][0].patches[j][0].sat_deficit
				* zones[i][0].patches[j][0].area;
			if (grow_flag > 0) {
				mean_nitrate += zones[i][0].patches[j][0].soil_ns.nitrate
					* zones[i][0].patches[j][0].area;
			}
			area += zones[i][0].patches[j][0].area;
			mean_hillslope_lna += zones[i][0].patches[j][0].lna
				* zones[i][0].patches[j][0].area;
		}
	}
	mean_sat_deficit = mean_sat_deficit / area;
	mean_hillslope_lna = mean_hillslope_lna / area;
	/*--------------------------------------------------------------*/
	/*      Estimate Qo - discharge when mean sat deficit = 0.      */
	/*      Note that Ambroise suggest that this condition results  */
	/*      in a stored water volume of infinity in the hillslope.  */
	/*      This is only valid for certain porosity with depth      */
	/*      functions (e.g. constant or increasing )                */
	/*	We use a hillslope_area of 1 since we want base_flow in	*/
	/*	m water / day.						*/
	/*--------------------------------------------------------------*/
	Q_0 = hillslope[0].aggdefs.Ksat_0 * exp( -1 * mean_hillslope_lna  ) / num_timesteps;
	/*--------------------------------------------------------------*/
	/*	Compute base flow. (m/day)				*/
	/*								*/
	/*      Use the TOPMODEL theory to estimate the base_flow        */
	/*      Note that there is nothing that stops us applying       */
	/*      the theory if the sat deficit is negative except        */
	/*      that we then buy into the space above the surface of    */
	/*      regions with negative sat deficit having non-infinite   */
	/*      lateral transmissivity.                                 */
	/*								*/
	/*	we compute an effective sat_deficit for  baseflow as	*/
	/*	0.5 * the  min of sat_def				*/
	/*	water equivalent capillary fringe height		*/
	/*	this assumes that some of the water in the cap. fringe  */
	/*	is able to contribute to baseflow			*/
	/*								*/
	/*	we now limit the baseflow to only remove water above	*/
	/*	maximum soil depth for all patches			*/
	/*--------------------------------------------------------------*/
	effective_sat_deficit = mean_sat_deficit - min(0.5 * mean_sat_deficit ,
		0.5 * hillslope[0].aggdefs.psi_air_entry *
		hillslope[0].aggdefs.porosity_0);
	base_flow = Q_0 * exp( -1 * effective_sat_deficit
		/ ( hillslope[0].aggdefs.m));
	total_baseflow += base_flow;
	if (grow_flag > 0) {
		mean_nitrate = mean_nitrate / hillslope[0].area;
		mean_sat_deficit_z = compute_z_final(
				verbose_flag,
				hillslope[0].aggdefs.porosity_0,
				hillslope[0].aggdefs.porosity_decay,
				hillslope[0].aggdefs.soil_depth,
				0.0,
				-1.0 * mean_sat_deficit);
		mean_N_leached = compute_N_leached( verbose_flag,
					mean_nitrate,
					base_flow,
					mean_sat_deficit,
					hillslope[0].aggdefs.soil_water_cap,
					hillslope[0].aggdefs.m,
					hillslope[0].aggdefs.Ksat_0 / num_timesteps,	
					hillslope[0].aggdefs.porosity_0,
					hillslope[0].aggdefs.porosity_decay,
					hillslope[0].aggdefs.N_decay_rate,
					hillslope[0].aggdefs.active_zone_z,
					hillslope[0].aggdefs.soil_depth,
					hillslope[0].aggdefs.mobile_N_proportion,
					NULL);
		hillslope[0].streamflow_N += mean_N_leached;
		mean_nitrate -= hillslope[0].streamflow_N;
	}
	/*--------------------------------------------------------------*/
	/*	Adjust the mean_sat_deficit for the base_flow.		*/
	/*--------------------------------------------------------------*/
	mean_sat_deficit += base_flow;
	/*--------------------------------------------------------------*/
	/*	Estimate new actual sat_deficit of each patch based	*/
	/*	on TOPMODEL and patch m.				*/
	/*	Note that if patch m varies one should use a lna and	*/
	/*	lambda that varies.					*/
	/*--------------------------------------------------------------*/
	new_mean_sat_deficit = 0.0;
	new_mean_unsat_storage = 0.0;
	new_mean_rz_storage = 0.0;			/* Taehee Hwang */
	total_new_return_flow = 0.0;
	for (	i=0 ; i<hillslope[0].num_zones; i++ ){
		for ( j=0; j < zones[i][0].num_patches; j++ ){
			patch =  zones[i][0].patches[j];

			preday_sat_deficit_z = compute_z_final(
				verbose_flag,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].soil_depth,
				0.0,
				-1.0 * patch[0].sat_deficit);
			/*--------------------------------------------------------------*/
			/*	adjust local sat deficit relative to mean		*/
			/*--------------------------------------------------------------*/
			patch[0].sat_deficit = mean_sat_deficit
				- ( patch[0].lna - mean_hillslope_lna )
				* hillslope[0].aggdefs.m ;

			/*--------------------------------------------------------------*/
			/*      Hillslope redistribution  will have changed the		*/
			/*      actual water table depth.                               */
			/*--------------------------------------------------------------*/
			patch[0].sat_deficit_z = compute_z_final(
				verbose_flag,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].soil_depth,
				0.0,
				-1.0 * patch[0].sat_deficit);

			/*--------------------------------------------------------------*/
			/*	compute new field capacity				*/
			/*--------------------------------------------------------------*/
			if (patch[0].sat_deficit_z < patch[0].rootzone.depth)  {
				patch[0].rootzone.field_capacity = compute_layer_field_capacity(
					command_line[0].verbose_flag,
					patch[0].soil_defaults[0][0].theta_psi_curve,
					patch[0].soil_defaults[0][0].psi_air_entry,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].soil_defaults[0][0].p3,
					patch[0].soil_defaults[0][0].p4,
					patch[0].soil_defaults[0][0].porosity_0,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].sat_deficit_z,
					patch[0].sat_deficit_z, 0.0);				
					
				patch[0].field_capacity = 0.0;
			}
			else  {
				patch[0].field_capacity = compute_layer_field_capacity(
					command_line[0].verbose_flag,
					patch[0].soil_defaults[0][0].theta_psi_curve,
					patch[0].soil_defaults[0][0].psi_air_entry,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].soil_defaults[0][0].p3,
					patch[0].soil_defaults[0][0].p4,
					patch[0].soil_defaults[0][0].porosity_0,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].sat_deficit_z,
					patch[0].sat_deficit_z, 0);
					
				patch[0].rootzone.field_capacity = compute_layer_field_capacity(
					command_line[0].verbose_flag,
					patch[0].soil_defaults[0][0].theta_psi_curve,
					patch[0].soil_defaults[0][0].psi_air_entry,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].soil_defaults[0][0].p3,
					patch[0].soil_defaults[0][0].p4,
					patch[0].soil_defaults[0][0].porosity_0,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].sat_deficit_z,
					patch[0].rootzone.depth, 0.0);	
			}

			/*--------------------------------------------------------------*/
			/*      Recompute patch soil moisture storage                   */
			/*--------------------------------------------------------------*/
			if (patch[0].sat_deficit < ZERO)  {
				patch[0].S = 1.0;
				patch[0].rootzone.S = 1.0;
				rz_drainage = 0.0;
				unsat_drainage = 0.0;
			}	
			else if (patch[0].sat_deficit_z > patch[0].rootzone.depth)  {		/* Constant vertical profile of soil porosity */

				/*-------------------------------------------------------*/
				/*	soil drainage and storage update	     	 */
				/*-------------------------------------------------------*/
				patch[0].rootzone.S = min(patch[0].rz_storage / patch[0].rootzone.potential_sat, 1.0);	
				rz_drainage = compute_unsat_zone_drainage(
					command_line[0].verbose_flag,
					patch[0].soil_defaults[0][0].theta_psi_curve,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].rootzone.S,
					patch[0].soil_defaults[0][0].mz_v,
					patch[0].rootzone.depth,
					patch[0].soil_defaults[0][0].Ksat_0 / num_timesteps / 2,	
					patch[0].rz_storage - patch[0].rootzone.field_capacity);
					
				patch[0].rz_storage -=  rz_drainage;
				patch[0].unsat_storage +=  rz_drainage;	
										
				patch[0].S = min(patch[0].unsat_storage / (patch[0].sat_deficit - patch[0].rootzone.potential_sat), 1.0);
				unsat_drainage = compute_unsat_zone_drainage(
					command_line[0].verbose_flag,
					patch[0].soil_defaults[0][0].theta_psi_curve,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].S,
					patch[0].soil_defaults[0][0].mz_v,
					patch[0].sat_deficit_z,
					patch[0].soil_defaults[0][0].Ksat_0 / num_timesteps / 2,
					patch[0].unsat_storage - patch[0].field_capacity);
					
				patch[0].unsat_storage -=  unsat_drainage;
				patch[0].sat_deficit -=  unsat_drainage;
			}
			else  {
				patch[0].sat_deficit -= patch[0].unsat_storage;	/* transfer left water in unsat storage to rootzone layer */
				patch[0].unsat_storage = 0.0;   
								
				patch[0].S = min(patch[0].rz_storage / patch[0].sat_deficit, 1.0);
				rz_drainage = compute_unsat_zone_drainage(
					command_line[0].verbose_flag,
					patch[0].soil_defaults[0][0].theta_psi_curve,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].S,
					patch[0].soil_defaults[0][0].mz_v,
					patch[0].sat_deficit_z,
					patch[0].soil_defaults[0][0].Ksat_0 / num_timesteps / 2,
					patch[0].rz_storage - patch[0].rootzone.field_capacity);	
				
				unsat_drainage = 0.0;	   				  		
				
				patch[0].rz_storage -=  rz_drainage;
				patch[0].sat_deficit -=  rz_drainage;
			}

			patch[0].unsat_drainage += unsat_drainage;
			patch[0].rz_drainage += rz_drainage;
			
			if (grow_flag > 0) {
				patch[0].soil_ns.nitrate = mean_nitrate;
			}
			
			/*--------------------------------------------------------------*/
			/*	compute any saturation excess flow			*/
			/*--------------------------------------------------------------*/
			if ((patch[0].sat_deficit - patch[0].rz_storage - patch[0].unsat_storage) < -1.0*ZERO)  {
				return_flow = compute_varbased_returnflow(
					patch[0].std * std_scale, 
					patch[0].rz_storage + patch[0].unsat_storage,
					patch[0].sat_deficit, &(patch[0].litter));
				patch[0].detention_store += return_flow; 
				patch[0].sat_deficit = 0.0;
				patch[0].unsat_storage = 0.0;
				patch[0].rz_storage = 0.0;
			}
			
			/*------------------------------------------------------------------*/
			/*  Distribute excess water between transmission and rootzone layer */
			/*------------------------------------------------------------------*/
			/* Only transmission layer saturated (SD changed) */
			else if (patch[0].sat_deficit_z > patch[0].rootzone.depth)  {
				if (patch[0].sat_deficit - patch[0].rootzone.potential_sat - patch[0].unsat_storage < -1.0*ZERO) {
					patch[0].sat_deficit -= patch[0].unsat_storage;
					patch[0].unsat_storage = 0.0;	
				}
			
				if (patch[0].rootzone.potential_sat - patch[0].rz_storage < -1.0*ZERO)  {
					patch[0].unsat_storage += patch[0].rz_storage - patch[0].rootzone.potential_sat;
					patch[0].rz_storage = patch[0].rootzone.potential_sat;	/* fully saturated */
				}
			}
			
			else {
				patch[0].sat_deficit -= patch[0].unsat_storage;		/* Make sure that there is no unsat_storage */
				patch[0].unsat_storage = 0.0;
			}			
			
			if (patch[0].detention_store > patch[0].soil_defaults[0][0].detention_store_size) {
				return_flow = patch[0].detention_store -
						patch[0].soil_defaults[0][0].detention_store_size;
				patch[0].detention_store -= return_flow;
				patch[0].return_flow += return_flow;
			}
			
			patch[0].sat_deficit_z = compute_z_final(
				verbose_flag,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].soil_depth,
				0.0,
				-1.0 * patch[0].sat_deficit);

		/*--------------------------------------------------------------*/
		/* 	leave behind field capacity			*/
		/*	if sat deficit has been lowered			*/
		/*	this should be an interactive process, we will use 	*/
		/*	0th order approximation					*/
		/*--------------------------------------------------------------*/
			if ((patch[0].sat_deficit_z > preday_sat_deficit_z) && (patch[0].sat_deficit > ZERO)) {
	       			add_field_capacity = compute_layer_field_capacity(
					command_line[0].verbose_flag,
					patch[0].soil_defaults[0][0].theta_psi_curve,
					patch[0].soil_defaults[0][0].psi_air_entry,
					patch[0].soil_defaults[0][0].pore_size_index,
					patch[0].soil_defaults[0][0].p3,
					patch[0].soil_defaults[0][0].p4,
					patch[0].soil_defaults[0][0].porosity_0,
					patch[0].soil_defaults[0][0].porosity_decay,
					patch[0].sat_deficit_z,
					patch[0].sat_deficit_z,
					preday_sat_deficit_z);

				patch[0].sat_deficit += add_field_capacity;
			
				if ((patch[0].sat_deficit_z > patch[0].rootzone.depth) && (preday_sat_deficit_z > patch[0].rootzone.depth))				
					patch[0].unsat_storage += add_field_capacity;
				
				else if ((patch[0].sat_deficit_z <= patch[0].rootzone.depth) && (preday_sat_deficit_z <= patch[0].rootzone.depth))
					patch[0].rz_storage += add_field_capacity;
				
				else  {
					patch[0].rz_storage += add_field_capacity * (patch[0].rootzone.depth - preday_sat_deficit_z) 
						/ (patch[0].sat_deficit_z - preday_sat_deficit_z);
					patch[0].unsat_storage += add_field_capacity * (patch[0].sat_deficit_z - patch[0].rootzone.depth) 
						/ (patch[0].sat_deficit_z - preday_sat_deficit_z);					
				}
			}

			/* ---------------------------------------------- */
			/*     Final rootzone saturation calculation      */
			/* ---------------------------------------------- */
		 
			if (patch[0].sat_deficit > patch[0].rootzone.potential_sat)
				patch[0].rootzone.S = min(patch[0].rz_storage / (patch[0].rootzone.potential_sat), 1.0);
			else 
				patch[0].rootzone.S = min((patch[0].rz_storage + patch[0].rootzone.potential_sat - patch[0].sat_deficit)
					/ (patch[0].rootzone.potential_sat), 1.0);
			
			total_new_return_flow += patch[0].return_flow * patch[0].area;
			new_mean_sat_deficit += patch[0].sat_deficit * patch[0].area;
			new_mean_unsat_storage += patch[0].unsat_storage * patch[0].area;
			new_mean_rz_storage += patch[0].rz_storage * patch[0].area;		/* Taehee Hwang */
			new_total_detention_store += patch[0].detention_store * patch[0].area;
			new_total_litter_store += patch[0].litter.rain_stored * patch[0].area;

			patch[0].streamflow = patch[0].return_flow;	
			
			patch[0].sat_deficit_z = compute_z_final(
				verbose_flag,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].soil_depth,
				0.0,
				-1.0 * patch[0].sat_deficit);
		} /* end patches */
	} /* end zones */

	} /* end time step iterations */

	/* for now water balance cannot be computed for multiple  */
	if (total_detention_store < ZERO)
		water_balance = ( -new_mean_sat_deficit + preday_mean_sat_deficit + 
			new_mean_unsat_storage + new_mean_rz_storage + new_total_litter_store -
			preday_total_litter_store - preday_mean_unsat_storage - preday_mean_rz_storage + 
			total_new_return_flow - preday_total_detention_store + new_total_detention_store) / hillslope[0].area
		 + total_baseflow; 
	 
	else
		water_balance = 0.0;

	if ((water_balance > 0.0000001) || (water_balance < -0.0000001))  
		printf("\n Hill Water Balance is %12.8f on %d %d %d for Hill %d base %lf return %lf sd %lf %lf un %lf %lf rz %lf %lf litter %lf %lf det %lf %lf\n",
		water_balance,
		current_date.day,
		current_date.month,
		current_date.year,
		hillslope[0].ID,
		total_baseflow,
		total_new_return_flow /hillslope[0].area,
		preday_mean_sat_deficit /hillslope[0].area , new_mean_sat_deficit /hillslope[0].area,
		preday_mean_unsat_storage /hillslope[0].area , new_mean_unsat_storage /hillslope[0].area,
		preday_mean_rz_storage /hillslope[0].area , new_mean_rz_storage /hillslope[0].area,
		preday_total_litter_store /hillslope[0].area , new_total_litter_store /hillslope[0].area,
		preday_total_detention_store /hillslope[0].area , new_total_detention_store / hillslope[0].area
		);

	/*--------------------------------------------------------------*/
	/* now that redistribution is complete update output variables	*/
	/*--------------------------------------------------------------*/
	for (	i=0 ; i<hillslope[0].num_zones; i++ ){
		for ( j=0; j < zones[i][0].num_patches; j++ ){

		patch =  zones[i][0].patches[j];
		if((command_line[0].output_flags.monthly == 1)&&(command_line[0].b != NULL)){
			scale = patch[0].area / basin[0].area;
			basin[0].acc_month.streamflow += (patch[0].return_flow) * scale;
			basin[0].acc_month.et += (patch[0].transpiration_unsat_zone
				+ patch[0].transpiration_sat_zone + patch[0].evaporation)*scale;
			basin[0].acc_month.denitrif += patch[0].ndf.denitrif*scale;
			basin[0].acc_month.nitrif += patch[0].ndf.sminn_to_nitrate*scale;
			basin[0].acc_month.mineralized += patch[0].ndf.net_mineralized*scale;
			basin[0].acc_month.uptake += patch[0].ndf.sminn_to_npool*scale;
			basin[0].acc_month.DOC_loss += patch[0].cdf.total_DOC_loss * scale;
			basin[0].acc_month.DON_loss+= patch[0].ndf.total_DON_loss * scale;
			basin[0].acc_month.psn += patch[0].net_plant_psn * scale;
			basin[0].acc_month.lai += patch[0].lai * scale;
			basin[0].acc_month.length += (1*scale);
		}
		if((command_line[0].output_flags.yearly == 1)&&(command_line[0].b != NULL)){
			scale = patch[0].area / basin[0].area;
			basin[0].acc_year.length += (1*scale);
			basin[0].acc_year.denitrif += patch[0].ndf.denitrif * scale;
			basin[0].acc_year.nitrif += patch[0].ndf.sminn_to_nitrate*scale;
			basin[0].acc_year.mineralized += patch[0].ndf.net_mineralized*scale;
			basin[0].acc_year.uptake += patch[0].ndf.sminn_to_npool*scale;
			basin[0].acc_year.DOC_loss += patch[0].cdf.total_DOC_loss * scale;
			basin[0].acc_year.DON_loss += patch[0].ndf.total_DON_loss * scale;
			basin[0].acc_year.psn += patch[0].net_plant_psn * scale;
			basin[0].acc_year.et += (patch[0].evaporation + patch[0].transpiration_unsat_zone + patch[0].transpiration_sat_zone)
						* scale;
			basin[0].acc_year.streamflow += (patch[0].streamflow)*scale;
			basin[0].acc_year.lai += patch[0].lai * scale;
			}

	
		if((command_line[0].output_flags.monthly == 1)&&(command_line[0].p != NULL)){
			patch[0].acc_month.sm_deficit += (patch[0].sat_deficit - patch[0].unsat_storage);

			if (patch[0].sat_deficit > ZERO)
				patch[0].acc_month.theta +=  patch[0].unsat_storage / patch[0].sat_deficit;
			else
				patch[0].acc_month.theta = 1.0;
			patch[0].acc_month.et += (patch[0].transpiration_unsat_zone
				+ patch[0].transpiration_sat_zone + patch[0].evaporation);
			patch[0].acc_month.denitrif += patch[0].ndf.denitrif;
			patch[0].acc_month.nitrif += patch[0].ndf.sminn_to_nitrate;
			patch[0].acc_month.mineralized += patch[0].ndf.net_mineralized;
			patch[0].acc_month.uptake += patch[0].ndf.sminn_to_npool;
			patch[0].acc_month.DOC_loss += patch[0].cdf.total_DOC_loss;
			patch[0].acc_month.DON_loss += patch[0].ndf.total_DON_loss;
			patch[0].acc_month.psn += patch[0].net_plant_psn;
			patch[0].acc_month.snowpack =  max( patch[0].snowpack.water_equivalent_depth, patch[0].acc_month.snowpack);
			patch[0].acc_month.lai = max(patch[0].acc_month.lai, patch[0].lai);
			patch[0].acc_month.length += 1;
		}
		if((command_line[0].output_flags.yearly == 1)&&(command_line[0].p != NULL)){
			patch[0].acc_year.length += 1;
			if (patch[0].sat_deficit > ZERO)
				patch[0].acc_year.theta +=  patch[0].unsat_storage / patch[0].sat_deficit;
			else
				patch[0].acc_year.theta = 1.0;
			if ( (patch[0].sat_deficit - patch[0].unsat_storage) >
				command_line[0].thresholds[SATDEF] )
				patch[0].acc_year.num_threshold += 1;
			patch[0].acc_year.denitrif += patch[0].ndf.denitrif;
			patch[0].acc_year.nitrif += patch[0].ndf.sminn_to_nitrate;
			patch[0].acc_year.mineralized += patch[0].ndf.net_mineralized;
			patch[0].acc_year.uptake += patch[0].ndf.sminn_to_npool;
			patch[0].acc_year.DOC_loss += patch[0].cdf.total_DOC_loss;
			patch[0].acc_year.DON_loss += patch[0].ndf.total_DON_loss;
			patch[0].acc_year.burn += patch[0].burn;
			patch[0].acc_year.psn += patch[0].net_plant_psn;
			patch[0].acc_year.snowpack =  max( patch[0].snowpack.water_equivalent_depth, patch[0].acc_year.snowpack);
			patch[0].acc_year.et += (patch[0].transpiration_unsat_zone
				+ patch[0].transpiration_sat_zone + patch[0].evaporation);
			patch[0].acc_year.lai = max(patch[0].acc_year.lai, patch[0].lai);
			
		}
	   }
	}

	basin[0].acc_month.stream_NO3 += (hillslope[0].streamflow_N * hillslope[0].area / basin[0].area);
	basin[0].acc_year.stream_NO3 += (hillslope[0].streamflow_N * hillslope[0].area / basin[0].area);
	basin[0].acc_month.streamflow += (total_baseflow * hillslope[0].area / basin[0].area);
	basin[0].acc_year.streamflow += (total_baseflow * hillslope[0].area / basin[0].area);

	return(total_baseflow);
} /*end top_model.c*/

