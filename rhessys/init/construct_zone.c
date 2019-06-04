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
#include "UTM.h"  //N.R 2019/05/31

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
		int     num_world_extra_base_stations,
		struct base_station_object  **extra_base_stations,  //NREN 20180711
		struct	default_object	*defaults);

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

    // convert lat lon to UTM, input are lat lon zone,&x &y, update x, y, and return zone
	int LatLonToUTMXY(float, float, int, float *, float *); //LatlonToUTM NR 20190531

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
	char	record[MAXSTR];
	struct	zone_object *zone;
	int paramCnt=0;
	param *paramPtr=NULL;

	notfound = 0;
	base_x = 0.0;
	base_y = 0.0;
	j = 0;
	k = 0;


	/*--------------------------------------------------------------*/
	/*	Allocate a zone object.								*/
	/*--------------------------------------------------------------*/
	zone = (struct zone_object *) alloc( 1 *
		sizeof( struct zone_object ),"zone","construct_zone" );
	/*--------------------------------------------------------------*/
	/*	Read in the next zone record for this hillslope.			*/
	/*--------------------------------------------------------------*/

	paramPtr=readtag_worldfile(&paramCnt,world_file,"Zone"); //so paramCnt store all the worldfile information

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
	zone[0].num_base_stations = getIntWorldfile(&paramCnt,&paramPtr,"n_basestations","%d",0,0);
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
	if (command_line[0].gridded_netcdf_flag==1 && world[0].num_extra_stations==1) // add condition to read the extra beetle attack time series 20180711
	{
	for ( i=0 ; i<zone[0].num_patches ; i++ ){
		zone[0].patches[i] = construct_patch(
			command_line,
			world_file,
			*num_world_base_stations,
			world_base_stations,
			world[0].num_extra_stations,
			(world[0].extra_stations),
			defaults);
		zone[0].patches[i][0].zone = zone;
            }
	}

	else
	{
	for ( i=0 ; i<zone[0].num_patches ; i++ ){
		zone[0].patches[i] = construct_patch(
			command_line,
			world_file,
			*num_world_base_stations,
			world_base_stations,
			0,
			world_base_stations,
			defaults);
		zone[0].patches[i][0].zone = zone;
	} /*end for*/

	} // end else

    /*--------------------------------------------------------------*/
	/* interpolate climate data by N.R 2019/05/30                   */
	/* only conduct interpolation when it netcdf and zone def.grid_ */
	/* interpolation is true                                        */
	/*--------------------------------------------------------------*/
    // first readin the zone new utm coordiate system
    if (command_line[0].gridded_netcdf_flag==1 && zone[0].defaults[0][0].grid_interpolation ==1)
    {

    zone[0].x_utm = zone[0].patches[0][0].x;
    zone[0].y_utm = zone[0].patches[0][0].y;
    zone[0].z_utm = zone[0].patches[0][0].z;

    //using another method to read the record of patch information
    // using the UTM system to search closing grid center normally seach 8 neribours and using inverse distance method
    // basestation_x_proj and y_proj is the utm coordinate system center for that basestation, all information is in world_base_stations[0][0]
    // *num_world_base_stations
    // first calcuate the resolution of the grid for both x direction and y direction
    int search_x =0;
    int search_y =0;

    if (*num_world_base_stations <=1) {
        printf("\n WARNING only one or no basestation, no need to interpolation \n");
        search_x=0;
        search_y=0;
    }
    else {
        search_x = zone[0].defaults[0][0].search_x - zone[0].defaults[0][0].res_patch;
        search_y = zone[0].defaults[0][0].search_y - zone[0].defaults[0][0].res_patch;

    }

    // now started search the neigbour grids x <= search_x y<= search_y

    int count =0;
    struct base_station_object *station_search; // this is single station
    struct base_station_object	**station_found;
    double distance[*num_world_base_stations];;
    double diff_elevation[*num_world_base_stations];
    double weight[*num_world_base_stations];
    double sum_weight =0; //weighting factor for inverse distance method

    double rain_temp =0;
    double tmax_temp =0;
    double tmin_temp =0;
    double rain_old = 0;
    double tmax_old = 0;
    double tmin_old = 0;


    double Tlapse_adjustment1 = 0;
    double Tlapse_adjustment2 = 0;
    int length;
    int day;

    double max_tmax = 50;
    double min_tmin = -50; //TODO, put into the zone.def sfiles for WA ID from http://www.ncdc.noaa.gov/extremes/scec/records

    station_found = (struct base_station_object **) alloc( *num_world_base_stations *
        sizeof(struct base_station_object), "base_station", "construct_zone");

    for (i=0; i< *num_world_base_stations; i++) {

    station_search = world_base_stations[i];

    if ( abs(station_search[0].proj_x -zone[0].x_utm) <= search_x && abs(station_search[0].proj_y -zone[0].y_utm) <= search_y)
        {
            //find =1;
              count++;
            station_found[count] = station_search;
            // due to the inverse distance method going to use square of distance so here no need to do square root
            distance[count]= ((zone[0].x_utm - station_search[0].proj_x) * (zone[0].x_utm - station_search[0].proj_x) + (zone[0].y_utm - station_search[0].proj_y) * (zone[0].y_utm - station_search[0].proj_y))/zone[0].defaults[0][0].res_patch/zone[0].defaults[0][0].res_patch;
            if (distance[count] > 0) {
            weight[count] = 1/distance[count];
            }
            diff_elevation[count] = zone[0].z_utm - station_search[0].z;

            if (command_line[0].verbose_flag == -3) {
            printf("\n there are %d neibourge stations, ID is %d \n", count, station_found[count][0].ID);
            }

        }

    }

    // after find these stations, caluate the sum of weight and final weight for each stations

    if (count >1) {
    sum_weight =0;
        for (j=1; j <= count; j++) {

            if (distance[count] > zone[0].defaults[0][0].res_patch ) {


            sum_weight = sum_weight + weight[j]; //simple inverse distance method, not considering the direction and slope effect

                }
        }

        // if one patch is very close the centre of basestation

        for (j=1; j <= count; j++) {

            if (distance[count] <= zone[0].defaults[0][0].res_patch ) {


            sum_weight = 0; //simple inverse distance method, not considering the direction and slope effect

                }
        }

        // now interpolation climate data only interploate the precipitation, m

        length = world[0].duration.day;
        // TODO put this as an equation, and using openmp

       if (sum_weight>0) {
           // printf("\n started interpolation climate data \n");
            for (day = 0; day<length; day++)
                {
                    rain_temp=0;
                    tmax_temp=0;
                    tmin_temp=0;

                    for (j =1; j<=count; j++) {
                    //precip, check crazy values too.
                    rain_temp = rain_temp + station_found[count][0].daily_clim[0].rain[day] * weight[j]/sum_weight;

                    if (command_line[0].verbose_flag == -3) {
                    printf("\n the ratio for station %d is %lf \n", j, weight[j]/sum_weight);
                    }
                    //Tmax
                    Tlapse_adjustment1 = diff_elevation[count]*zone[0].defaults[0][0].lapse_rate_tmax; // adjust the temperature based on elevation
                    tmax_temp = tmax_temp + (station_found[count][0].daily_clim[0].tmax[day]) * weight[j]/sum_weight;

                    //Tmin
                    Tlapse_adjustment2 = diff_elevation[count]*zone[0].defaults[0][0].lapse_rate_tmin; // adjst the min temperature based on elevation
                    tmin_temp = tmin_temp + (station_found[count][0].daily_clim[0].tmin[day]) * weight[j]/sum_weight;

                    } // end for count

                    //rain
                    if (rain_temp <0.0)
                    {
                            printf("\n WARNING, the interpolated rain %lf is smaller than 0 for day :%d \n", rain_temp, day);
                    }

                    if (command_line[0].verbose_flag == -3) {
                    printf("\n rain differences between interpolated value and original value is %lf \n", (rain_temp -zone[0].base_stations[0][0].daily_clim[0].rain[day]));
                    }
                    //assign value
                    zone[0].base_stations[0][0].daily_clim[0].rain[day] = rain_temp;
                    //tmax
                    if (tmax_temp > max_tmax || tmax_temp < min_tmin)
                    {
                            printf("\n WARNING, the interpolated tmax %lf is out of range (-50, 50) for day :%d, lapse adjustment is %lf, elevation difference is %lf, \n", tmax_temp, day, Tlapse_adjustment1, diff_elevation[0] );

                    }

                    if (command_line[0].verbose_flag == -3) {
                    printf("\n tmax differences between interpolated value and original value is %lf \n", (tmax_temp -zone[0].base_stations[0][0].daily_clim[0].tmax[day]));
                    }
                    //assign value
                    tmax_old = zone[0].base_stations[0][0].daily_clim[0].tmax[day];
                    zone[0].base_stations[0][0].daily_clim[0].tmax[day] = tmax_temp;
                    //tmin
                     if (tmin_temp > max_tmax || tmin_temp < min_tmin)
                    {
                            printf("\n WARNING, the interpolated tmin %lf is out of range (-50, 50) for day :%d, lapse adjustment is %lf, elevation difference is %lf, \n", tmin_temp, day, Tlapse_adjustment2, diff_elevation[0] );
                    }
                    if (command_line[0].verbose_flag == -3) {
                    printf("\n tmin differences between interpolated value and original value is %lf \n", (tmin_temp -zone[0].base_stations[0][0].daily_clim[0].tmin[day]));
                    }
                    //assign value
                    tmin_old = zone[0].base_stations[0][0].daily_clim[0].tmin[day];
                    zone[0].base_stations[0][0].daily_clim[0].tmin[day] = tmin_temp;

                    if (tmax_temp < tmin_temp) {
                    printf("\n WARNING: tmax is smaller than tmin after interpolation, tmax_inter %lf, tmin_inter is %lf, tmax_old %lf, tmin_old %lf, ID %d, num_neiboughors %d",
                    tmax_temp, tmin_temp, tmax_old, tmin_old, zone[0].ID, count);

                    }

                }
        }
    }
    else {
        printf("\n WARNING: no neigbour station found, using the climate grid data where the patch is located \n");
    }

    }// end of if do interpolation
	if(paramPtr!=NULL)
	  free(paramPtr);




	return(zone);
} /*end construct_zone.c*/

