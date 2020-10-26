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
#include "params.h"

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
	param	*readtag_worldfile(int *,
				  FILE *,
				  char *);	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		base_stationID;
	int		i, dtmp;
	int		soil_default_object_ID;
	int		landuse_default_object_ID;
	char		record[MAXSTR];
	double	mpar, ltmp;
	int		paramCnt=0;
	param		*paramPtr=NULL;
	
	/*--------------------------------------------------------------*/
	/*	Read in the next patch record for this hillslope.			*/
	/*--------------------------------------------------------------*/
 	paramPtr = readtag_worldfile(&paramCnt,world_file,"Patch");


	/*--------------------------------------------------------------*/
	/*	Read in the number of  patch base stations 					*/
	/*--------------------------------------------------------------*/
	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"patch_n_basestations","%d",0,1);
      	
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
	if(paramPtr!=NULL){
	  free(paramPtr);
	}	
	return;
} /*end input_new_patch.c*/

