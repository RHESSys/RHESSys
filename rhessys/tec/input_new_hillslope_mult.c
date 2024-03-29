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
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

void input_new_hillslope_mult(
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
	char		record[MAXSTR];
	double		ltmp;
	int		paramCnt=0;
	param		*paramPtr=NULL;		
	
	/*--------------------------------------------------------------*/
	/*	Read in the hillslope record from the world file.			*/
	/*--------------------------------------------------------------*/
	paramPtr = readtag_worldfile(&paramCnt,world_file,"Hillslope");

	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"x","%lf",1,1);	
	if (fabs(ltmp - NULLVAL) >= ZERO)  hillslope[0].x = ltmp * hillslope[0].x;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"y","%lf",1,1);		
	if (fabs(ltmp - NULLVAL) >= ZERO)  hillslope[0].y = ltmp * hillslope[0].y;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"z","%lf",1,1);		
	if (fabs(ltmp - NULLVAL) >= ZERO)  hillslope[0].z = ltmp * hillslope[0].z;


	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"hill_parm_ID","%d",hillslope[0].hill_parm_ID,1);
	 if (dtmp > 0)  hillslope[0].hill_parm_ID = dtmp;

	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"gw.storage","%lf",1,1);		
	if (fabs(ltmp - NULLVAL) >= ZERO)  hillslope[0].gw.storage = ltmp * hillslope[0].gw.storage;
	ltmp = getDoubleWorldfile(&paramCnt,&paramPtr,"gw.NO3","%lf",1,1);	
	if (fabs(ltmp - NULLVAL) >= ZERO)  hillslope[0].gw.NO3 = ltmp * hillslope[0].gw.NO3;

	dtmp = getIntWorldfile(&paramCnt,&paramPtr,"hillslope_n_basestations","%d",1,1);	
	
	/*--------------------------------------------------------------*/
	/*  Assign  defaults for this hillslope                             */
	/*--------------------------------------------------------------*/
	if (hillslope[0].hill_parm_ID > 0) {	
		i = 0;
		while (defaults[0].hillslope[i].ID != hillslope[0].hill_parm_ID) {
			i++;
			/*--------------------------------------------------------------*/
			/*  Report an error if no match was found.  Otherwise assign    */
			/*  the default to point to this hillslope.                         */
			/*--------------------------------------------------------------*/
			if ( i>= defaults[0].num_hillslope_default_files ){
				fprintf(stderr,
					"\nFATAL ERROR: in input_new_hillslope,hillslope default ID %d not found.\n",
					hillslope[0].hill_parm_ID);
				exit(EXIT_FAILURE);
			}
		} /* end-while */
		hillslope[0].defaults[0] = &defaults[0].hillslope[i];
	}
	/*--------------------------------------------------------------*/
	/*	Allocate a list of base stations for this hillslope.		*/
	/*--------------------------------------------------------------*/
 	/*  fscanf(world_file,"%d",&(dtmp));
	read_record(world_file, record);*/
	if (dtmp > 0) {
		hillslope[0].num_base_stations = dtmp * hillslope[0].num_base_stations;
		hillslope[0].base_stations = (struct base_station_object **)
			alloc(hillslope[0].num_base_stations *
			sizeof(struct base_station_object *),"base_stations",
			"input_new_hillslopes" );
		/*--------------------------------------------------------------*/
		/*	Read each base_station ID and then point to that base_statio*/
		/*--------------------------------------------------------------*/
		for (i=0 ; i<hillslope[0].num_base_stations; i++){
			fscanf(world_file,"%d",&(base_stationID));
			read_record(world_file, record);
			/*--------------------------------------------------------------*/
			/*		Point to the appropriate base station in the base       */
			/*		station list for this world.							*/
			/*																*/
			/*--------------------------------------------------------------*/
			hillslope[0].base_stations[i] = assign_base_station(
				base_stationID,
				num_world_base_stations,
				world_base_stations);
		} /*end for*/
	}
	
	if(paramPtr!=NULL){
	  free(paramPtr);
	}
	return;
} /*end input_new_hillslope.c*/
