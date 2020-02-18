/*--------------------------------------------------------------*/
/*  construct_netcdf_grid.c - creates a zone object */
/*  SYNOPSIS   */
/*  struct base_station_object **construct_netcdf_grid(*/
/*    base_station_file_name,*/
/*    start_date,*/
/*    duration*/
/*    column_index);  */
/*  OPTIONS */
/*  DESCRIPTION */
/*  PROGRAMMER NOTES*/
/***
  Read netcdf format met data to construct a zone object
  The netcdf cdf has three dimention (day,lat,lon)
  The netcdf cdf has three dimention (day,lat,lon)
  day is the day since 1900-1-1
  
  By Mingliang Liu
  Nov. 18, 2011, WSU Pullman
***/



/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"



struct base_station_object *construct_netcdf_grid(
                struct base_station_ncheader_object *base_station_ncheader,
                int     *num_world_base_stations,
                float   zone_x,
                float   zone_y,
                float   zone_z,
                struct    date start_date,
                struct    date duration)
{
  /*--------------------------------------------------------------*/
  /*  Local function definition.                  */
  /*--------------------------------------------------------------*/


  void  *alloc(   size_t, char *, char *);
  
  struct  base_station_object *base_station;
  

  /* Allocate for the new base station cell */
  base_station = (struct base_station_object*) alloc(1 * sizeof(struct base_station_object),
                     "base_station", "construct_netcdf_grid_dummy");
  return(base_station);
}

