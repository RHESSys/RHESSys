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
								   struct	world_object	*world,
								   int		num_world_base_stations,
								   struct	base_station_object **world_base_stations,
								   struct	zone_object 	*zone,
								   long day)
{
     // if (command_line[0].gridded_netcdf_flag==1 && zone[0].defaults[0][0].grid_interpolation ==1)

    int search_x =0;
    int search_y =0;
    // now started search the neigbour grids x <= search_x y<= search_y

    int count =0;
    int i, j;
    //struct base_station_object *station_search; // this is single station
    //struct base_station_object	**station_found;
    double distance[num_world_base_stations];;
    double diff_elevation[num_world_base_stations];
    double weight[num_world_base_stations];
    double sum_weight =0; //weighting factor for inverse distance method

    struct base_station_object *station_search; // this is single station
    //struct base_station_object	**station_found;
    //using array instead of station stations to save the found data
    double rain_found[num_world_base_stations];
    double tmax_found[num_world_base_stations];
    double tmin_found[num_world_base_stations];
    double ID_found[num_world_base_stations];


    double rain_temp =0;
    double tmax_temp =0;
    double tmin_temp =0;
    double rain_old = 0;
    double tmax_old = 0;
    double tmin_old = 0;
    double res_square = 0;

    double Tlapse_adjustment1 = 0;
    double Tlapse_adjustment2 = 0;
    double isohyet_adjustment = 0;
    int length;


    double max_tmax = 50;
    double min_tmin = -50; //TODO, put into the zone.def sfiles for WA ID from http://www.ncdc.noaa.gov/extremes/scec/records

    //station_found = (struct base_station_object **) alloc( (num_world_base_stations+100) *
    //    sizeof(struct base_station_object), "base_station", "construct_zone");




    zone[0].x_utm = zone[0].patches[0][0].x;
    zone[0].y_utm = zone[0].patches[0][0].y;
    zone[0].z_utm = zone[0].patches[0][0].z;
    if (command_line[0].verbose_flag == -3) {
        printf("\n Zone %d, x is %lf, y is %lf, z is  %lf \n", zone[0].ID, zone[0].x_utm, zone[0].y_utm, zone[0].z_utm);
        printf("\n the original climate data tmax %lf, tmin %lf, rain %lf \n", zone[0].base_stations[0][0].daily_clim[0].tmax[day],
                        zone[0].base_stations[0][0].daily_clim[0].tmin[day],
                        zone[0].base_stations[0][0].daily_clim[0].rain[day]);
    }

    //using another method to read the record of patch information
    // using the UTM system to search closing grid center normally seach 8 neribours and using inverse distance method
    // basestation_x_proj and y_proj is the utm coordinate system center for that basestation, all information is in world_base_stations[0][0]
    // *num_world_base_stations
    // first calcuate the resolution of the grid for both x direction and y direction

    if (num_world_base_stations <=1) {
        printf("\n WARNING only one or no basestation, no need to interpolation \n");
        search_x=0;
        search_y=0;
    }
    else {
        search_x = zone[0].defaults[0][0].search_x - zone[0].defaults[0][0].res_patch;
        search_y = zone[0].defaults[0][0].search_y - zone[0].defaults[0][0].res_patch;

    }


    for (i=0; i< num_world_base_stations; i++) {

    station_search = world_base_stations[i];

    if ( abs(station_search[0].proj_x -zone[0].x_utm) <= search_x && abs(station_search[0].proj_y -zone[0].y_utm) <= search_y)
        {
            //find =1;

            rain_found[count] = station_search[0].daily_clim[0].rain[day];
            tmax_found[count] = station_search[0].daily_clim[0].tmax[day];
            tmin_found[count] = station_search[0].daily_clim[0].tmin[day];
            ID_found[count] = station_search[0].ID;

            // due to the inverse distance method going to use square of distance so here no need to do square root
            res_square = zone[0].defaults[0][0].res_patch * zone[0].defaults[0][0].res_patch;
            distance[count]= ((zone[0].x_utm - station_search[0].proj_x) * (zone[0].x_utm - station_search[0].proj_x)/res_square + (zone[0].y_utm - station_search[0].proj_y) * (zone[0].y_utm - station_search[0].proj_y)/res_square);
            if (distance[count] > 0) {
            weight[count] = 1/distance[count];
            }
            diff_elevation[count] = zone[0].z_utm - station_search[0].z;

            if (command_line[0].verbose_flag == -3) {
            printf("\n there are %d neibourge stations, ID is %d, distance is %lf, weight is %lf\n", count, ID_found[count], distance[count], weight[count]);

            }
            count++;

        }

    }

    // after find these stations, caluate the sum of weight and final weight for each stations

    if (count > 0) {
        sum_weight = 0;

        for (j=0; j <  count; j++) {
            if (distance[j] > 2.0 ) { // when calculate distance I using res to normalize the distance
            sum_weight = sum_weight + weight[j]; //simple inverse distance method, not considering the direction and slope effect
                }
        }

        // if one patch is very close the centre of basestation

        for (j=0; j < count; j++) {
            if (distance[j] <= 2.0 ) {
            sum_weight = 0; //simple inverse distance method, not considering the direction and slope effect
                }
        }

        // now interpolation climate data only interploate the precipitation, m


        // TODO put this as an equation, and using openmp

       if (sum_weight > 1e-6) {
           // printf("\n started interpolation climate data \n");
          // printf("\n the ratio for zone ID %d is %lf \n", zone[0].ID, weight[1]/sum_weight);

                    rain_temp=0;
                    tmax_temp=0;
                    tmin_temp=0;
                    tmax_old = 0;
                    tmin_old = 0;

                    for (j =0; j< count; j++) {
                    //precip, check crazy values too.
                    //isohyet_adjustment = zone[0].defaults[0][0].lapse_rate_precip_default * diff_elevation[j] +1.0;
                   // isohyet_adjustment = max(0, isohyet_adjustment)


                    rain_temp = rain_temp + rain_found[j] * weight[j]/sum_weight;

                    if (command_line[0].verbose_flag == -3) {
                    printf("\n the ratio for station %d is %lf \n", j, weight[j]/sum_weight);
                    }
                    //Tmax
                    if (rain_temp > ZERO)
                        Tlapse_adjustment1 = diff_elevation[j] * zone[0].defaults[0][0].wet_lapse_rate;
                    else
                        Tlapse_adjustment1 = diff_elevation[j]*zone[0].defaults[0][0].lapse_rate_tmax; // adjust the temperature based on elevation
                    tmax_temp = tmax_temp + tmax_found[j] * weight[j]/sum_weight;

                    //Tmin
                    if (rain_temp > ZERO)
                        Tlapse_adjustment2 = diff_elevation[j] * zone[0].defaults[0][0].wet_lapse_rate;
                    else
                        Tlapse_adjustment2 = diff_elevation[j] * zone[0].defaults[0][0].lapse_rate_tmin;    // adjst the min temperature based on elevation
                    tmin_temp = tmin_temp + tmin_found[j] * weight[j]/sum_weight;

                    } // end for count

                    //rain
                    if (rain_temp <0.0)
                    {
                            printf("\n WARNING, the interpolated rain %lf is smaller than 0 for day :%d \n", rain_temp, day);
                    }

                    if (command_line[0].verbose_flag == -3) {
                    printf("\n Day: %d rain differences between interpolated value %lf and original value %lf is %lf \n", day, rain_temp, zone[0].base_stations[0][0].daily_clim[0].rain[day],  (rain_temp -zone[0].base_stations[0][0].daily_clim[0].rain[day]));
                    }
                    //assign value
                    zone[0].rain_interpolate = rain_temp;
                    //tmax
                    if (tmax_temp > max_tmax || tmax_temp < min_tmin)
                    {
                            printf("\n WARNING, the interpolated tmax %lf is out of range (-50, 50) for day :%d, lapse adjustment is %lf, elevation difference is %lf, \n", tmax_temp, day, Tlapse_adjustment1, diff_elevation[0] );

                    }

                    if (command_line[0].verbose_flag == -3) {
                    printf("\n Day: %d tmax differences between original value %lf and interpolated value %f is %lf \n", day, zone[0].base_stations[0][0].daily_clim[0].tmax[day], tmax_temp, (tmax_temp -zone[0].base_stations[0][0].daily_clim[0].tmax[day]));
                    }
                    //assign value
                    tmax_old = zone[0].base_stations[0][0].daily_clim[0].tmax[day];
                    zone[0].tmax_interpolate = tmax_temp;
                    //tmin
                     if (tmin_temp > max_tmax || tmin_temp < min_tmin)
                    {
                            printf("\n WARNING, the interpolated tmin %lf is out of range (-50, 50) for day :%d, lapse adjustment is %lf, elevation difference is %lf, \n", tmin_temp, day, Tlapse_adjustment2, diff_elevation[0] );
                    }
                    if (command_line[0].verbose_flag == -3) {
                    printf("\n Day: %d tmin differences between interpolated value and original value is %lf \n", day, (tmin_temp -zone[0].base_stations[0][0].daily_clim[0].tmin[day]));
                    }
                    //assign value
                    tmin_old = zone[0].base_stations[0][0].daily_clim[0].tmin[day];
                    zone[0].tmin_interpolate = tmin_temp;

                    if (tmax_temp < tmin_temp || tmax_old < tmin_old) {
                    printf("\n WARNING: tmax is smaller than tmin after interpolation, tmax_inter %lf, tmin_inter is %lf, tmax_old %lf, tmin_old %lf, ID %d, num_neiboughors %d for day %d",
                    tmax_temp, tmin_temp, tmax_old, tmin_old, zone[0].ID, count, day);

                    }


        } //end sum_weight>0
        else {
                zone[0].rain_interpolate = zone[0].base_stations[0][0].daily_clim[0].rain[day];
                zone[0].tmax_interpolate = zone[0].base_stations[0][0].daily_clim[0].tmax[day];
                zone[0].tmin_interpolate = zone[0].base_stations[0][0].daily_clim[0].tmin[day];

                if (day == 1)
            printf("\n WARNING: patch %d, is close the climate station, no need to interpoaltion \n", zone[0].ID);
        } //end else
    }//end if count>1

    else {
                zone[0].rain_interpolate = zone[0].base_stations[0][0].daily_clim[0].rain[day];
                zone[0].tmax_interpolate = zone[0].base_stations[0][0].daily_clim[0].tmax[day];
                zone[0].tmin_interpolate = zone[0].base_stations[0][0].daily_clim[0].tmin[day];
        if (day==1)
        printf("\n WARNING: patch %d no neigbour station found, using the climate grid data where the patch is located \n", zone[0].ID);
    }

    //free the memory
    // no need to free station found still point to the same memory



	 //return(base_station);
	 return;

}
