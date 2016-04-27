/*--------------------------------------------------------------*/
/* 																*/
/*					construct_hillslope							*/
/*																*/
/*	construct_hillslope.c - creates a hillslope object			*/
/*																*/
/*	NAME														*/
/*	construct_hillslope.c - creates a hillslope object			*/
/*																*/
/*	SYNOPSIS													*/
/* 	void construct_hillslope(									*/
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

struct hillslope_object *construct_hillslope(
											 struct	command_line_object	*command_line,
											 FILE	*world_file,
											 int	*num_world_base_stations,
											 struct	base_station_object	**world_base_stations,
											 struct	default_object	*defaults,
											 struct base_station_ncheader_object *base_station_ncheader,
											 struct world_object *world)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
		int	,
		int	,
		struct base_station_object **);
	
	struct zone_object *construct_zone(
		struct command_line_object *,
		FILE	*,
		int		*num_world_base_stations,
		struct	base_station_object	**world_base_stations,
		struct	default_object *,
		struct base_station_ncheader_object *,
	    struct world_object *);
	
	void	*alloc(	size_t,
		char	*,
		char	*);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i,j;
	int		base_stationID;
	int		default_object_ID;
	char		record[MAXSTR];
	struct	hillslope_object *hillslope;
	
	/*--------------------------------------------------------------*/
	/*	Allocate a hillslope object.								*/
	/*--------------------------------------------------------------*/
	hillslope = (struct hillslope_object *) alloc( 1 *
		sizeof( struct hillslope_object ),"hillslope",
		"construct_hillsope" );
	
	/*--------------------------------------------------------------*/
	/*	Create the grow extension to the hillslope object if 		*/
	/*	needed.														*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].grow_flag  == 1){
		/*--------------------------------------------------------------*/
		/*		Allocate the grow object.								*/
		/*--------------------------------------------------------------*/
		hillslope[0].grow = (struct grow_hillslope_object *)
			alloc(1 * sizeof( struct grow_hillslope_object ),
			"grow","construct_hillslope");
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Read in the hillslope record from the world file.			*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(hillslope[0].ID));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(hillslope[0].x));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(hillslope[0].y));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(hillslope[0].z));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(default_object_ID));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(hillslope[0].gw.storage));
	read_record(world_file, record);
	fscanf(world_file,"%lf",&(hillslope[0].gw.NO3));
	read_record(world_file, record);
	fscanf(world_file,"%d",&(hillslope[0].num_base_stations));
	read_record(world_file, record);

	hillslope[0].streamflow_NO3 = 0.0;	
	hillslope[0].streamflow_NH4 = 0.0;	
	/*--------------------------------------------------------------*/
	/*  Assign  defaults for this hillslope                             */
	/*--------------------------------------------------------------*/
	hillslope[0].defaults = (struct hillslope_default **)
		alloc( sizeof(struct hillslope_default *),"defaults",
		"construct_hillslopes" );
	
	i = 0;
	while (defaults[0].hillslope[i].ID != default_object_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this hillslope.                         */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_hillslope_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_hillslope,hillslope default ID %d not found.\n",
				default_object_ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	hillslope[0].defaults[0] = &defaults[0].hillslope[i];
	/*--------------------------------------------------------------*/
	/*	Allocate a list of base stations for this hillslope.		*/
	/*--------------------------------------------------------------*/
	hillslope[0].base_stations = (struct base_station_object **)
		alloc(hillslope[0].num_base_stations *
		sizeof(struct base_station_object *),"base_stations",
		"construct_hillslopes" );
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
			*num_world_base_stations,
			world_base_stations);
	} /*end for*/
	
	/*--------------------------------------------------------------*/
	/*	Read in number of zones in this hillslope.					*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(hillslope[0].num_zones));
	read_record(world_file, record);
	
	/*--------------------------------------------------------------*/
	/*	Allocate list of pointers to zone objects .					*/
	/*--------------------------------------------------------------*/
	hillslope[0].zones = ( struct zone_object ** )
		alloc( hillslope[0].num_zones * sizeof( struct zone_object *),
		"zones","construct_hillslopes");
	
	hillslope[0].streamflow_NO3 = 0.0;
	hillslope[0].streamflow_NH4 = 0.0;
	hillslope[0].streamflow_DON = 0.0;
	hillslope[0].streamflow_DOC = 0.0;
	hillslope[0].gw.DON = 0.0;
	hillslope[0].gw.DOC = 0.0;
	hillslope[0].gw.NH4 = 0.0;
	/*--------------------------------------------------------------*/
	/*	Construct the zones in this hillslope.						*/
	/*	and calculate hillslope area								*/
	/*--------------------------------------------------------------*/

	hillslope[0].area = 0.0;
	hillslope[0].riparian_area = 0.0;
	hillslope[0].slope = 0.0;
	hillslope[0].aggdefs.soil_water_cap = 0.0;
	hillslope[0].aggdefs.m = 0.0;
	hillslope[0].aggdefs.psi_air_entry = 0.0;
	hillslope[0].aggdefs.porosity_0 = 0.0;
	hillslope[0].aggdefs.porosity_decay = 0.0;
	hillslope[0].aggdefs.N_decay_rate = 0.0;
	hillslope[0].aggdefs.DOM_decay_rate = 0.0;
	hillslope[0].aggdefs.active_zone_z = 0.0;
	hillslope[0].aggdefs.Ksat_0 = 0.0;
	hillslope[0].aggdefs.soil_depth = 0.0;
	hillslope[0].aggdefs.NO3_adsorption_rate = 0.0;
	hillslope[0].aggdefs.NH4_adsorption_rate = 0.0;
	hillslope[0].aggdefs.DON_adsorption_rate = 0.0;
	hillslope[0].aggdefs.DOC_adsorption_rate = 0.0;
	
	for ( i=0 ; i<hillslope[0].num_zones ; i++ ){
		hillslope[0].zones[i] = construct_zone( command_line,
			world_file,
			num_world_base_stations,
			world_base_stations, defaults,
			base_station_ncheader, world);
		for	 (j =0; j < hillslope[0].zones[i][0].num_patches ; j++) {
			hillslope[0].area += hillslope[0].zones[i][0].patches[j][0].area;
			if (hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].ID == 42) 
				hillslope[0].riparian_area += hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].slope += hillslope[0].zones[i][0].patches[j][0].slope *
					hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.soil_water_cap +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].soil_water_cap *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.Ksat_0 +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].Ksat_0 *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.m +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].m *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.psi_air_entry +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].psi_air_entry*
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.porosity_0 +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].porosity_0 *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.porosity_decay +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].porosity_decay *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.DOM_decay_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].DOM_decay_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.N_decay_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].N_decay_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.active_zone_z +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].active_zone_z *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.soil_depth +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].soil_depth *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.NO3_adsorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].NO3_adsorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.NH4_adsorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].NH4_adsorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.DON_adsorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].DON_adsorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.DOC_adsorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].DOC_adsorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;

		}
	}
	hillslope[0].slope /= hillslope[0].area;
	hillslope[0].aggdefs.psi_air_entry /= hillslope[0].area;
	hillslope[0].aggdefs.m /= hillslope[0].area;
	hillslope[0].aggdefs.Ksat_0 /= hillslope[0].area;
	hillslope[0].aggdefs.porosity_0 /= hillslope[0].area;
	hillslope[0].aggdefs.porosity_decay /= hillslope[0].area;
	hillslope[0].aggdefs.N_decay_rate /= hillslope[0].area;
	hillslope[0].aggdefs.DOM_decay_rate /= hillslope[0].area;
	hillslope[0].aggdefs.active_zone_z /= hillslope[0].area;
	hillslope[0].aggdefs.soil_depth /= hillslope[0].area;
	hillslope[0].aggdefs.soil_water_cap /= hillslope[0].area;
	hillslope[0].aggdefs.NO3_adsorption_rate /= hillslope[0].area;
	hillslope[0].aggdefs.NH4_adsorption_rate /= hillslope[0].area;
	hillslope[0].aggdefs.DON_adsorption_rate /= hillslope[0].area;
	hillslope[0].aggdefs.DOC_adsorption_rate /= hillslope[0].area;


	/*--------------------------------------------------------------*/
	/*      initialize accumulator variables for this patch         */
	/*--------------------------------------------------------------*/
	hillslope[0].acc_month.snowpack = 0.0;
	hillslope[0].acc_month.et = 0.0;
	hillslope[0].acc_month.streamflow = 0.0;
	hillslope[0].acc_month.length = 0;
	hillslope[0].acc_month.denitrif = 0.0;
	hillslope[0].acc_month.DOC_loss = 0.0;
	hillslope[0].acc_month.DON_loss = 0.0;
	hillslope[0].acc_month.stream_NO3 = 0.0;
	hillslope[0].acc_month.stream_NH4 = 0.0;
	hillslope[0].acc_month.psn = 0.0;

	hillslope[0].acc_year.num_threshold = 0;
	hillslope[0].acc_year.et = 0.0;
	hillslope[0].acc_year.streamflow = 0.0;
	hillslope[0].acc_year.length = 0;
	hillslope[0].acc_year.denitrif = 0.0;
	hillslope[0].acc_year.DOC_loss = 0.0;
	hillslope[0].acc_year.DON_loss = 0.0;
	hillslope[0].acc_year.stream_NO3 = 0.0;
	hillslope[0].acc_year.stream_NH4 = 0.0;
	hillslope[0].acc_year.psn = 0.0;

	return(hillslope);
} /*end construct_hillslope.c*/
