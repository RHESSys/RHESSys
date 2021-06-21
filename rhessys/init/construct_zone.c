/*--------------------------------------------------------------*/
/* 																*/
/*					construct_zone								*/
/*																*/
/*	construct_zone.c - creates a zone object					*/
/*																*/
/*	NAME														*/
/*	construct_zone.c - creates a zone object					*/
/*																*/
/*	SYNOPSIS													*/
/*	struct zone_object *construct_zone(										*/
/*					struct	command_line_object	*command_line,	*/
/*					FILE	*world_file,					*/
/*					struct	default_object						*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Allocates memory for a zone object and reads in parameters  */
/*	from an already opened hillslopes file.						*/
/*																*/
/*	Invokes construction of patches in the zone.				*/
/*																*/
/*	Refer to construct_basin.c for a specification of the 		*/
/*	hillslopes file.											*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We assume that the FILE pointers to the 					*/
/*	hillslope file are positioned properly.						*/
/*	 															*/
/*	We assume that the hillslope file has correct				*/
/*	syntax.														*/
/*																*/
/*	Original code, January 16, 1996.							*/
/*																*/
/*	 															*/ 
/* May 15, 1997		C.Tague					*/
/*	- C assumes radians so slope and aspect must 		*/
/*	be converted from degrees				*/
/* July 28, 1997	C.tague					*/
/*	- isohyet changed to a precip. lapse rate 		*/
/*													*/
/*	Sep 2, 1997 - RAF								*/
/*	Removed all references to extended objects or	*/
/*	grow objects in code.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"
#include "params.h"

struct zone_object *construct_zone(
								   struct	command_line_object	*command_line,
								   FILE		*world_file,
								   int		*num_world_base_stations,
								   struct	base_station_object **world_base_stations,
								   struct	default_object	*defaults,
								   struct	base_station_ncheader_object *base_station_ncheader,
								   struct	world_object *world)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct	base_station_object *assign_base_station(
		int ,
		int ,
		struct base_station_object **);
	
	struct	base_station_object *assign_base_station_xy(
		float ,
		float ,
		int ,
		int *,
        struct base_station_object **,
        const struct base_station_ncheader_object *
        //#ifdef LIU_NETCDF_READER
        //,double
        //#endif
        #ifdef FIND_STATION_BASED_ON_ID
        ,const int
        #endif
                );
	
	struct patch_object *construct_patch(
		struct command_line_object *,
		FILE	*,
		int		num_world_base_stations,
		struct base_station_object **world_base_stations,
		struct	default_object	*defaults);

	struct patch_family_object *construct_patch_family(
		struct zone_object *zone,
		int     patch_family_ID,
		struct 	command_line_object *command_line);
	
	struct base_station_object *construct_netcdf_grid(
        #ifdef LIU_NETCDF_READER
        struct base_station_object *,
        #endif
		struct command_line_object *command_line,
    struct base_station_ncheader_object *,
		int *,
		float,
		float,
		float,
        struct date*,
        struct date*);
	
	int get_netcdf_xy(char *, char *, char *, float, float, float, float *, float *);
	
	void	*alloc(size_t, char *, char *);
	double	atm_pres( double );
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		base_stationID;
	int		i, k, j;
	int		notfound;
    int     basestation_id;
	float   base_x, base_y;
	double	sum_patch_area; 
	char	record[MAXSTR];
	struct	zone_object *zone;
	int paramCnt=0;
	param *paramPtr=NULL;
	
	notfound = 0;
	base_x = 0.0;
	base_y = 0.0;
	j = 0;
	k = 0;	
	sum_patch_area = 0.0;

	/*--------------------------------------------------------------*/
	/*	Allocate a zone object.								*/
	/*--------------------------------------------------------------*/
	zone = (struct zone_object *) alloc( 1 *
		sizeof( struct zone_object ),"zone","construct_zone" );
	/*--------------------------------------------------------------*/
	/*	Read in the next zone record for this hillslope.			*/
	/*--------------------------------------------------------------*/

	paramPtr=readtag_worldfile(&paramCnt,world_file,"Zone");

	zone[0].ID = getIntWorldfile(&paramCnt,&paramPtr,"zone_ID","%d",-9999,0);
	zone[0].x = getDoubleWorldfile(&paramCnt,&paramPtr,"x","%lf",0,1);
	zone[0].y = getDoubleWorldfile(&paramCnt,&paramPtr,"y","%lf",0,1);
	zone[0].z = getDoubleWorldfile(&paramCnt,&paramPtr,"z","%lf",-9999,0);
	zone[0].zone_parm_ID = getIntWorldfile(&paramCnt,&paramPtr,"zone_parm_ID","%d",-9999,0);
	zone[0].area = getDoubleWorldfile(&paramCnt,&paramPtr,"area","%lf",-9999,0);
	zone[0].slope = getDoubleWorldfile(&paramCnt,&paramPtr,"slope","%lf",-9999,0);
	zone[0].aspect = getDoubleWorldfile(&paramCnt,&paramPtr,"aspect","%lf",-9999,0);
	zone[0].precip_lapse_rate = getDoubleWorldfile(&paramCnt,&paramPtr,"precip_lapse_rate","%lf",1.0,1);
	zone[0].e_horizon = getDoubleWorldfile(&paramCnt,&paramPtr,"e_horizon","%lf",-9999,0);
	zone[0].w_horizon = getDoubleWorldfile(&paramCnt,&paramPtr,"w_horizon","%lf",-9999,0);
	zone[0].num_base_stations = getIntWorldfile(&paramCnt,&paramPtr,"zone_n_basestations","%d",0,0);	
	/*--------------------------------------------------------------*/
	/* check for negative horizons - these may occur if using a GIS function that is computing topographic rather than */
	/* sun horizons - for us, those will be zero			*/
	/*--------------------------------------------------------------*/

	if (zone[0].e_horizon < ZERO) zone[0].e_horizon = 0.0;
	if (zone[0].w_horizon < ZERO) zone[0].w_horizon = 0.0;

	/*--------------------------------------------------------------*/
	/* if using stem density to change e and w horizons need to remember originals */
	/*--------------------------------------------------------------*/
	zone[0].e_horizon_topog = zone[0].e_horizon;
	zone[0].w_horizon_topog = zone[0].w_horizon;

	/*--------------------------------------------------------------*/
	/*	convert from degrees to radians for slope and aspects 	*/
	/*--------------------------------------------------------------*/
	zone[0].aspect = zone[0].aspect * DtoR;
	zone[0].slope = zone[0].slope * DtoR;
	/*--------------------------------------------------------------*/
	/*	Define cos and sine of slopes and aspects.					*/
	/*--------------------------------------------------------------*/
	zone[0].cos_aspect = cos(zone[0].aspect);
	zone[0].cos_slope = cos(zone[0].slope);
	zone[0].sin_aspect = sin(zone[0].aspect);
	zone[0].sin_slope = sin(zone[0].slope);
	/*--------------------------------------------------------------*/
	/*      Initialize accumulator variables                        */
	/*--------------------------------------------------------------*/
	zone[0].acc_month.K_direct = 0.0;
	zone[0].acc_month.K_diffuse = 0.0;
	zone[0].acc_month.tmax = 0.0;
	zone[0].acc_month.tmin = 0.0;
	zone[0].acc_month.precip = 0.0;
	zone[0].acc_month.length = 0;
	/*--------------------------------------------------------------*/
	/*	Define hourly array zone				*/
	/*--------------------------------------------------------------*/
	zone[0].hourly = (struct zone_hourly_object *) alloc(
		sizeof( struct zone_hourly_object ),
		"hourly","zone_hourly");
	/*--------------------------------------------------------------*/
	/*	Assign	defaults for this zone								*/
	/*--------------------------------------------------------------*/
	zone[0].defaults = (struct zone_default **)
		alloc( sizeof(struct zone_default *),"defaults",
		"construct_zone" );
	i = 0;
	while (defaults[0].zone[i].ID != zone[0].zone_parm_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this zone.						    */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_zone_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_zone, zone default ID %d not found.\n",
				zone[0].zone_parm_ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	zone[0].defaults[0] = &defaults[0].zone[i];
	
	if ( command_line[0].verbose_flag == -3 ){
		printf("\nConstructing zone base stations %d", zone[0].ID);
	}	
	
	/*--------------------------------------------------------------*/
	/*	Allocate a list of base stations for this zone.          */
	/*--------------------------------------------------------------*/
	zone[0].base_stations = (struct base_station_object **)
		alloc(zone[0].num_base_stations *
		sizeof(struct base_station_object *),
		"base_stations","construct_zone" );
	/*--------------------------------------------------------------*/
	/* NON NETCDF BASE STATIONS */
	/*--------------------------------------------------------------*/
	/*	Read each base_station ID and then point to that base station */
	/*--------------------------------------------------------------*/
	if (command_line[0].gridded_netcdf_flag == 0){
	for (i=0 ; i<zone[0].num_base_stations ; i++ ){
		fscanf(world_file,"%d",&(base_stationID));
		read_record(world_file, record);
		/*--------------------------------------------------------------*/
		/*  Point to the appropriate base station in the base           */
		/*              station list for this world.                    */
		/*                                                              */
		/*--------------------------------------------------------------*/
		zone[0].base_stations[i] =	assign_base_station(
			base_stationID,
			*num_world_base_stations,
			world_base_stations);
	} /*end for*/
	}
	else {
        fscanf(world_file,"%d",&basestation_id);
        read_record(world_file, record);
	}
	/*--------------------------------------------------------------*/
	/* NETCDF BASE STATIONS                                         */
	/*--------------------------------------------------------------*/
	/*	Construct the list of base stations.		                  	*/
	/*--------------------------------------------------------------*/


	if (command_line[0].dclim_flag == 0) {
		if(command_line[0].gridded_netcdf_flag == 1){
			/*--------------------------------------------------------------*/
			/*	Construct the base_stations.				                        */
			/*--------------------------------------------------------------*/
			if ( command_line[0].verbose_flag == -3 ){
                //printf("\n   Constructing base stations from %s: zoney=%lf zonex=%lf num=%d resolution_meter=%lf base_y=%lf base_x=%lf",
                printf("\n   Constructing base stations from %s: zoney=%lf zonex=%lf num=%d base_y=%lf base_x=%lf",
					   base_station_ncheader[0].netcdf_tmax_filename,
					   zone[0].y,
					   zone[0].x,
					   *num_world_base_stations,
                       // base_station_ncheader[0].resolution_meter,
					   base_y,
					   base_x);
				printf("\n   STARTING CLOSEST CELL: y=%lf x=%lf",base_y,base_x);
			}

#ifndef FIND_STATION_BASED_ON_ID
			/* Identify centerpoint coords for closest netcdf cell to zone x, y */
			k = get_netcdf_xy(base_station_ncheader[0].netcdf_tmax_filename, 
							  base_station_ncheader[0].netcdf_y_varname,
							  base_station_ncheader[0].netcdf_x_varname,
                              (float)zone[0].y,
                              (float)zone[0].x,
                              base_station_ncheader[0].resolution_meter,
							  &(base_y), 
							  &(base_x));
			if ( command_line[0].verbose_flag == -3 ){
				printf("\n   CLOSEST CELL: y=%lf x=%lf num=%d",base_y,base_x,*num_world_base_stations);
			}
			if(k == -1){
              fprintf(stderr,"can't locate station data in netcdf according to zone coordinates!\n");
				exit(0);
			}
#endif
			/* Assign base station based on closest found coordinates */
			if (*num_world_base_stations > 0) {
				zone[0].base_stations[0] =	assign_base_station_xy(
																   base_x,
																   base_y,
																   *num_world_base_stations,
																   &(notfound),
                                        world_base_stations,
                                        world[0].base_station_ncheader
                                        //#ifdef LIU_NETCDF_READER
                                        //,base_station_ncheader[0].resolution_meter / 3.0
                                        //#endif
                                        #ifdef FIND_STATION_BASED_ON_ID
                                        ,basestation_id
                                        #endif
                                        );
			}
			else notfound = 1;
			/* If station is not already in list, add it */
#ifdef LIU_NETCDF_READER

      if (notfound) {
       fprintf(stderr,"can't locate station data for ID:%d in netcdf!!!\n", basestation_id);
       exit(0);
      }
#else
			if (notfound == 1) {
				if ( command_line[0].verbose_flag == -3 ){
					printf("\n   Starting construct_netcdf_grid: file=%s num=%d",
						   world[0].base_station_files[0],
						   *num_world_base_stations);
				}
				j = *num_world_base_stations;
				world_base_stations[j] = construct_netcdf_grid( 
                                 command_line,
															   base_station_ncheader,  
															   num_world_base_stations,
															   base_x,
															   base_y,
															   zone[0].z,
                                                               &(world[0].start_date),
                                                               &(world[0].duration));
				world[0].num_base_stations = *num_world_base_stations;
				if ( command_line[0].verbose_flag == -3 ){
					printf("\n   Assigning base station: file=%s num=%d lai=%lf notfound=%d lastid=%d numworld=%d ID=%d LAI=%lf",
						   world[0].base_station_files[0],
						   *num_world_base_stations,
						   (*(world_base_stations[j])).effective_lai,
						   notfound,
						   base_station_ncheader[0].lastID,
						   world[0].num_base_stations,
						   (*(world_base_stations[j])).ID,
						   (*(world_base_stations[j])).effective_lai);
				}
				/* Now link the zone to the newly added base station */
				zone[0].base_stations[0] = world_base_stations[j];
				
				if ( command_line[0].verbose_flag == -3 ){
					printf("\n   Zone base station: lai=%lf tmin=%lf tmax=%lf rain=%lf",
						   (*(zone[0].base_stations[0])).effective_lai,
						   (*(zone[0].base_stations[0])).daily_clim[0].tmin[0],
						   (*(zone[0].base_stations[0])).daily_clim[0].tmax[0],
						   (*(zone[0].base_stations[0])).daily_clim[0].rain[0]);
				}
			}
            #endif
		}
	}
	
	if ( command_line[0].verbose_flag == -3 ){
		printf("\n   Ending zone base station: num=%d worldnum=%d",*num_world_base_stations,world[0].num_base_stations);
	}
	
	/*--------------------------------------------------------------*/
	/*	Read in number of patches in this zone.						*/
	/*--------------------------------------------------------------*/
	fscanf(world_file,"%d",&(zone[0].num_patches));
	read_record(world_file, record);

	/*--------------------------------------------------------------*/
	/*	Allocate list of pointers to patch objects .				*/
  /*--------------------------------------------------------------*/
  zone[0].patches = (struct patch_object ** ) 
		alloc( zone[0].num_patches * sizeof( struct patch_object *),
		"patches","construct_zone");
	/*--------------------------------------------------------------*/
	/*	Initialize any variables that should be initialized at	*/
	/*	the start of a simulation run for the zone.				*/
	/* for temperaturature we initialize to 999 so that they will be set on the first day based */
	/* on air temperatures on that day - which we don't know at this point */
	/*--------------------------------------------------------------*/
	zone[0].metv.pa	= atm_pres( zone[0].z );
	zone[0].metv.tsoil_sum = 0.0;
	zone[0].metv.tsoil = 0.0;
	zone[0].metv.tmin_ravg = 3.0;
	zone[0].metv.vpd_ravg = 900;
	zone[0].metv.dayl_ravg = 38000;

	/*--------------------------------------------------------------*/
	/*	Construct the intervals in this zone.						*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<zone[0].num_patches ; i++ ){
		zone[0].patches[i] = construct_patch(
			command_line,
			world_file,
			*num_world_base_stations,
			world_base_stations,
			defaults);
		zone[0].patches[i][0].zone = zone;
		sum_patch_area += zone[0].patches[i][0].area;
	} /*end for*/

	// check that zone area is equal to sum of patch areas
	if ( fabs(sum_patch_area -zone[0].area) > ZERO)
	{
		fprintf(stderr,"patch areas do not sum to zone area %lf for zone %d\n", sum_patch_area, zone[0].ID);
		exit(0);
	}

	/*--------------------------------------------------------------*/
	/*	Get number + ID of patch families for this zone				*/
	/*--------------------------------------------------------------*/

	if (command_line[0].multiscale_flag == 1) {

		// Vars
		int count;
		int freq[zone[0].num_patches];
		int patch_family_IDs[zone[0].num_patches];
		
		zone[0].num_patch_families = 0;

		// get number of patch families
		for (i = 0; i < zone[0].num_patches; i++) freq[i] = -1; // set freq to -1 for all patches

		for (i = 0; i < zone[0].num_patches; i++) // iterate through patches
		{
			count = 1; // count duplicates
			for (j = i + 1; j < zone[0].num_patches; j++) // iterate through all patches after patch i
			{
				if (zone[0].patches[i][0].family_ID == zone[0].patches[j][0].family_ID) // if theres a duplicate
				{
					count++; // incrament counter
					freq[j] = 0; // set freq of duplicate to 0 to not double count
				}
			}
			if (freq[i] != 0)	freq[i] = count; // if not a duplicate, set freq to the number of duplicates
		}
        
		for (i = 0; i < zone[0].num_patches; i++)
		{
			if (freq[i] > 0)
			{
				patch_family_IDs[zone[0].num_patch_families] = zone[0].patches[i][0].family_ID;
				zone[0].num_patch_families ++;
			}
		}

		if (command_line[0].verbose_flag == -6) printf("\n Constructing %d patch families for zone %d\n",zone[0].num_patch_families, zone[0].ID);
		
	/*--------------------------------------------------------------*/
	/*	Allocate pointers to patch family objects					*/
  	/*--------------------------------------------------------------*/
		zone[0].patch_families = (struct patch_family_object **) 
			alloc(zone[0].num_patch_families * sizeof(struct patch_family_object *), "patch_families", "construct_zone");

	/*--------------------------------------------------------------*/
	/*	Construct patch families									*/
  	/*--------------------------------------------------------------*/
		for (i = 0; i < zone[0].num_patch_families; i++)
		{
			if (command_line[0].verbose_flag == -6) printf("\nPatch fam ID %d, itr %d \n",patch_family_IDs[i], i);
			zone[0].patch_families[i] = construct_patch_family(
			zone,
			patch_family_IDs[i],
			command_line);
		}
	} /* end patch family for loop */

	if(paramPtr!=NULL)
	  free(paramPtr);
	
	return(zone);
} /*end construct_zone.c*/

