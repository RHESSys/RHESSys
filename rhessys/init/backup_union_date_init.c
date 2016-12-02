
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
 *  1. find the total number of days which has hourly input
 *  2. malloc mem to world[0].master_hourly_date
 *  3. use the master_hourly_date to record these days
 *  
 *  INPUT
 *  struct base_station * 
 *	    world[0].base_station[0]
 *
 *
 *  OUTPUT
 *  No output
 *-----------------------------------------------------------------------------*/

#include<stdio.h>
#include<stdlib.h>
#include "rhessys.h"

void union_date_init(struct world_object * world,
		      struct base_station_object * base_station){
  /*-----------------------------------------------------------------------------
   *  Local function definition
   *-----------------------------------------------------------------------------*/  
   void *alloc(size_t,char *, char *);
   long julday(struct date);
  /*-----------------------------------------------------------------------------
   *  Local variable definition
   *-----------------------------------------------------------------------------*/
  int i,inx,num_d;
  int year, month, day;
  int num_base_stations;
  int start_inx;
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
  year=0;
  month=0;
  day=0;
  hourly = base_station[0].hourly_clim;
  start_date = world[0].start_date;
  end_date = world[0].end_date;
  start_inx =0;
  /*-----------------------------------------------------------------------------
   *  1. Total number of days which has hourly record (even it is 0)
   *-----------------------------------------------------------------------------*/

   /* if there no hourly data in that base station, use the first day in the daily data*/
  if(hourly[0].rain.inx ==-999){ // no hourly precipitation record
    num_d = 0;
  }else if(julday(end_date)<julday(hourly[0].rain.seq[0].edate)){  
  /*during this period, there is no hourly record*/
    num_d = 0;
  }else if(julday(start_date)<=julday(hourly[0].rain.seq[0].edate)){ 
     /*the date of first hourly record is earlier than the end date*/
    /* need to find out whether is any hourly record between start_date and end_date */
    prev_date = hourly[0].rain.seq[0].edate;
    start_inx = 0;
    num_d = 1;
    i=1;
    while(hourly[0].rain.seq[i].edate.year !=0 && (julday(hourly[0].rain.seq[i].edate)<=julday(end_date))){
	if (julday(prev_date)<julday(hourly[0].rain.seq[i].edate)){
	  // it is a new day
	  num_d = num_d + 1;
	  prev_date=hourly[0].rain.seq[i].edate;
	}
	  i = i+1;
    }
      
  }
  else{  
  /* the date of the first hourly record is ealier than the start date */
    i=1;
    prev_date=hourly[0].rain.seq[0].edate;
    while(hourly[0].rain.seq[i].edate.year!=0 && (julday(hourly[0].rain.seq[i].edate)<julday(start_date))){
	prev_date=hourly[0].rain.seq[i].edate;
	i=i+1;
    }
    if (hourly[0].rain.seq[i].edate.year ==0){
      /* the date of the latest hourly date is earlier than the start_date */
      num_d=0;
    }
    else{
      num_d=0;
      
      while(hourly[0].rain.seq[i].edate.year!=0 && (julday(hourly[0].rain.seq[i].edate)<=julday(end_date))){
	if (julday(prev_date)<julday(hourly[0].rain.seq[i].edate)){
	  /* if start a new day */
	  num_d = num_d + 1;
	  if(num_d==1) start_inx=i;
	  prev_date=hourly[0].rain.seq[i].edate;
	}
	i=i+1;
      }
    }

  }



  /*-----------------------------------------------------------------------------
   *  2. Create space for the num_d date 
   *  3. use master_hourly_date to store these dates
   *-----------------------------------------------------------------------------*/
    num_base_stations=world[0].num_base_stations;
    world[0].master_hourly_date = (struct date **)alloc( 1 * sizeof(struct date *),
					"master_hourly_date","union_date_init");
    
 
    if(num_d==0){

	world[0].master_hourly_date[0] = (struct date *)alloc( 1 * sizeof(struct date),
					 "master_hourly_date_[i]","union_date_init");
	world[0].master_hourly_date[0][0].year = start_date.year;
	world[0].master_hourly_date[0][0].month = start_date.month;
	world[0].master_hourly_date[0][0].day = start_date.day;
    }
    else{
	world[0].master_hourly_date[0] = (struct date *)alloc( num_d * sizeof(struct date),
					 "master_hourly_date_[i]","union_date_init");
	for(i=0;i<num_d;i++){
	    world[0].master_hourly_date[0][i].year = hourly[0].rain.seq[i*24+start_inx].edate.year;
	    world[0].master_hourly_date[0][i].month = hourly[0].rain.seq[i*24+start_inx].edate.month;
	    world[0].master_hourly_date[0][i].day = hourly[0].rain.seq[i*24+start_inx].edate.day;

	    printf("\n i = %d, date =%d %d %d\n",i, world[0].master_hourly_date[0][i].year,
	    world[0].master_hourly_date[0][i].month,world[0].master_hourly_date[0][i].day);
	}

    }
  
  //printf("\nyear = %d, month = %d, day = %d\n",year, month, day);
  printf("\n num_day = %d,start_inx=%d\n",num_d,start_inx);

}
