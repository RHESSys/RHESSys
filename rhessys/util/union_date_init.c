
/*-----------------------------------------------------------------------------
 *  union_date_init
 *
 *  NAME
 *  union_date_init - initiliaze the list for collecting date with hourly precipitation
 *                    using the data from the first base station
 *
 *  SYNOPSIS
 *
 *  OPTIONS
 *
 *  DESCRIPTION
 *
 *  Initiliaze the list of date for collecting date which has hourly precipitations.
 *  1. alloc mem to world[0].master_hourly_date
 *  2. find the total number of days which has hourly input for each base station
 *  3. use the master_hourly_date to record these days
 *  
 *  INPUT
 *  struct world_object * world,
 *  struct base_station ** base_station
 *
 *
 *  OUTPUT
 *  No output
 *-----------------------------------------------------------------------------*/

#include<stdio.h>
#include<stdlib.h>
#include "rhessys.h"

void union_date_init(struct world_object * world,
		      struct base_station_object ** base_station){
  /*-----------------------------------------------------------------------------
   *  Local function definition
   *-----------------------------------------------------------------------------*/  
   void *alloc(size_t,char *, char *);
   long julday(struct date);
   int	get_num_daywhourly(struct base_station_object * station);
  /*-----------------------------------------------------------------------------
   *  Local variable definition
   *-----------------------------------------------------------------------------*/
  int i,inx,num_d,base;
  int num_base_stations;
  struct base_station_object *clim_station;
  struct hourly_clim_object *hourly;
  struct date start_date;
  struct date end_date;
  struct date prev_date;
  /*-----------------------------------------------------------------------------
   *  Initiliaze the local variables
   *-----------------------------------------------------------------------------*/

  i=0;
  inx=0;
  num_d=0; //inx store the number of days which has hourly precipitation in this station
  start_date = world[0].start_date;
  end_date = world[0].end_date;


  /*-----------------------------------------------------------------------------
   *  1. alloc space for master_hourly_date
   *-----------------------------------------------------------------------------*/
    num_base_stations=world[0].num_base_stations;
    world[0].master_hourly_date = (struct date **)alloc( num_base_stations * sizeof(struct date *),
					"master_hourly_date","union_date_init");

  /*-----------------------------------------------------------------------------
   *  2. find number of days which has hourly precipitation in each base station
   *  3. use master_hourly_date to store these dates
   *-----------------------------------------------------------------------------*/

  for(base=0;base<num_base_stations;base++){
    clim_station=world[0].base_stations[base];
    num_d = get_num_daywhourly(clim_station);
    hourly = base_station[base]->hourly_clim;
    //num_d = 0;
    if(num_d==0){

	world[0].master_hourly_date[base] = (struct date *)alloc( 1 * sizeof(struct date),
					 "master_hourly_date_[i]","union_date_init");
	world[0].master_hourly_date[base][0].year = start_date.year;
	world[0].master_hourly_date[base][0].month = start_date.month;
	world[0].master_hourly_date[base][0].day = start_date.day;
    }
    else{
	world[0].master_hourly_date[base] = (struct date *)alloc( (num_d+1) * sizeof(struct date),
					 "master_hourly_date_[i]","union_date_init");
	for(i=0;i<num_d;i++){
	    world[0].master_hourly_date[base][i].year = hourly[0].rain.seq[i*24].edate.year;
	    world[0].master_hourly_date[base][i].month = hourly[0].rain.seq[i*24].edate.month;
	    world[0].master_hourly_date[base][i].day = hourly[0].rain.seq[i*24].edate.day;
	    /*printf("\n base =%d, i = %d, date =%d %d %d\n",
		  base,i, world[0].master_hourly_date[base][i].year,
		  world[0].master_hourly_date[0][i].month,
		  world[0].master_hourly_date[base][i].day);*/
	}
	world[0].master_hourly_date[base][num_d].year = 0;

    }
  
  //printf("\nyear = %d, month = %d, day = %d\n",year, month, day);
  //printf("\n num_day = %d\n",num_d);
  }
}
