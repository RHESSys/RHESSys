/*--------------------------------------------------------------*/
/* 																*/
/*					input_new_hillslope							*/
/*																*/
/*	input_new_hillslope.c - creates a hillslope object			*/
/*																*/
/*	NAME														*/
/*	input_new_hillslope.c - creates a hillslope object			*/
/*																*/
/*	SYNOPSIS													*/
/* 	void input_new_hillslope(									*/
/*			struct	command_line_object	*command_line,			*/
/*			FILE	*world_file,								*/
/*			int		num_world_base_stations,					*/
/*			struct	base_station_object	**world_base_stations,	*/
/*			struct	hillslope_object	**hillslope_list,		*/
/*			struct	default_object		*defaults)				*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Reads the world file to get data for 1 hillslope.			*/
/*																*/
/*	Points to base_stations assigned to the hillslope.			*/
/*																*/
/*	Invokes construction of zones in the hillslope.				*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	March 4, 1997 C. Tague										*/
/*			- removed m parameter to hillslope defaults to		*/
/*			 be consistent with patch level soil parameteriza	*/
/*	March 9, 1997 C. Tague										*/
/*			- added baseflow as an input state					*/
/*	Sept 21, 1998 C. Tague						*/
/*		removed input of lambda - calculated from patch lambdas */
/*		in topmodel					 	*/

/*																*/
/*																*/
/*	Original code, January 16, 1996.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "params.h"

void skip_hillslope(
											 struct	command_line_object	*command_line,
											 FILE	*world_file,
											 int		num_world_base_stations,
											 struct	base_station_object	**world_base_stations,
											 struct	default_object	*defaults,
											 struct hillslope_object *hillslope)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
		int	,
		int	,
		struct base_station_object **);
	
	void	*alloc(	size_t,
		char	*,
		char	*);
	param	*readtag_worldfile(int *,
				  FILE *,
				  char *);	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i,j, dtmp;
	int		base_stationID;
	int		default_object_ID;
	char		record[MAXSTR];
	double		ltmp;
	int		paramCnt=0;
	param		*paramPtr=NULL;	
	/*--------------------------------------------------------------*/
	/*	Read in the hillslope record from the world file.			*/
	/*--------------------------------------------------------------*/
 	paramPtr = readtag_worldfile(&paramCnt,world_file,"Hillslope");


	/*--------------------------------------------------------------*/
	/*	Allocate a list of base stations for this hillslope.		*/
	/*--------------------------------------------------------------*/
	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"hillslope_n_basestations","%d",0,1);

	if (dtmp > 0) {
		/*hillslope[0].num_base_stations = dtmp;*/
		/*hillslope[0].base_stations = (struct base_station_object **)
			alloc(hillslope[0].num_base_stations *
			sizeof(struct base_station_object *),"base_stations",
			"input_new_hillslopes" );*/
		/*--------------------------------------------------------------*/
		/*	Read each base_station ID and then point to that base_statio*/
		/*--------------------------------------------------------------*/
		for (i=0 ; i<dtmp; i++){
			fscanf(world_file,"%d",&(ltmp));
			read_record(world_file, record);
			/*--------------------------------------------------------------*/
			/*		Point to the appropriate base station in the base       */
			/*		station list for this world.							*/
			/*																*/
			/*--------------------------------------------------------------*/
	/*		hillslope[0].base_stations[i] = assign_base_station(
				base_stationID,
				num_world_base_stations,
				world_base_stations);*/
		} /*end for*/
	}
	if(paramPtr !=NULL){
	  free(paramPtr);
	}	
	return;
} /*end input_new_hillslope.c*/
