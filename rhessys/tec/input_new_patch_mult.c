/*--------------------------------------------------------------*/
/* 																*/
/*					input_new_patch								*/
/*																*/
/*	input_new_patch.c - creates a patch object					*/
/*																*/
/*	NAME														*/
/*	input_new_patch.c - creates a patch object					*/
/*																*/
/*	SYNOPSIS													*/
/*	void  input_new_patch( 						*/
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
#define ONE 1.0


 void  input_new_patch_mult(
									 struct	command_line_object	*command_line,
									 FILE	*world_file,
									 int     num_world_base_stations,
									 struct  base_station_object **world_base_stations,
									 struct	default_object	*defaults,
									 struct	 patch_object *patch)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
		int ,
		int ,
		struct base_station_object **);
	double	compute_z_final( 	int,
		double,
		double,
		double,
		double,
		double);
	void	update_litter_interception_capacity (double, struct litter_c_object *,
		struct litter_object *);
	
	void	*alloc(	size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		base_stationID;
	int		i, dtmp;
	int		soil_default_object_ID;
	int		landuse_default_object_ID;
	char		record[MAXSTR];
	double	mpar, ltmp;

	
	/*--------------------------------------------------------------*/
	/*	Read in the next patch record for this hillslope.			*/
	/*--------------------------------------------------------------*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].x = ltmp * patch[0].x;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].y = ltmp * patch[0].y;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].z = ltmp * patch[0].z;

	fscanf(world_file,"%d",&(soil_default_object_ID));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(landuse_default_object_ID));
	read_record(world_file, record);

 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].area = ltmp * patch[0].area;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].slope = ltmp * patch[0].slope;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].lna = ltmp * patch[0].lna;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].Ksat_vertical = ltmp * patch[0].Ksat_vertical;
 	fscanf(world_file,"%lf",&(mpar));
	read_record(world_file, record);
	if (command_line[0].stdev_flag == 1) {
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].std = ltmp * patch[0].std;
		}
		
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].rz_storage = ltmp * patch[0].rz_storage;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].unsat_storage = ltmp * patch[0].unsat_storage;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].sat_deficit = ltmp * patch[0].sat_deficit;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.water_equivalent_depth = ltmp * patch[0].snowpack.water_equivalent_depth;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.water_depth = ltmp * patch[0].snowpack.water_depth;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.T = ltmp * patch[0].snowpack.T;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.surface_age = ltmp * patch[0].snowpack.surface_age;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.energy_deficit = ltmp * patch[0].snowpack.energy_deficit;

	if (command_line[0].snow_scale_flag == 1) {
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snow_redist_scale = ltmp * patch[0].snow_redist_scale;
		}


 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter.cover_fraction = ltmp * patch[0].litter.cover_fraction;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter.rain_stored = ltmp * patch[0].litter.rain_stored;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter_cs.litr1c = ltmp * patch[0].litter_cs.litr1c;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter_ns.litr1n = ltmp * patch[0].litter_ns.litr1n;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].litter_cs.litr2c = ltmp * patch[0].litter_cs.litr2c;
		patch[0].litter_ns.litr2n = patch[0].litter_cs.litr2c / CEL_CN;
		}
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].litter_cs.litr3c = ltmp * patch[0].litter_cs.litr3c;
		patch[0].litter_ns.litr3n = patch[0].litter_cs.litr3c / CEL_CN;
		}
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].litter_cs.litr4c = ltmp * patch[0].litter_cs.litr4c;
		patch[0].litter_ns.litr4n = patch[0].litter_cs.litr4c / LIG_CN;
		}

 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil1c = ltmp * patch[0].soil_cs.soil1c;
		patch[0].soil_ns.soil1n = patch[0].soil_cs.soil1c / SOIL1_CN;
		}
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].soil_ns.sminn = ltmp * patch[0].soil_ns.sminn;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].soil_ns.nitrate = ltmp * patch[0].soil_ns.nitrate;
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil2c = ltmp * patch[0].soil_cs.soil2c;
		patch[0].soil_ns.soil2n = patch[0].soil_cs.soil2c / SOIL2_CN;
		}
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil3c = ltmp * patch[0].soil_cs.soil3c;
		patch[0].soil_ns.soil3n = patch[0].soil_cs.soil3c / SOIL3_CN;
		}
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil4c = ltmp * patch[0].soil_cs.soil4c;
		patch[0].soil_ns.soil4n = patch[0].soil_cs.soil4c / SOIL4_CN;
		}
	
	
	/*--------------------------------------------------------------*/
	/*	initialize litter capacity				*/
	/*--------------------------------------------------------------*/
	
	update_litter_interception_capacity(
		patch[0].litter.moist_coef,
		&(patch[0].litter_cs),
		&(patch[0].litter));
	/*--------------------------------------------------------------*/
	/*	initialize sinks					*/
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

	
	/*--------------------------------------------------------------*/
	/*	Assign	defaults for this patch								*/
	/*--------------------------------------------------------------*/
	if (soil_default_object_ID > 0) {
		i = 0;
		while (defaults[0].soil[i].ID != soil_default_object_ID) {
			i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this patch.						    */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_soil_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in input_new_patch, soil default ID %d not found for patch %d\n" ,
				soil_default_object_ID, patch[0].ID);
			exit(EXIT_FAILURE);
			}
		} /* end-while */
	
	patch[0].soil_defaults[0] = &defaults[0].soil[i];
	}

	if (landuse_default_object_ID > 0) {
		i = 0;
		while (defaults[0].landuse[i].ID != landuse_default_object_ID) {
			i++;
			/*--------------------------------------------------------------*/
			/*  Report an error if no match was found.  Otherwise assign    */
			/*  the default to point to this patch.						    */
			/*--------------------------------------------------------------*/
			if ( i>= defaults[0].num_landuse_default_files ){
				fprintf(stderr,
					"\nFATAL ERROR: in input_new_patch, landuse default ID %d not found for patch %d\n" ,
					landuse_default_object_ID, patch[0].ID);
				exit(EXIT_FAILURE);
			}
		} /* end-while */
		patch[0].landuse_defaults[0] = &defaults[0].landuse[i];
	}
	/*--------------------------------------------------------------*/
	/* FOR now substitute worldfile m (if > 0) in defaults			*/
	/*--------------------------------------------------------------*/
	if (mpar > ZERO) {
		patch[0].original_m = mpar;
		patch[0].soil_defaults[0][0].m = mpar * command_line[0].sen[M];
		patch[0].soil_defaults[0][0].m_z = mpar * command_line[0].sen[M] / 
				patch[0].soil_defaults[0][0].porosity_0;
	}


	/*--------------------------------------------------------------*/
	/* compute a biological soil depth based on the minimum of soil depth */
	/* and m, K parameters defining conductivity < 0.1% original value */
	/*--------------------------------------------------------------*/
	patch[0].soil_defaults[0][0].effective_soil_depth = patch[0].soil_defaults[0][0].soil_depth;
	
	/*  patch[0].soil_defaults[0][0].effective_soil_depth = min(patch[0].soil_defaults[0][0].soil_depth,
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
 	fscanf(world_file,"%d",&(dtmp));
	read_record(world_file, record);
	if (dtmp > 0)  {
		patch[0].num_base_stations = dtmp * patch[0].num_base_stations;
		/*--------------------------------------------------------------*/
		/*    Allocate a list of base stations for this patch.			*/
		/*--------------------------------------------------------------*/
		patch[0].base_stations = (struct base_station_object **)
			alloc(patch[0].num_base_stations *
			sizeof(struct base_station_object *),
			"base_stations","input_new_patch" );
		/*--------------------------------------------------------------*/
		/*      Read each base_station ID and then point to that base_statio*/
		/*--------------------------------------------------------------*/
		for (i=0 ; i<patch[0].num_base_stations; i++){
			fscanf(world_file,"%d",&(base_stationID));
			read_record(world_file, record);
			/*--------------------------------------------------------------*/
			/*	Point to the appropriate base station in the base       	*/
			/*              station list for this world.					*/
			/*																*/
			/*--------------------------------------------------------------*/
			patch[0].base_stations[i] = assign_base_station(
				base_stationID,
				num_world_base_stations,
				world_base_stations);
		} /*end for*/
	}
	
	/*--------------------------------------------------------------*/
	/*	compute actual depth to water tablke			*/
	/*--------------------------------------------------------------*/
	patch[0].sat_deficit_z = compute_z_final(
		command_line[0].verbose_flag,
		patch[0].soil_defaults[0][0].porosity_0,
		patch[0].soil_defaults[0][0].porosity_decay,
		patch[0].soil_defaults[0][0].soil_depth,
		0,
		-1*patch[0].sat_deficit);
	return;
} /*end input_new_patch.c*/

