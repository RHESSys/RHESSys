/*--------------------------------------------------------------------*/
/*                                                                    */
/*            climate_grid_interpolation                              */
/*                                                                    */
/*to interpolate the grid climate data using inverse distance method  */
/*references paper Donald Shepard 1968 .ACM National Conference       */
/* SYMAP interpolation Algorthim Shepard, D.S., 1984, Spatial         */
/* Statistics and Models. reference code source also from VIC websit  */
/* https://vic.readthedocs.io/en/vic.4.2.d/Documentation/GridNCDC/    */
/* Adapted by N.R. 2019.06.01                                         */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"
#include "params.h"
#include "UTM.h"  //N.R 2019/05/31

void climate_interpolation(
								   struct	command_line_object	*command_line,
								   int		*num_world_base_stations,
								   struct	base_station_object **world_base_stations,
								   struct	zone_object 	*zone_temp)
{
    double search_x =0;
    double search_y =0;
    int count =0;
    int j;
    int i;
    struct base_station_object *station_search; // this is single station
    struct base_station_object	**station_found;
    struct base_station_object *base_station;
    double distance[*num_world_base_stations];
    double diff_elevation[*num_world_base_stations];
    double weight[*num_world_base_stations];
    double sum_weight =0; //weighting factor for inverse distance method

    double rain_temp =0;
    double tmax_temp =0;
    double tmin_temp =0;
    double search_x_default;
    double search_y_default;
    double res_patch;



    station_found = (struct base_station_object **) alloc( *num_world_base_stations *
        sizeof(struct base_station_object), "base_station", "construct_zone");
    station_search = (struct base_station_object *) alloc(1*
        sizeof(struct base_station_object), "base_station", "construct_basestation");
    base_station = (struct base_station_object *) alloc(1*
        sizeof(struct base_station_object), "base_station", "construct_basestation");

    /*--------------------------------------------------------------*/
	/* interpolate climate data by N.R 2019/05/30                   */
	/*--------------------------------------------------------------*/
    // first readin the zone new utm coordiate system from the patch
    // here we assume in the worldfile, one zoneID corresponding to one patch ID

    zone_temp[0].x_utm = zone_temp[0].patches[0][0].x;
    zone_temp[0].y_utm = zone_temp[0].patches[0][0].y;
    zone_temp[0].z_utm = zone_temp[0].patches[0][0].z;


    // using the UTM system to search closing grid center normally seach 8 neribours and using inverse distance method
    // basestation_x_proj and y_proj is the utm coordinate system center for that basestation, all information is in world_base_stations[0][0]
    // *num_world_base_stations
    // first calcuate the resolution of the grid for both x direction and y direction


    if (*num_world_base_stations <=1) {
        printf("\n WARNING only one or no basestation, no need to interpolation \n");
        search_x=0;
        search_y=0;
    }
    else {

        //search_x_default = zone_temp[0].defaults[0][0].search_x;
       // search_y_default = zone_temp[0].defaults[0][0].search_y;
        //res_patch = zone_temp[0].defaults[0][0].res_patch;
        search_x_default = 3376;
        search_y_default = 4838;
        res_patch = 100;

        search_x =  search_x_default - res_patch;
        search_y =  search_y_default - res_patch;
        //search_x = zone_temp->defaults->search_x - zone_temp->defaults->res_patch;
        //search_y = zone_temp->defaults->search_y - zone_temp->defaults->res_patch;
    }

    // now started search the neigbour grids x <= search_x y<= search_y


    for (i=0; i< *num_world_base_stations; i++) {

    station_search = world_base_stations[i];
    if ( abs(station_search[0].proj_x -zone_temp[0].x_utm) <= search_x && abs(station_search[0].proj_y -zone_temp[0].y_utm) <= search_y)
        {
            //find =1;
            count = count +1;
            station_found[count] = station_search;
            // due to the inverse distance method going to use square of distance so here no need to do square root
            distance[count] =  (zone_temp[0].x_utm - station_search[0].proj_x) *  (zone_temp[0].x_utm - station_search[0].proj_x) + (zone_temp[0].y_utm - station_search[0].proj_y) * (zone_temp[0].y_utm - station_search[0].proj_y);
            diff_elevation[count] = zone_temp[0].z_utm - station_search[0].z;
            printf("\n there are %d neibourge stations, ID is %d \n", count, station_found[count][0].ID);
        }

    }

    // after find these stations, caluate the sum of weight and final weight for each stations

    if (count >1) {
        for (j=0; j < count; j++) {

            if (distance[count] <= 0 ) {
                zone_temp[0].base_stations[0] = station_found[count];
                sum_weight =0;
            }
            else {
            weight[count] = 1/distance[count];
            sum_weight = sum_weight + weight[count]; //simple inverse distance method, not considering the direction and slope effect

                }
        }

        // now interpolation climate data only interploate the precipitation, m

  /*      if (sum_weight>0) {
            for (j =0; j<=count; j++) {
                //precip
                rain_temp = rain_temp + station_found[count].daily_clim[0].rain * weight[count]/sum_weight;
                //Tmax
                Tlapse_adjustment = diff_elevation[count]*zone[0].defaults[0][0].lapse_rate_tmax; // adjust the temperature based on elevation
                tmax_temp = tmax_temp + (station_found[count].daily_clim[0].tmax - Tlapse_adjustment) * weight[count]/sum_weight;
                //Tmin
                Tlapse_adjustment = diff_elevation[count]*zone[0].defaults[0][0].lapse_rate_tmin; // adjst the min temperature based on elevation
                tmin_temp = tmin_temp + (station_found[count].daily_clim[0].tmax - Tlapse_adjustment) * weight[count]/sum_weight;

            }
        } */
    }
    else {
        printf("\n WARNING: no neigbour station found, using the climate grid data where the patch is located \n");
    }


    if(station_found !=NULL)
	 free(station_found);
    if(station_search !=NULL)
	 free(station_search);

	 //base_station = zone_temp[0].base_stations[0];

	 //return(base_station);
	 return;

}
