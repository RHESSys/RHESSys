
/*-----------------------------------------------------------------------------
 *  reconstruct_hourly_clim
 *
 *  NAME
 *  reconstruct_hourly_clim
 *
 *  INPUT
 *  struct world_object *
 *  struct base_station_object **
 *
 *  OUTPUT
 *  None
 *
 *  DESCRIPTION
 *  Reconstruct all hourly climate sequnce in each of the base station, based on 
 *  the date seq union_date
 *  For each day in the union_date:
 *  If there is hourly precipitation record in the ori base station, use it
 *  Else if there is none, us eht edaily precipitation record AND divided by 24
 *-----------------------------------------------------------------------------*/


#include<stdio.h>
#include<stdlib.h>
#include "rhessys.h"

void reconstruct_hourly_clim(struct world_object * world,
			      struct base_station_object **base_stations,
			      struct date *union_date,
			      struct date start_date,
			      int duration){


  /*-----------------------------------------------------------------------------
   *   Local function definition
   *-----------------------------------------------------------------------------*/
  int get_num_daywhourly(struct base_station_object *);
  void *alloc(size_t, char *, char *);
  /*-----------------------------------------------------------------------------
   *  Local variable definition
   *-----------------------------------------------------------------------------*/
  int i,num_d,d,j;
  int dh;
  int num_di;
  int tmp;
  int dd;
  int inx; // flag, indicate whether the precipitation data of certain day has been found
  int num_base_stations;
  struct hourly_clim_object *hourly_clim;
  struct daily_clim_object *daily_clim;
  struct dated_sequence * seq;
  /*-----------------------------------------------------------------------------
   *  Initialize the local variables
   *-----------------------------------------------------------------------------*/
  num_d=0;
  i=0;
  j=0;
  num_di=0;
  d=0;
  dh=0;
  dd=0;
  inx=0;
  num_base_stations=world[0].num_base_stations;
  /*-----------------------------------------------------------------------------
   *  Get the num of days in date_seq
   *-----------------------------------------------------------------------------*/
  while(union_date[i].year!=0){
    i++;
  }
  num_d=i;
  /*-----------------------------------------------------------------------------
   *  Alloc space for each base station
   *-----------------------------------------------------------------------------*/
  for(i=0;i<num_base_stations;i++){

    hourly_clim = (*base_stations[i]).hourly_clim;
    daily_clim = (*base_stations[i]).daily_clim;
 

    num_di = get_num_daywhourly(base_stations[i]);
    if(num_di == num_d && num_di>=2){
      /*  it has the hourly record for union_date already */

      continue;
    }
    else{
      /*  it lacks several record than the union_date */
      seq = (struct dated_sequence *)alloc((num_d*24+1) * sizeof(struct dated_sequence),
				    "sequence","reconstruct_hourly_clim.c");
      /*-----------------------------------------------------------------------------
       *  If there is hourly precipitation record in the ori base station, use it 
       *  Else if there is none, use the daily precipitation record and divided by 24
       *-----------------------------------------------------------------------------*/
      for(d=0;d<num_d;d++){
	  inx=0;
	  /*-----------------------------------------------------------------------------
	   *  Look into ori hourly precipitation data first
	   *-----------------------------------------------------------------------------*/
	  if(hourly_clim[0].rain.inx!=-999){

	    /*  Look for hourly precipitation data first */
	    dh=0;
	    while(hourly_clim[0].rain.seq[dh].edate.year!=0){
	      if(julday(hourly_clim[0].rain.seq[dh].edate)>julday(union_date[d])){
		inx=0;
		break;
	      }
	      else if(julday(hourly_clim[0].rain.seq[dh].edate)==julday(union_date[d])){
	        inx=1;
		break;
	      }
	      dh++;
	    }
	    
	    /*  Set value */
	    if(inx==1){
	      for(tmp=0;tmp<24;tmp++){
		seq[24*d+tmp].edate = hourly_clim[0].rain.seq[dh+tmp].edate;
		seq[24*d+tmp].value = hourly_clim[0].rain.seq[dh+tmp].value;

	      

	      }
	      continue;
	    }
	  }
	
	  /*-----------------------------------------------------------------------------
	   *  Look into the daily clim  data
	   *-----------------------------------------------------------------------------*/
	  if(inx ==0){
	      for(dd=0;dd<duration;dd++){
		if((julday(start_date)+dd)==julday(union_date[d])){
		  inx = 1;
		  
		  break;
		}
	      }
	      if(inx==1){

		tmp=0;
		for(tmp=0;tmp<24;tmp++){

      		  //seq[0].edate.year = union_date[d].year;
		  seq[24*d+tmp].edate.year=union_date[d].year;
		  seq[24*d+tmp].edate.month= union_date[d].month;
		  seq[24*d+tmp].edate.day  = union_date[d].day;
		  seq[24*d+tmp].edate.hour = tmp+1;
		  seq[24*d+tmp].value = daily_clim[0].rain[dd]/24;

		}
		continue;
	      }
	  }
	  
	  /*-----------------------------------------------------------------------------
	   *  If couldn't find precipitation data on this day
	   *-----------------------------------------------------------------------------*/
	  if(inx==0){
		tmp=0;
		for(tmp=0;tmp<24;tmp++){
		  seq[24*d+tmp].edate.year = union_date[d].year;
		  seq[24*d+tmp].edate.month= union_date[d].month;
		  seq[24*d+tmp].edate.day  = union_date[d].day;
		  seq[24*d+tmp].edate.hour = tmp+1;
		  seq[24*d+tmp].value = 0;
		}
	  }


      }
      seq[24*num_d+1].edate.year=0;

      /*-----------------------------------------------------------------------------
       * Replace the old seq with this new seq, free the memory 
       *-----------------------------------------------------------------------------*/
      if(hourly_clim[0].rain.inx !=-999){
	  free(hourly_clim[0].rain.seq);
      }
      hourly_clim[0].rain.inx=0;
      hourly_clim[0].rain.seq = seq;

    }


  }

}

