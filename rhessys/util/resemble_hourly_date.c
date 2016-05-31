/*--------------------------------------------------------------*/
/*					resemble_hourly_date		*/
/*								*/
/* resemble_hourly_date.c  initiliaze the date record from hourly precipitation data */
/*								*/
/* NAME								*/
/* SYNOPSIS							*/
/*								*/
/* OPTIONS							*/
/*								*/
/* DESCRIPTION							*/
/*								*/
/*								*/ 
/*Resemble the date from hourly precipitation data so that once there is      */
/*an hourly record in any of the stations, all station will have hourly	      */
/*precipitation data on that day. This is for the purpose of running hourly/daily*/
/*simulation on the same day.						      */


/* 1. List the days which has hourly record for any base station	      */
/* 1.1  initiliaze the date record for hourly precipitation		      */
/* 1.2 create the union date for all stations
 * 
 * 2.  resemble the hourly precipitation data in all base stations
 *    case 2.1 if there is hourly records on that day, keep it
 *    case 2.2 if there is no hourly record on that day, use the daily records
 *		divided by 24, create the hourly records for that day, add it
 *		to the hourly records sequence.
 *
 * INPUT:
 * world_object *world
 *
 * OUTPUT:
 * NO OUPUT. 
 * */
#include<stdlib.h>
#include<stdio.h>
#include "rhessys.h"
#include <errno.h>

void resemble_hourly_date(struct world_object *world){
	

  /*-----------------------------------------------------------------------------
   *  Local function definition
   *-----------------------------------------------------------------------------*/
  void union_date_init(struct world_object *,
			struct base_station_object **);

  struct date * union_date_combine(struct world_object *,
				  struct date **);

  void reconstruct_hourly_clim(struct world_object *, 
				struct base_station_object **,
				struct date *,
				struct date ,
				int );



  
  /*-----------------------------------------------------------------------------
   *  Local variable definition
   *-----------------------------------------------------------------------------*/
  int i;
  int inx;
  struct hourly_clim_object *hourly_clim;

  /*-----------------------------------------------------------------------------
   *  If there is only one base station, there is no need to resemble the records
   *-----------------------------------------------------------------------------*/
  if(world[0].num_base_stations==1){
    return;
  }
  else{
  /*-----------------------------------------------------------------------------
   *  1.  List the days which have hourly records for any base station
   *-----------------------------------------------------------------------------*/
  union_date_init(world, world[0].base_stations);
  /*-----------------------------------------------------------------------------
   * 1.2  calculate the union date from other stations 
   *-----------------------------------------------------------------------------*/
  struct date *union_date;
  union_date=union_date_combine(world,
				world[0].master_hourly_date);	
    i=0;

  /*-----------------------------------------------------------------------------
   *  2. reconstruct hourly precipitation data for each base station for union_date
   *  for each day in the union_date:
   *  If there is hourly precipitation record in the ori base station, use it
   *  Else if there is none, use the daily precipitation record and divided by 24
   *-----------------------------------------------------------------------------*/
  reconstruct_hourly_clim(world,
			  world[0].base_stations,
			  union_date,
			  world[0].start_date,
			  world[0].duration.day);
  
  inx=0;
  /*hourly_clim=world[0].base_stations[1][0].hourly_clim;
  while(hourly_clim[0].rain.seq[inx].edate.year!=0){
    printf("year = %d, month=%d, day=%d,hour=%d,value=%f\n",
	    hourly_clim[0].rain.seq[inx].edate.year,
	    hourly_clim[0].rain.seq[inx].edate.month,
	    hourly_clim[0].rain.seq[inx].edate.day,
	    hourly_clim[0].rain.seq[inx].edate.hour,
	    hourly_clim[0].rain.seq[inx].value);
    inx++;
  }*/
  
}

  
}
