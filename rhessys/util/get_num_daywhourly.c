
/*-----------------------------------------------------------------------------
 *  get_num_daywhourly
 *
 *  NAME
 *  get_num_daywhourly
 *
 *  DESCRIPTION
 *  This function is to get the number of days which has hourly precipitation data
 *  from base station
 *
 *  INPUT
 *
 *  struct hourly_clim_object * hourly
 *
 *
 *  OUTPUT 
 *  int num_b
 *-----------------------------------------------------------------------------*/
#include<stdio.h>
#include<stdlib.h>
#include "rhessys.h"
int get_num_daywhourly(struct base_station_object * station){
 //int get_num_daywhourly(struct hourly_clim_object * hourly){

   /*-----------------------------------------------------------------------------
    *  Local function definition
    *-----------------------------------------------------------------------------*/
   long julday(struct date);
   /*-----------------------------------------------------------------------------
    *  Local variable definition
    *-----------------------------------------------------------------------------*/
    int num_b,i;
    struct date prev_date;
    struct hourly_clim_object * hourly;
  /*-----------------------------------------------------------------------------
   * Initialize the variables  
   *-----------------------------------------------------------------------------*/
    num_b=0;
    hourly = station[0].hourly_clim;

  /*-----------------------------------------------------------------------------
   * Get the number of day with hourly precipitation data  
   *-----------------------------------------------------------------------------*/
  if(hourly[0].rain.inx == -999){
    num_b = 0;
  }
  else if(hourly[0].rain.seq[0].edate.year==0){
    num_b=0;
  }
  else{
    i=0;
    num_b=1;
    prev_date = hourly[0].rain.seq[0].edate;
    while(hourly[0].rain.seq[i].edate.year!=0){
      if(julday(prev_date)<julday(hourly[0].rain.seq[i].edate)){
	num_b=num_b+1;
	prev_date = hourly[0].rain.seq[i].edate;
      }
      i=i+1;
    }
  }
  
  return(num_b);

}


