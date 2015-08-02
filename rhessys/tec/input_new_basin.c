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
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"
#include "params.h"

void input_new_basin(
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
	param	*readtag_worldfile(int *,
				  FILE *,
				  char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	base_stationID;
	int		i,dtmp;
	int		default_object_ID;
	char		record[MAXSTR];
	double		ltmp;
	int		paramCnt=0;
	param		*paramPtr=NULL;	
	
	/*--------------------------------------------------------------*/
	/*	Read in the basinID.									*/
	/*--------------------------------------------------------------*/
	paramPtr=readtag_worldfile(&paramCnt,world_file,"Basin");
	
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"x","%lf",basin[0].x,1);
	if (fabs(ltmp - NULLVAL) >= ZERO)  basin[0].x = ltmp;
	/*fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);*/
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"y","%lf",basin[0].y,1);	
	if (fabs(ltmp - NULLVAL) >= ZERO)  basin[0].y = ltmp;
	/*fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);*/
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"z","%lf",basin[0].z,1);	
	if (fabs(ltmp - NULLVAL) >= ZERO)  basin[0].z = ltmp;
         /*fscanf(world_file,"%d",&(default_object_ID));
	read_record(world_file, record);*/
	default_object_ID = getIntWorldfile(&paramCnt,&paramPtr,"basin_parm_ID","%d",0,1);		
         /*fscanf(world_file,"%lf",&(ltmp));
	read_record(world_file, record);*/
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"latitude","%lf",basin[0].latitude,1);	
	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"n_basestations","%d",basin[0].num_base_stations,1);	
	
	if (fabs(ltmp - NULLVAL) >= ZERO)  {
		basin[0].latitude = ltmp;
		basin[0].cos_latitude = cos(basin[0].latitude*DtoR);
		basin[0].sin_latitude = sin(basin[0].latitude*DtoR);
		}
	
	/*--------------------------------------------------------------*/
	/*    Allocate a list of base stations for this basin.			*/
	/*--------------------------------------------------------------*/
  	/*  fscanf(world_file,"%d",&(dtmp));
	read_record(world_file, record);*/
	
	if (dtmp > 0) {
		basin[0].num_base_stations = dtmp;
		basin[0].base_stations = (struct base_station_object **)
			alloc(basin[0].num_base_stations *
			sizeof(struct base_station_object *),"base_stations","input_new_basin");
		
		/*--------------------------------------------------------------*/
		/*      Read each base_station ID and then point to that base_statio*/
		/*--------------------------------------------------------------*/
		for (i=0 ; i<basin[0].num_base_stations; i++) {
			fscanf(world_file,"%d",&(base_stationID));
			read_record(world_file, record);
			/*--------------------------------------------------------------*/
			/*	Point to the appropriate base station in the base       	*/
			/*              station list for this world.					*/
			/*--------------------------------------------------------------*/
			basin[0].base_stations[i] = assign_base_station(
				base_stationID,
				num_world_base_stations,
				world_base_stations);
			
		} /*end for*/
	}	
	
	/*--------------------------------------------------------------*/
	/*  Assign  defaults for this basin                             */
	/*--------------------------------------------------------------*/
	if (default_object_ID > 0) {
		i = 0;
		while (defaults[0].basin[i].ID != default_object_ID) {
			i++;
			/*--------------------------------------------------------------*/
			/*  Report an error if no match was found.  Otherwise assign    */
			/*  the default to point to this basin.                         */
			/*--------------------------------------------------------------*/
			if ( i>= defaults[0].num_basin_default_files ){
				fprintf(stderr,
					"\nFATAL ERROR: in input_new_basin,basin default ID %d not found.\n",
					default_object_ID);
				exit(EXIT_FAILURE);
			}
		} /* end-while */
		basin[0].defaults[0] = &defaults[0].basin[i];
	}
	
	if(paramPtr!=NULL){
	  free(paramPtr);
	}


	return;
} /*end input_new_basin.c*/
