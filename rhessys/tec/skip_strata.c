/*--------------------------------------------------------------*/
/* 																*/
/*					input_new_strata						*/
/*																*/
/*	input_new_strata.c - creates a canopy_strata object	*/
/*																*/
/*	NAME														*/
/*	input_new_strata.c - creates a canopy_strata object	*/
/*																*/
/*	SYNOPSIS													*/
/*	void input_new_strata( 		*/
/*				struct	command_line_object	*command_line,		*/
/*				FILE	*world_file)						*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Allocates memory for a canopy strata object and reads in 	*/
/*	parameters for the object from an open hillslopes file.		*/
/*																*/
/*	Refer to construct_basin.c for a specification of the 		*/
/*	hillslopes file.											*/
/*																*/
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
#define ONE 1.0
#include "params.h"

void skip_strata(
											  struct	command_line_object	*command_line,
											  FILE	*world_file,
											  int		num_world_base_stations,
											  struct base_station_object **world_base_stations,
											  struct	default_object	*defaults,
											  struct	patch_object *patch,
											  struct canopy_strata_object     *canopy_strata)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
		int ,
		int ,
		struct base_station_object **);

	int compute_annual_turnover(struct epconst_struct,
		struct epvar_struct *,
		struct cstate_struct *);


	int	compute_annual_litfall(
		struct epconst_struct,
		struct phenology_struct *,
		struct cstate_struct *,
		int);


	int	update_rooting_depth(
		struct rooting_zone_object *,
		double,
		double,
		double,
		double,
		double);

	void	*alloc(	size_t,
		char	*,
		char	*);

	param *readtag_worldfile(int *,
				FILE *,
				char *);	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int	base_stationID;
	int	i, dtmp, num_lines;
	int	default_object_ID;
	char	record[MAXSTR];
	double 	rootc, ltmp;
	int	*paramCnt=0;
	param	*paramPtr=NULL;
	/*--------------------------------------------------------------*/
	/*	Read in the next canopy strata record for this patch.		*/
	/*--------------------------------------------------------------*/
	paramPtr = readtag_worldfile(&paramCnt,world_file,"Canopy_Strata");
		/*--------------------------------------------------------------*/
		/*	Read in the number of  strata base stations 					*/
		/*--------------------------------------------------------------*/
	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"n_basestations","%d",0,1);
	
		if (dtmp > 0) {
			/*canopy_strata[0].num_base_stations = dtmp;*/
			/*--------------------------------------------------------------*/
			/*    Allocate a list of base stations for this strata.			*/
			/*--------------------------------------------------------------*/
			/*canopy_strata[0].base_stations = (struct base_station_object **)
				alloc(canopy_strata[0].num_base_stations *
				sizeof(struct base_station_object *),"base_stations",
				"skip_strata");*/
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
		/*		canopy_strata[0].base_stations[i] = assign_base_station(
					base_stationID,
					num_world_base_stations,
					world_base_stations);*/
			} /*end for*/
		}

	if(paramPtr!=NULL){
	  free(paramPtr);
	}			 
	return;
} /*end input_new_strata.c*/

