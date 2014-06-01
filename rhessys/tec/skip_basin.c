/*--------------------------------------------------------------*/
/* 																*/
/*					input_new_basin								*/
/*																*/
/*	input_new_basin.c - creates a basin object					*/
/*																*/
/*	NAME														*/
/*	input_new_basin.c - creates a basin object					*/
/*																*/
/*	SYNOPSIS													*/
/*	void input_new_basin(										*/
/*			struct	command_line_object	*command_line,			*/
/*			FILE	*world_file									*/
/*			int		num_world_base_stations,					*/
/*			struct base_station_object	**world_base_stations,	*/
/*			struct basin_object	**basin_list,					*/
/*			struct default_object *defaults,
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	inputs a new basin - changing only state variable with non-NULLVAL 			*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

void skip_basin(
									 struct	command_line_object	*command_line,
									 FILE	*world_file,
									 int		num_world_base_stations,
									 struct base_station_object	**world_base_stations,
									 struct	default_object	*defaults,
									 struct	basin_object *basin)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
								int,
								int,
								struct base_station_object **);
	
	
	void	*alloc( 	size_t, char *, char *);
	
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	base_stationID;
	int		i,dtmp;
	int		default_object_ID;
	char		record[MAXSTR];
	double		ltmp;
	
	
	/*--------------------------------------------------------------*/
	/*	Read in the basinID.									*/
	/*--------------------------------------------------------------*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  basin[0].x = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  basin[0].y = ltmp;*/
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  basin[0].z = ltmp;*/
 	fscanf(world_file,"%d",&(ltmp));
	read_record(world_file, record);
 	fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);
	/*if (fabs(ltmp - NULLVAL) >= ZERO)  {
		basin[0].latitude = ltmp;
		basin[0].cos_latitude = cos(basin[0].latitude*DtoR);
		basin[0].sin_latitude = sin(basin[0].latitude*DtoR);
		}*/
	
	/*--------------------------------------------------------------*/
	/*    Allocate a list of base stations for this basin.			*/
	/*--------------------------------------------------------------*/
 	fscanf(world_file,"%d",&(dtmp));
	read_record(world_file, record);
	if (dtmp > 0) {
		/*basin[0].num_base_stations = dtmp;*/
		/*basin[0].base_stations = (struct base_station_object **)
			alloc(basin[0].num_base_stations *
			sizeof(struct base_station_object *),"base_stations","skip_basin");*/
		
		/*--------------------------------------------------------------*/
		/*      Read each base_station ID and then point to that base_statio*/
		/*--------------------------------------------------------------*/
		for (i=0 ; i<dtmp; i++) {
			fscanf(world_file,"%d",&(ltmp));
			read_record(world_file, record);
			/*--------------------------------------------------------------*/
			/*	Point to the appropriate base station in the base       	*/
			/*              station list for this world.					*/
			/*--------------------------------------------------------------*/
			/*basin[0].base_stations[i] = assign_base_station(
				base_stationID,
				num_world_base_stations,
				world_base_stations);*/
			
		} /*end for*/
	}	
	
	

	return;
} /*end input_new_basin.c*/
