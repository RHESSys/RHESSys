/*--------------------------------------------------------------*/
/* 																*/
/*					construct_patch								*/
/*																*/
/*	construct_patch.c - creates a patch object					*/
/*																*/
/*	NAME														*/
/*	construct_patch.c - creates a patch object					*/
/*																*/
/*	SYNOPSIS													*/
/*	struct patch_object  construct_patch( 						*/
/*					FILE	*world_file,						*/
/*					struct	command_line_object	*command_line,	*/
/*					struct	default_object)						*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Allocates memory and reads in parameters from the hillslope	*/
/*	file to create a patch object.  Invokes construction		*/
/*	of canopy_stratum objects.									*/
/*																*/
/*	Refer to cnostruct_basin.c for a specification of the		*/
/*	hillslopes file.											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We assume that the FILE pointers to the 					*/
/*	hillslope file are positioned properly.						*/
/*	 															*/
/*	We assume that the basin and hillslope files have correct	*/
/*	syntax.														*/
/*																*/
/*	Original code, January 16, 1996.							*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "phys_constants.h"

struct patch_object *construct_patch(
									 struct	command_line_object	*command_line,
									 FILE	*world_file,
									 int     num_world_base_stations,
									 struct  base_station_object **world_base_stations,
									 struct	default_object	*defaults)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
		int ,
		int ,
		struct base_station_object **);
	struct 	canopy_strata_object *construct_canopy_strata( 
		struct command_line_object *,
		FILE	*,
		struct	patch_object *,
		int     num_world_base_stations,
		struct  base_station_object **world_base_stations,
		struct	default_object	*defaults);
	struct 	canopy_strata_object *construct_empty_shadow_strata( 
		struct command_line_object *,
		struct	patch_object *,
		struct  canopy_strata_object *stratum,
		struct	default_object	*defaults);
	double	compute_z_final( 	int,
		double,
		double,
		double,
		double,
		double);
	void	update_litter_interception_capacity (
		double, 
		double, 
		struct litter_c_object *,
		struct litter_object *);
	
	void	sort_patch_layers(struct patch_object *);
	void	*alloc(	size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definitions				*/
	/*--------------------------------------------------------------*/
	int		base_stationID;
	int		i;
	int		soil_default_object_ID;
	int		landuse_default_object_ID;
	int		fire_default_object_ID;
	int		surface_energy_default_object_ID;
	char		record[MAXSTR];
	struct patch_object *patch;
	double	mpar;
	
	/*--------------------------------------------------------------*/
	/*  Allocate a patch object.                                */
	/*--------------------------------------------------------------*/
	patch = (struct patch_object *) alloc( 1 *
		sizeof( struct patch_object ),"patch","construct_patch");

  /*---------------------------------------------------------------------------------*/
  /*  Allocate a shadow_litter object, and shadow_soil object if spinup flag is set  */
  /*---------------------------------------------------------------------------------*/
 	if ( (command_line[0].vegspinup_flag > 0) ) {
   patch[0].shadow_litter_cs = (struct litter_c_object *) alloc( 1 *
      sizeof( struct litter_c_object ),"shadow_litter_cs", "construct_patch" );
        
   patch[0].shadow_litter_ns = (struct litter_n_object *) alloc( 1 *
      sizeof( struct litter_n_object ),"shadow_litter_ns", "construct_patch" );
    
   patch[0].shadow_soil_cs = (struct soil_c_object *) alloc( 1 *
      sizeof( struct soil_c_object ),"shadow_soil_cs", "construct_patch" );
        
   patch[0].shadow_soil_ns = (struct soil_n_object *) alloc( 1 *
      sizeof( struct soil_n_object ),"shadow_soil_ns", "construct_patch" );
  }
	
	/*--------------------------------------------------------------*/
	/*	Read in the next patch record for this hillslope.			*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(patch[0].ID));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].x));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].y));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].z));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(soil_default_object_ID));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(landuse_default_object_ID));
	read_record(world_file, record);

	if (command_line[0].firespread_flag == 1) {
		fscanf(world_file,"%d",&(fire_default_object_ID));
		read_record(world_file, record);
		}

	if (command_line[0].surface_energy_flag == 1) {
		fscanf(world_file,"%d",&(surface_energy_default_object_ID));
		read_record(world_file, record);
		}

	fscanf(world_file,"%lf",&(patch[0].area));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].slope));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].lna));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].Ksat_vertical));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(mpar));
	read_record(world_file, record);
	if (command_line[0].stdev_flag == 1) {
		fscanf(world_file,"%lf",&(patch[0].std));
		read_record(world_file, record);
		patch[0].std = patch[0].std*command_line[0].std_scale;
		}
	else patch[0].std = 0.0;
	fscanf(world_file,"%lf",&(patch[0].rz_storage));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].unsat_storage));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].sat_deficit));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].snowpack.water_equivalent_depth));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].snowpack.water_depth));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].snowpack.T));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].snowpack.surface_age));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].snowpack.energy_deficit));
	read_record(world_file, record);
	if (command_line[0].snow_scale_flag == 1) {
		fscanf(world_file,"%lf",&(patch[0].snow_redist_scale));
		read_record(world_file, record);
		}



	patch[0].slope = patch[0].slope * DtoR;
	patch[0].surface_Tday = -999.9;
	patch[0].surface_Tnight = -999.9;
	patch[0].preday_sat_deficit  = patch[0].sat_deficit;
	patch[0].deltaS = 0.0;
	patch[0].streamflow = 0.0;
	patch[0].return_flow = 0.0;
	patch[0].gw_drainage = 0.0;
	patch[0].gw_drainage_hourly = 0.0;
	patch[0].infiltration_excess = 0.0;
	patch[0].streamflow_NH4 = 0.0;
	patch[0].streamflow_NO3 = 0.0;
	patch[0].snowpack.height = patch[0].snowpack.water_equivalent_depth *10.0;
	patch[0].tmp = 0.0;
	patch[0].detention_store = 0.0;	
	patch[0].soil_ns.DON = 0.0;
	patch[0].soil_cs.DOC = 0.0;

	/*--------------------------------------------------------------*/
	/*      initialize accumulator variables for this patch         */
	/*--------------------------------------------------------------*/

	patch[0].acc_month.et = 0.0;
	patch[0].acc_month.snowpack = 0.0;
	patch[0].acc_month.theta = 0.0;
	patch[0].acc_month.streamflow = 0.0;
	patch[0].acc_month.length = 0;
	patch[0].acc_month.denitrif = 0.0;
	patch[0].acc_month.nitrif = 0.0;
	patch[0].acc_month.mineralized = 0.0;
	patch[0].acc_month.uptake = 0.0;
	patch[0].acc_month.lai = 0.0;
	patch[0].acc_month.leach = 0.0;
	patch[0].acc_month.DOC_loss = 0.0;
	patch[0].acc_month.DON_loss = 0.0;
	patch[0].acc_month.stream_NO3 = 0.0;
	patch[0].acc_month.stream_NH4 = 0.0;
	patch[0].acc_month.PET = 0.0;
	patch[0].acc_month.psn = 0.0;
	patch[0].acc_month.num_threshold = 0;


	patch[0].acc_year.burn = 0.0;
	patch[0].acc_year.pcp = 0.0;
	patch[0].acc_year.snowin = 0.0;
	patch[0].acc_year_trans = 0.0;
	patch[0].acc_year.trans = 0.0;
	patch[0].acc_year.maxtrans = 0.0;
	patch[0].acc_year.day7trans = 0.0;
	patch[0].acc_year.et = 0.0;
	patch[0].acc_year.snowpack = 0.0;
	patch[0].acc_year.theta = 0.0;
	patch[0].acc_year.streamflow = 0.0;
	patch[0].acc_year.length = 0;
	patch[0].acc_year.denitrif = 0.0;
	patch[0].acc_year.nitrif = 0.0;
	patch[0].acc_year.mineralized = 0.0;
	patch[0].acc_year.uptake = 0.0;
	patch[0].acc_year.lai = 0.0;
	patch[0].acc_year.leach = 0.0;
	patch[0].acc_year.DOC_loss = 0.0;
	patch[0].acc_year.DON_loss = 0.0;
	patch[0].acc_year.stream_NO3 = 0.0;
	patch[0].acc_year.stream_NH4 = 0.0;
	patch[0].acc_year.PET = 0.0;
	patch[0].acc_year.psn = 0.0;
	patch[0].acc_year.num_threshold = 0;
	patch[0].acc_year.meltday = 0;
	patch[0].acc_year.wyd = 0;
	patch[0].acc_year.rec_wyd = 0;
	patch[0].acc_year.peaksweday = 0;
	patch[0].acc_year.potential_recharge = 0.0;
	patch[0].acc_year.potential_recharge_wyd = 0.0;
	patch[0].acc_year.recharge = 0.0;
	patch[0].acc_year.recharge_wyd = 0.0;

	if (command_line[0].firespread_flag == 1) {
		patch[0].fire.et = 0.0;
		patch[0].fire.pet = 0.0;
		}	
	/*--------------------------------------------------------------*/
	/*	Variables for the dynamic version are included here     */
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%lf",&(patch[0].litter.cover_fraction));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].litter.rain_stored));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].litter_cs.litr1c));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].litter_ns.litr1n));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].litter_cs.litr2c));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].litter_cs.litr3c));
	read_record(world_file, record); 
	fscanf(world_file,"%lf",&(patch[0].litter_cs.litr4c));
	read_record(world_file, record);
	
	patch[0].litter_ns.litr2n = patch[0].litter_cs.litr2c / CEL_CN;
	patch[0].litter_ns.litr3n = patch[0].litter_cs.litr3c / CEL_CN;
	patch[0].litter_ns.litr4n = patch[0].litter_cs.litr4c / LIG_CN;
	
	fscanf(world_file,"%lf",&(patch[0].soil_cs.soil1c));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].soil_ns.sminn));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].soil_ns.nitrate));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].soil_cs.soil2c));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].soil_cs.soil3c));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(patch[0].soil_cs.soil4c));
	read_record(world_file, record);


	patch[0].soil_ns.soil1n = patch[0].soil_cs.soil1c / SOIL1_CN;
	patch[0].soil_ns.soil2n = patch[0].soil_cs.soil2c / SOIL2_CN;
	patch[0].soil_ns.soil3n = patch[0].soil_cs.soil3c / SOIL3_CN;
	patch[0].soil_ns.soil4n = patch[0].soil_cs.soil4c / SOIL4_CN;
	
	/*--------------------------------------------------------------*/
	/*	initialize sinks				                                   	*/
	/*--------------------------------------------------------------*/
	
	patch[0].litter_cs.litr1c_hr_snk = 0.0;
	patch[0].litter_cs.litr2c_hr_snk = 0.0;
	patch[0].litter_cs.litr4c_hr_snk = 0.0;
	
	patch[0].soil_cs.soil1c_hr_snk = 0.0;
	patch[0].soil_cs.soil2c_hr_snk = 0.0;
	patch[0].soil_cs.soil4c_hr_snk = 0.0;
	
	patch[0].soil_ns.nfix_src = 0.0;
	patch[0].soil_ns.ndep_src = 0.0;
	patch[0].soil_ns.nleached_snk = 0.0;
	patch[0].soil_ns.nvolatilized_snk = 0.0;

	patch[0].litter.NO3_stored = 0.0;
	patch[0].surface_NO3 = 0.0;
	patch[0].surface_NH4 = 0.0;
	patch[0].surface_DOC = 0.0;
	patch[0].surface_DON = 0.0;
	patch[0].fertilizer_NO3 = 0.0;
	patch[0].fertilizer_NH4 = 0.0;
	patch[0].grazing_Closs = 0.0;

  /*--------------------------------------------------------------*/
  /*   Initialize shadow litter and soil objects for this  patch. */
  /*--------------------------------------------------------------*/
 	if( (command_line[0].vegspinup_flag > 0) ) {
    patch[0].shadow_litter_cs[0].litr1c = patch[0].litter_cs.litr1c;    
    patch[0].shadow_litter_cs[0].litr2c = patch[0].litter_cs.litr2c;    
    patch[0].shadow_litter_cs[0].litr3c = patch[0].litter_cs.litr3c;    
    patch[0].shadow_litter_cs[0].litr4c = patch[0].litter_cs.litr4c;    
    
    patch[0].shadow_litter_ns[0].litr1n = patch[0].litter_ns.litr1n; 
    patch[0].shadow_litter_ns[0].litr2n = patch[0].litter_ns.litr2n; 
    patch[0].shadow_litter_ns[0].litr3n = patch[0].litter_ns.litr3n; 
    patch[0].shadow_litter_ns[0].litr4n = patch[0].litter_ns.litr4n; 

    patch[0].shadow_soil_cs[0].soil1c = patch[0].soil_cs.soil1c;    
    patch[0].shadow_soil_cs[0].soil2c = patch[0].soil_cs.soil2c;    
    patch[0].shadow_soil_cs[0].soil3c = patch[0].soil_cs.soil3c;    
    patch[0].shadow_soil_cs[0].soil4c = patch[0].soil_cs.soil4c;    
    
    patch[0].shadow_soil_ns[0].soil1n = patch[0].soil_ns.soil1n; 
    patch[0].shadow_soil_ns[0].soil2n = patch[0].soil_ns.soil2n; 
    patch[0].shadow_soil_ns[0].soil3n = patch[0].soil_ns.soil3n; 
    patch[0].shadow_soil_ns[0].soil4n = patch[0].soil_ns.soil4n; 
  }


	/*--------------------------------------------------------------*/
	/*	Assign	defaults for this patch								*/
	/*--------------------------------------------------------------*/
	patch[0].soil_defaults = (struct soil_default **)
		alloc( sizeof(struct soil_default *),"defaults",
		"construct_patch" );
	
	i = 0;
	while (defaults[0].soil[i].ID != soil_default_object_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this patch.						    */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_soil_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_patch, soil default ID %d not found for patch %d\n" ,
				soil_default_object_ID, patch[0].ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	patch[0].soil_defaults[0] = &defaults[0].soil[i];

	patch[0].landuse_defaults = (struct landuse_default **)
		alloc( sizeof(struct landuse_default *),"defaults",
		"construct_patch" );
	i = 0;
	while (defaults[0].landuse[i].ID != landuse_default_object_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this patch.						    */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_landuse_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_patch, landuse default ID %d not found for patch %d\n" ,
				landuse_default_object_ID, patch[0].ID);
                        // fprintf(stderr, "\n %d ", defaults[0].landuse[i-1].ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	patch[0].landuse_defaults[0] = &defaults[0].landuse[i];


	
	/*--------------------------------------------------------------*/
	/* if fire spread module is called assign fire defaults		*/

	/*--------------------------------------------------------------*/
	if (command_line[0].firespread_flag == 1) {
	patch[0].fire_defaults = (struct fire_default **)
		alloc( sizeof(struct fire_default *),"defaults",
		"construct_patch" );
	i = 0;
	while (defaults[0].fire[i].ID != fire_default_object_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this patch.						    */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_fire_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_patch, fire default ID %d not found for patch %d\n" ,
				fire_default_object_ID, patch[0].ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	patch[0].fire_defaults[0] = &defaults[0].fire[i];
	}

	/*--------------------------------------------------------------*/
	/* if surface energy module is called assign fire defaults	*/

	/*--------------------------------------------------------------*/
	if (command_line[0].surface_energy_flag == 1) {

		patch[0].surface_energy_profile = (struct surface_energy_object *)
		alloc(4* sizeof(struct surface_energy_object),"energy_object",
		"construct_patch");

		patch[0].surface_energy_defaults = (struct surface_energy_default **)
		alloc( sizeof(struct surface_energy_default *),"defaults",
		"construct_patch" );
		i = 0;
	while (defaults[0].surface_energy[i].ID != surface_energy_default_object_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this patch.						    */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_surface_energy_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_patch, surface energy default ID %d not found for patch %d\n" ,
				surface_energy_default_object_ID, patch[0].ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	patch[0].surface_energy_defaults[0] = &defaults[0].surface_energy[i];

	patch[0].surface_energy_profile[0].organic = 1.0;
	patch[0].surface_energy_profile[1].organic = 0.2;
	patch[0].surface_energy_profile[2].organic = 0.0;
	patch[0].surface_energy_profile[3].organic = 0.0;

	patch[0].surface_energy_profile[0].quartz = 0.0;
	patch[0].surface_energy_profile[1].quartz = patch[0].soil_defaults[0][0].soil_type.sand;
	patch[0].surface_energy_profile[2].quartz = patch[0].soil_defaults[0][0].soil_type.sand;
	patch[0].surface_energy_profile[3].quartz = patch[0].soil_defaults[0][0].soil_type.sand;

	/*--------------------------------------------------------------*/
	/* there are always 4 layers (corresponding to litter, rooting, unsat/sat and soil depth */
	/* the first later is litter not sure what pore size, psi and porosity should be */
	/*--------------------------------------------------------------*/
	patch[0].surface_energy_profile[0].porosity = 0.8;
	patch[0].surface_energy_profile[0].psi_air_entry = 0.20;
	patch[0].surface_energy_profile[0].pore_size_index = 0.2;

	patch[0].surface_energy_profile[1].psi_air_entry = patch[0].soil_defaults[0][0].psi_air_entry;
	patch[0].surface_energy_profile[2].psi_air_entry = patch[0].soil_defaults[0][0].psi_air_entry;
	patch[0].surface_energy_profile[3].psi_air_entry = patch[0].soil_defaults[0][0].psi_air_entry;

	
	patch[0].surface_energy_profile[1].pore_size_index = patch[0].soil_defaults[0][0].pore_size_index;
	patch[0].surface_energy_profile[2].pore_size_index = patch[0].soil_defaults[0][0].pore_size_index;
	patch[0].surface_energy_profile[3].pore_size_index = patch[0].soil_defaults[0][0].pore_size_index;

	patch[0].surface_energy_profile[3].depth = patch[0].soil_defaults[0][0].soil_depth;
	patch[0].litter.T = -999.0;
	patch[0].rootzone.T = -999.0;
		
	}


	/*--------------------------------------------------------------*/
	/* FOR now substitute worldfile m (if > 0) in defaults			*/
	/*--------------------------------------------------------------*/
	patch[0].original_m = mpar;
	if (mpar > ZERO) {
		patch[0].m = mpar * command_line[0].sen[M];
		patch[0].m_z = patch[0].soil_defaults[0][0].porosity_0 * mpar;
	}
	else {
		patch[0].m = patch[0].soil_defaults[0][0].m;
		patch[0].m_z = patch[0].soil_defaults[0][0].porosity_0 * patch[0].m;
		}

	/*--------------------------------------------------------------*/
	/*	if landuse default files include a percent impervious	*/
	/*	use this to over-ride Ksat vertical			*/
	/*--------------------------------------------------------------*/
	if (patch[0].landuse_defaults[0][0].percent_impervious > ZERO)
		patch[0].Ksat_vertical = 1.0-patch[0].landuse_defaults[0][0].percent_impervious;

	/*--------------------------------------------------------------*/
 	/* initialize PH to land use default value			*/
	/*--------------------------------------------------------------*/
	patch[0].PH = patch[0].landuse_defaults[0][0].PH;

	/*--------------------------------------------------------------*/
	/* compute a biological soil depth based on the minimum of soil depth */
	/* and m, K parameters defining conductivity < 0.1% original value */
	/* turn this off for now */
	/*--------------------------------------------------------------*/
	patch[0].soil_defaults[0][0].effective_soil_depth = patch[0].soil_defaults[0][0].soil_depth;
	/*
	patch[0].soil_defaults[0][0].effective_soil_depth = min(patch[0].soil_defaults[0][0].soil_depth,
				6.9*patch[0].soil_defaults[0][0].m_z);	
	*/

	/*--------------------------------------------------------------*/
	/* detention store size can vary with both soil and landuse		*/
	/*	use the maximum of the two									*/
	/*--------------------------------------------------------------*/
	patch[0].soil_defaults[0][0].detention_store_size = 
				max(patch[0].landuse_defaults[0][0].detention_store_size,
				patch[0].soil_defaults[0][0].detention_store_size);
	/*--------------------------------------------------------------*/
	/*	Read in the number of  patch base stations 					*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(patch[0].num_base_stations));
	read_record(world_file, record);
	/*--------------------------------------------------------------*/
	/*    Allocate a list of base stations for this patch.			*/
	/*--------------------------------------------------------------*/
	patch[0].base_stations = (struct base_station_object **)
		alloc(patch[0].num_base_stations *
		sizeof(struct base_station_object *),
		"base_stations","construct_patch" );
	/*--------------------------------------------------------------*/
	/*      Read each base_station ID and then point to that base_statio*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<patch[0].num_base_stations; i++){
		fscanf(world_file,"%d",&(base_stationID));
		read_record(world_file, record);
		/*--------------------------------------------------------------*/
		/*	Point to the appropriate base station in the base       	*/
		/*              station list for this world.					*/
		/*--------------------------------------------------------------*/
		patch[0].base_stations[i] = assign_base_station(
			base_stationID,
			num_world_base_stations,
			world_base_stations);
	} /*end for*/
	/*--------------------------------------------------------------*/
	/*	Read in number of canopy strata objects in this patch		*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(patch[0].num_canopy_strata));
	read_record(world_file, record);
	
	/*--------------------------------------------------------------*/
	/*	Allocate list of pointers to stratum objects .				*/
	/*--------------------------------------------------------------*/
	patch[0].canopy_strata = ( struct canopy_strata_object ** )
		alloc( patch[0].num_canopy_strata *
		sizeof( struct canopy_strata_object *),
		"canopy_strata","construct_patch");
 	
		patch[0].shadow_strata = ( struct canopy_strata_object ** )
			alloc( patch[0].num_canopy_strata * 
			sizeof( struct canopy_strata_object *),
			"shadow_strata","construct_patch");

	/*--------------------------------------------------------------*/
	/*      Allocate the patch hourly object.	  */
	/*--------------------------------------------------------------*/
	if ((patch[0].hourly = (struct patch_hourly_object *) calloc(1,
		sizeof(struct patch_hourly_object))) == NULL ){
		fprintf(stderr,"FATAL ERROR: in patch_hourly\n");
		exit(EXIT_FAILURE);
	}
	
	/*--------------------------------------------------------------*/
	/*      Initialize patch level rainand snow stored              */
	/*--------------------------------------------------------------*/
	patch[0].rain_stored = 0.0;
	patch[0].snow_stored = 0.0;
	patch[0].daily_fire_litter_turnover = 0.0;
	patch[0].psi_max_veg = 0.0;
	patch[0].litter.gl_c = 0.0;
	patch[0].litter.gsurf_slope = 0.0;
	patch[0].litter.moist_coef = 0.0;
	patch[0].litter.density = 0.0;	
	patch[0].litter.gsurf_intercept = 0.0;
	patch[0].rootzone.depth =  0.0;
	/*--------------------------------------------------------------*/
	/*	Construct the strata in this patch.						*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<patch[0].num_canopy_strata ; i++ ){
		patch[0].canopy_strata[i] = construct_canopy_strata(  
			command_line,
			world_file,
			patch,
			num_world_base_stations,
			world_base_stations,defaults);
		/*--------------------------------------------------------------*/
		/*      Aggregate rain and snow stored already for water balance*/
		/*--------------------------------------------------------------*/
		patch[0].rain_stored += patch[0].canopy_strata[i][0].rain_stored
			* patch[0].canopy_strata[i][0].cover_fraction;
		patch[0].snow_stored += patch[0].canopy_strata[i][0].snow_stored
			* patch[0].canopy_strata[i][0].cover_fraction;
		patch[0].daily_fire_litter_turnover +=
			patch[0].canopy_strata[i][0].defaults[0][0].epc.daily_fire_turnover
				* patch[0].canopy_strata[i][0].cover_fraction;		
		patch[0].psi_max_veg =
			min(patch[0].canopy_strata[i][0].defaults[0][0].epc.psi_close,
				patch[0].psi_max_veg);	
		patch[0].litter.gl_c +=
			patch[0].canopy_strata[i][0].defaults[0][0].epc.gl_c
			* patch[0].canopy_strata[i][0].cover_fraction;
		patch[0].litter.gsurf_slope +=
			patch[0].canopy_strata[i][0].defaults[0][0].epc.litter_gsurf_slope
			* patch[0].canopy_strata[i][0].cover_fraction;
		patch[0].litter.gsurf_intercept +=
			patch[0].canopy_strata[i][0].defaults[0][0].epc.litter_gsurf_intercept
			* patch[0].canopy_strata[i][0].cover_fraction;
		patch[0].litter.moist_coef +=
			patch[0].canopy_strata[i][0].defaults[0][0].epc.litter_moist_coef
			* patch[0].canopy_strata[i][0].cover_fraction;
		patch[0].litter.density +=
			patch[0].canopy_strata[i][0].defaults[0][0].epc.litter_density
			* patch[0].canopy_strata[i][0].cover_fraction;
		patch[0].rootzone.depth = max(patch[0].rootzone.depth, 
			 patch[0].canopy_strata[i][0].rootzone.depth);
	} /*end for*/

	patch[0].wilting_point = exp(-1.0*log(-1.0*100.0*patch[0].psi_max_veg/
						patch[0].soil_defaults[0][0].psi_air_entry) 
			* patch[0].soil_defaults[0][0].pore_size_index) * patch[0].soil_defaults[0][0].porosity_0;

	patch[0].precip_with_assim = 0.0;
	
  /*--------------------------------------------------------------*/
	/*	Construct the shadow strata in this patch.		*/
	/*--------------------------------------------------------------*/
	if ( (command_line[0].vegspinup_flag > 0) ) {
	
	for ( i=0 ; i<patch[0].num_canopy_strata ; i++ ){
		patch[0].shadow_strata[i] = construct_empty_shadow_strata(
			command_line,
			patch,
			patch[0].canopy_strata[i],
      defaults);     
 
		patch[0].shadow_strata[i][0].ID = patch[0].canopy_strata[i][0].ID;
		patch[0].shadow_strata[i][0].defaults = patch[0].canopy_strata[i][0].defaults;
		patch[0].shadow_strata[i][0].base_stations = patch[0].canopy_strata[i][0].base_stations;
		patch[0].shadow_strata[i][0].num_base_stations = patch[0].canopy_strata[i][0].num_base_stations;
	
        } /*end for*/
	} /*end shadow stratum if statement*/

	/*--------------------------------------------------------------*/
	/*	initialize litter capacity				*/
	/* 	set litter temperature to -999 to trigger update	*/
	/*--------------------------------------------------------------*/
	update_litter_interception_capacity(
		patch[0].litter.moist_coef,
		patch[0].litter.density,
		&(patch[0].litter_cs),
		&(patch[0].litter));

	/*--------------------------------------------------------------*/
	/*	Define a list of canopy strata layers that can at least	*/
	/*	fit all of the canopy strata.				*/
	/*--------------------------------------------------------------*/
	patch[0].layers = (struct layer_object *) alloc( patch[0].num_canopy_strata *
		sizeof( struct layer_object ),"layers","construct_patch");
	patch[0].num_layers = 0;
	sort_patch_layers(patch);

		

	/*--------------------------------------------------------------*/
	/*	compute actual depth to water tablke			*/
	/*--------------------------------------------------------------*/
	patch[0].unsat_zone_volume = patch[0].sat_deficit + patch[0].unsat_storage;
	patch[0].sat_deficit_z = compute_z_final(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		0,
		-1*patch[0].sat_deficit);
	patch[0].preday_sat_deficit_z = patch[0].sat_deficit_z;
	


	return(patch);
} /*end construct_patch.c*/

