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
#include "rhessys.h"
#include "phys_constants.h"
#define ONE 1.0


 void  skip_patch(
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
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].x = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].y = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].z = ltmp;*/

	fscanf(world_file,"%d",&(ltmp));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(ltmp));
	read_record(world_file, record);

 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].area = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].slope = ltmp * DtoR;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].lna = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].Ksat_vertical = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	if (command_line[0].stdev_flag == 1) {
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
		/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].std = ltmp;*/
		}
		
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].rz_storage = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].unsat_storage = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].sat_deficit = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.water_equivalent_depth = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.water_depth = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.T = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.surface_age = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snowpack.energy_deficit = ltmp;*/

	if (command_line[0].snow_scale_flag == 1) {
 		fscanf(world_file,"%lf",&(ltmp));
		read_record(world_file, record);
	/*	if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].snow_redist_scale = ltmp;*/
		}


 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter.cover_fraction = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter.rain_stored = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter_cs.litr1c = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].litter_ns.litr1n = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].litter_cs.litr2c = ltmp;
		patch[0].litter_ns.litr2n = patch[0].litter_cs.litr2c / CEL_CN;
		}*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].litter_cs.litr3c = ltmp;
		patch[0].litter_ns.litr3n = patch[0].litter_cs.litr3c / CEL_CN;
		}*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].litter_cs.litr4c = ltmp;
		patch[0].litter_ns.litr4n = patch[0].litter_cs.litr4c / LIG_CN;
		}*/

 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil1c = ltmp;
		patch[0].soil_ns.soil1n = patch[0].soil_cs.soil1c / SOIL1_CN;
		}*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].soil_ns.sminn = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  patch[0].soil_ns.nitrate = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil2c = ltmp;
		patch[0].soil_ns.soil2n = patch[0].soil_cs.soil2c / SOIL2_CN;
		}*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil3c = ltmp;
		patch[0].soil_ns.soil3n = patch[0].soil_cs.soil3c / SOIL3_CN;
		}*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ONE)  {
		patch[0].soil_cs.soil4c = ltmp;
		patch[0].soil_ns.soil4n = patch[0].soil_cs.soil4c / SOIL4_CN;
		}*/
	

	/*--------------------------------------------------------------*/
	/*	Read in the number of  patch base stations 					*/
	/*--------------------------------------------------------------*/
 	fscanf(world_file,"%d",&(dtmp));
	read_record(world_file, record);
	if (dtmp > 0)  {
		/*patch[0].num_base_stations = dtmp;*/
		/*--------------------------------------------------------------*/
		/*    Allocate a list of base stations for this patch.			*/
		/*--------------------------------------------------------------*/
		/*patch[0].base_stations = (struct base_station_object **)
			alloc(patch[0].num_base_stations *
			sizeof(struct base_station_object *),
			"base_stations","skip_patch" );*/
		/*--------------------------------------------------------------*/
		/*      Read each base_station ID and then point to that base_statio*/
		/*--------------------------------------------------------------*/
		for (i=0 ; i<dtmp; i++){
			fscanf(world_file,"%d",&(ltmp));
			read_record(world_file, record);
			/*--------------------------------------------------------------*/
			/*	Point to the appropriate base station in the base       	*/
			/*              station list for this world.					*/
			/*																*/
			/*--------------------------------------------------------------*/
	/*		patch[0].base_stations[i] = assign_base_station(
				base_stationID,
				num_world_base_stations,
				world_base_stations);*/
		} /*end for*/
	}
	
	return;
} /*end input_new_patch.c*/

