#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netcdf.h>
//#include "rhessys.h"

#define NDAYS_NAME "time"
#define NLAT_NAME_OLD "y"
#define NLONT_NAME_OLD "x"

#define UNITS "units"
#define DESCRIPTION "description"

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERR(e) {fprintf(stderr,"Error: %s\n", nc_strerror(e)); return -1;}

#ifndef _LEAPYR
#define LEAPYR(y) (!((y)%400) || (!((y)%4) && ((y)%100)))
#endif

struct date
        {
        long    year;
        long    month;
        long    day;
        long    hour;
        };
int monthdays[12]={31,28,31,30,31,30,31,31,30,31,30,31};
void *alloc(size_t, char *, char *);
struct  date caldat(long);
/****from year,mon,day converted to days since STARTYEAR-01-01****/
int get_indays(int year,int mon,int day, int year_start, int leap_year){
  int inday=0;
  int i;
  if(year<year_start){
    fprintf(stderr,"time is earlier than netcdf file start year\n");
    return -1;
  }
  if (leap_year == 1) {
    for(i=year_start;i<year;i++){
      if(LEAPYR(i)) inday += 366;
      else inday += 365;
    }
  }
  else {
    for(i=year_start;i<year;i++){
      inday += 365;
    }
  }
  if(LEAPYR(year)) monthdays[1] = 29;
  else monthdays[1] = 28;
  for(i=1;i<mon;i++){
    inday += monthdays[i-1];
  }
  inday += (day-1);
  return inday;
}

/***Locate a value in an monotonic array***/
int locate(float *data, int n, float x, float md){
  int ascnd;	//1: if ascending
  int jl,ju,jm;
  float corl,coru,cormin;
  if(n<2){
    fprintf(stderr,"locate size error\n");
    return -1;
  }
  if(data[n-1] >= data[0]) ascnd = 1;
  else ascnd = 0;
  jl = 0;
  ju = n-1;
  while((ju-jl) > 1){
    jm = (ju+jl) >> 1;	//compute a midpoint
    if((x >= data[jm]) == ascnd)
      jl = jm;
    else
      ju = jm;
  }
  corl = fabs(data[jl]-x);
  coru = fabs(data[ju]-x);
  cormin = (corl<=coru?corl:coru);

  if(cormin<=md){
    if(corl <= coru) return jl;
    else return ju;
  }
  else{
    //fprintf(stderr,"can't locate\n");
    return -1;
  }
}
//_____________________________________________________________________________/
int get_netcdf_var_timeserias(char *netcdf_filename, char *varname, 
    char *nlat_name, char *nlon_name, 
    float rlat, float rlon, float sd, 
    int startday, int day_offset, int duration, int clim_repeat_flag, float *data ){
  /***Read netcdf format metdata by using lat,lon as index
varname: variable name
rlat,rlon: latitue and longitude of site location
sd: the minimum distance for searching nearby grid in netcdf
startday: startday of metdata (since STARTYEAR-01-01)
duration: days of required metdata

Mingliang Liu
Nov. 17, 2011
   ************************************************************/

  int ncid, temp_varid,ndaysid,nlatid,nlontid;
  int dayid,latid,lontid;
  size_t nday,nlat,nlont;
  int ndims_in, nvars_in, ngatts_in, unlimdimid_in;
  int *days;
  float *lat,*lont;
  size_t start[3],count[3];
  int retval;
  int idlat,idlont;	//offset
  /*printf("\n   Opening netcdf file...\n");*/
  /***open netcdf***/
  if((retval = nc_open(netcdf_filename, NC_NOWRITE, &ncid)))
    ERR(retval);
  if((retval = nc_inq(ncid, &ndims_in, &nvars_in, &ngatts_in,
          &unlimdimid_in)))
    ERR(retval);
  /*printf("\n   ncid=%d ndims_in=%d nvars_in=%d ngatts_in=%d unlimdimid_in=%d\n",ncid, ndims_in, nvars_in, ngatts_in, unlimdimid_in);*/
  /*printf("\n   Getting dimensions and var id... %d %s %d\n",ncid,NDAYS_NAME,ndaysid);*/
  /***Get the dimension and var id***/
  if((retval = nc_inq_dimid(ncid,NDAYS_NAME, &ndaysid)))
    ERR(retval);
  if((retval = nc_inq_dimid(ncid, nlat_name, &nlatid)))
    ERR(retval);
  if((retval = nc_inq_dimid(ncid, nlon_name, &nlontid)))
    ERR(retval);
  if((retval = nc_inq_dimlen(ncid, ndaysid, &nday)))
    ERR(retval);
  if((retval = nc_inq_dimlen(ncid, nlatid, &nlat)))
    ERR(retval);
  if((retval = nc_inq_dimlen(ncid, nlontid, &nlont)))
    ERR(retval);
  /* Get the varids of variables. */
  if ((retval = nc_inq_varid(ncid, NDAYS_NAME, &dayid)))
    ERR(retval);
  if ((retval = nc_inq_varid(ncid, nlat_name, &latid)))
    ERR(retval);
  if ((retval = nc_inq_varid(ncid, nlon_name, &lontid)))
    ERR(retval);
  if ((retval = nc_inq_varid(ncid, varname, &temp_varid)))
    ERR(retval);

  days = (int *) alloc(nday * sizeof(int),"days","get_netcdf_var_timeserias");
  lat = (float *) alloc(nlat * sizeof(float),"lat","get_netcdf_var_timeserias");
  lont = (float *) alloc(nlont * sizeof(float),"lont","get_netcdf_var_timeserias");
  /* get dimension var */
  if ((retval = nc_get_var_int(ncid, dayid, &days[0]))){
    free(days);
    free(lat);
    free(lont);	
    ERR(retval);
  }
  if ((retval = nc_get_var_float(ncid, latid, &lat[0]))){
    free(days);
    free(lat);
    free(lont);	
    ERR(retval);
  }
  if ((retval = nc_get_var_float(ncid, lontid, &lont[0]))){
    free(days);
    free(lat);
    free(lont);	
    ERR(retval);
  }
  /*locate the record */
  idlat = locate(lat,nlat,rlat,sd);
  idlont = locate(lont,nlont,rlon,sd);
  if(idlat == -1 || idlont == -1){
    fprintf(stderr,"rlat:%lf\trlon:%lf\tsd:%lf\tlat[0]:%lf\tlont[0]:%lf can't locate the station get_netcdf_var_timeseries\n",rlat,rlon,sd,lat[0],lont[0]);
    free(days);
    free(lat);
    free(lont);	
    return -1;
  }
  /*printf("\nstartday=%d duration=%d nday=%d day1=%d dayfin=%d\n",startday,duration,nday,days[0],days[nday-1]);*/
  //int MAX_DATA_SIZE = days[ nday - 1 ];
  int read_duration = duration;
  int shouldRepeat = 0;

  shouldRepeat = 1;// XXX artificial flag for testing; remove and get rid of subsequent instances
  
  //fprintf(stderr, "days being measured: read_duration %d, nday %d\n", read_duration, nday );

  if((startday<days[0] || (duration+startday) > days[nday-1])){
    if( clim_repeat_flag == 0 || shouldRepeat != 1 ) {
      fprintf(stderr,"time period is out of the range of metdata\n");
      free(days);
      free(lat);
      free(lont);	
      return -1;
    }else{
      //read_duration = nday;
      shouldRepeat = 1;
    }
  }
  
  int daysNeeded = duration - ( nday - startday-days[0]+day_offset );
  fprintf( stderr, "start day:%d daysNeeded:%d\n", startday-days[0]+day_offset, daysNeeded );

 
  // if shouldRepeat, start reading from 0 index
  start[0] = shouldRepeat ? 0 : startday-days[0]+day_offset;		//netcdf 4.1.3 problem: there is 1 day offset
  start[1] = idlat;           //lat
  start[2] = idlont;
  
  // if shouldRepeat, read all the available data (nday stores the total number of available days)
  count[0] = shouldRepeat ? nday : nday - startday-days[0]+day_offset;//read_duration;
  count[1] = 1;
  count[2] = 1;
  /***Read netcdf data***/ 
  float * allActualData;
 
  if( !shouldRepeat ) {
    if ((retval = nc_get_vara_float(ncid,temp_varid,start,count,&data[0]))){
      free(days);
      free(lat);
      free(lont);
      ERR(retval);
    }
  }else{
    fprintf( stderr, "READING ALL AVAILABLE DATA\n" );
    allActualData  = (float *) alloc(nday * sizeof(float),"allActualData","get_netcdf_var_timeserias");
    if ((retval = nc_get_vara_float(ncid,temp_varid,start,count,&allActualData[0]))){
      free(days);
      free(lat);
      free(lont);
      ERR(retval);
    }
  }
  fprintf( stderr, "WE HAVE READ NETCDF\n" );

/*
 * 1. Read all data from netcdf and store in separate array (allActualData). DONE
 * 2. Determine start index to read from allAcutalData.
 * 3. memcpy start index to end of allActualData into passed data[] array (assuming we want that many days)
 * 4. we are now at the end of the allActualData array. Starting from the beginning of allActualData, we try
 *    to find matching month/day combinations for subsequent required data points. So for each further required data point, we:
 *    - increment a counter
 *    - check the value held in allActualData at the counter position
 *    - if the month and day of the value is the same as the month and data we need for our next data point, copy the value into our target data[] array.
 *    - else repeat this process
 *
 * 5. For each step in the above loop, check to see if we've filled in the required amount of data; if so, return. Also check to see if we've passed the end
 *    of the allActualData array... if so, reset counter to 0.
 *
 * 6. Need to read clim_repeat_flag from command line to determine whether any of this takes place.
 *
 * Variables:
 *    nday : the total number of days in the actual netcdf file
 *    allActualData: the entire dataset from the netcdf file, irrespective of how much data is requested by the user
 *    startday: the starting index to read from allActualData... the first date requested when this function is called.
 *    duration: the number of days of requested data... 
 *    data: an array passed as an argument to this function to be populated with the requested netcdf data
 */

  if( shouldRepeat ) {
    // XXX put Naomi's code in here
    // pseudocode:
    struct date target_date;
    struct date curr_date;
    int start_date = startday - days[0] + day_offset;
    int amountToMemCopy = nday - start_date;

    memcpy( &allActualData[ start_date ], &data[0], amountToMemCopy );
/* 
 * now we should have all the data from the start date to the end of the actual data copied over. 
 * next comes looping through and artifically creating the other data points.
 */   

    fprintf( stderr, "start_date %d, startday %d, durationRequest %d, days in dataset %d\n", start_date, startday, duration, nday );

    for( int i = amountToMemCopy; i < duration; i++ ) {
      int target_fnd = 0;
      int j = 0;
      
      // our next data point to populate
      target_date = caldat( start_date + i );

      while( target_fnd == 0 && j < nday ) {	
          curr_date = caldat( start_date + j );
          if ((curr_date.month == target_date.month) && (curr_date.day == target_date.day)) {
            target_fnd = 1;
          }
          j++;
      }

      if( j < nday && target_fnd == 1 ) {
        fprintf( stderr, "match! j: %d, i: %d\n", j, i );
        data[i]  = allActualData[j];
      } else {
        fprintf( stderr, "target_date.day %d, target_date.month %d, target_date.year %d\n", target_date.day, target_date.month, target_date.year ); 
        fprintf(stderr,"FATAL ERROR: in construct_clim_sequence\n");
        fprintf(stderr,"\n not enough data in base climate to repeat\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  if ((retval = nc_close(ncid))){
    free(days);
    free(lat);
    free(lont);
    ERR(retval);
  }

  free(days);
  free(lat);
  free(lont);
  if( shouldRepeat ) {
    free( allActualData );
  }
  return 0;
}
//_____________________________________________________________________________/
int get_netcdf_var(char *netcdf_filename, char *varname, 
    char *nlat_name, char *nlon_name,
    float rlat, float rlon, float sd, float *data){
  /***Read netcdf format metdata by using lat,lon as index
    NO TIME DIMENSION
varname: variable name
rlat,rlon: y and x of site location
sd: the minimum distance for searching nearby grid in netcdf
   ************************************************************/

  int ncid, temp_varid,nlatid,nlontid;
  int latid,lontid;
  size_t nlat,nlont;
  int ndims_in, nvars_in, ngatts_in, unlimdimid_in;
  float *lat,*lont;
  size_t start[2],count[2];
  int retval;
  int idlat,idlont;	//offset
  /*printf("\n   Opening netcdf file...\n");*/
  /***open netcdf***/
  if((retval = nc_open(netcdf_filename, NC_NOWRITE, &ncid)))
    ERR(retval);
  if((retval = nc_inq(ncid, &ndims_in, &nvars_in, &ngatts_in,
          &unlimdimid_in)))
    ERR(retval);
  /*printf("\n   ncid=%d ndims_in=%d nvars_in=%d ngatts_in=%d unlimdimid_in=%d\n",ncid, ndims_in, nvars_in, ngatts_in, unlimdimid_in);*/
  /***Get the dimension and var id***/
  if((retval = nc_inq_dimid(ncid, nlat_name, &nlatid)))
    ERR(retval);
  if((retval = nc_inq_dimid(ncid, nlon_name, &nlontid)))
    ERR(retval);
  if((retval = nc_inq_dimlen(ncid, nlatid, &nlat)))
    ERR(retval);
  if((retval = nc_inq_dimlen(ncid, nlontid, &nlont)))
    ERR(retval);
  /* Get the varids of variables. */
  if ((retval = nc_inq_varid(ncid, nlat_name, &latid)))
    ERR(retval);
  if ((retval = nc_inq_varid(ncid, nlon_name, &lontid)))
    ERR(retval);
  if ((retval = nc_inq_varid(ncid, varname, &temp_varid)))
    ERR(retval);

  lat = (float *) alloc(nlat * sizeof(float),"lat","get_netcdf_var");
  lont = (float *) alloc(nlont * sizeof(float),"lont","get_netcdf_var");
  /* get dimention var */
  if ((retval = nc_get_var_float(ncid, latid, &lat[0]))){
    free(lat);
    free(lont);	
    ERR(retval);
  }
  if ((retval = nc_get_var_float(ncid, lontid, &lont[0]))){
    free(lat);
    free(lont);	
    ERR(retval);
  }
  /*locate the record */
  idlat = locate(lat,nlat,rlat,sd);
  idlont = locate(lont,nlont,rlon,sd);
  if(idlat == -1 || idlont == -1){
    fprintf(stderr,"rlat:%lf\trlon:%lf can't locate the station get_netcdf_var\n",rlat,rlon);
    free(lat);
    free(lont);	
    return -1;
  }

  start[0] = idlat;           //lat
  start[1] = idlont;
  count[0] = 1;
  count[1] = 1;
  /***Read netcdf data***/ 
  if ((retval = nc_get_vara_float(ncid,temp_varid,start,count,&data[0]))){
    free(lat);
    free(lont);
    ERR(retval);
  }

  if ((retval = nc_close(ncid))){
    free(lat);
    free(lont);
    ERR(retval);
  }

  free(lat);
  free(lont);
  return 0;
}
//_____________________________________________________________________________/
int get_netcdf_xy(char *netcdf_filename, char *nlat_name, char *nlon_name,
    float rlat, float rlon, float sd, float *y, float *x){
  /***Read netcdf format metdata by using lat,lon as index and return x, y coords
    rlat,rlon: latitue and longitude of site location
sd: the minimum distance for searching nearby grid in netcdf
   ************************************************************/

  int ncid, nlatid,nlontid;
  int latid,lontid;
  size_t nlat,nlont;
  int ndims_in, nvars_in, ngatts_in, unlimdimid_in;
  float *lat,*lont;
  //size_t start[3],count[3];
  int retval;
  int idlat,idlont;	//offset
  /*printf("\n   Opening netcdf file... %s lat=%lf lon=%lf sd=%lf",
    netcdf_filename,
    rlat,
    rlon,
    sd);*/
  /***open netcdf***/
  if((retval = nc_open(netcdf_filename, NC_NOWRITE, &ncid)))
    ERR(retval);
  if((retval = nc_inq(ncid, &ndims_in, &nvars_in, &ngatts_in,
          &unlimdimid_in)))
    ERR(retval);
  //printf("\n   ncid=%d ndims_in=%d nvars_in=%d ngatts_in=%d unlimdimid_in=%d\n",ncid, ndims_in, nvars_in, ngatts_in, unlimdimid_in);
  /***Get the dimension and var id***/
  if((retval = nc_inq_dimid(ncid, nlat_name, &nlatid)))
    ERR(retval);
  if((retval = nc_inq_dimid(ncid, nlon_name, &nlontid)))
    ERR(retval);
  if((retval = nc_inq_dimlen(ncid,nlatid, &nlat)))
    ERR(retval);
  if((retval = nc_inq_dimlen(ncid,nlontid, &nlont)))
    ERR(retval);
  /* Get the varids of variables. */
  if ((retval = nc_inq_varid(ncid, nlat_name, &latid)))
    ERR(retval);
  if ((retval = nc_inq_varid(ncid, nlon_name, &lontid)))
    ERR(retval);

  lat = (float *) alloc(nlat * sizeof(float),"lat","get_netcdf_xy");
  lont = (float *) alloc(nlont * sizeof(float),"lont","get_netcdf_xy");
  /* get dimention var */
  if ((retval = nc_get_var_float(ncid, latid, &lat[0]))){
    free(lat);
    free(lont);	
    ERR(retval);
  }
  if ((retval = nc_get_var_float(ncid, lontid, &lont[0]))){
    free(lat);
    free(lont);	
    ERR(retval);
  }

  /*locate the record */
  idlat = locate(lat,nlat,rlat,sd);
  idlont = locate(lont,nlont,rlon,sd);
  if(idlat == -1 || idlont == -1){
    fprintf(stderr,"rlat:%lf\trlon:%lf can't locate the station get_netcdf_xy\n",rlat,rlon);
    free(lat);
    free(lont);	
    return -1;
  }
  else {
    *x = lont[idlont];
    *y = lat[idlat];
  }

  if ((retval = nc_close(ncid))){
    free(lat);
    free(lont);
    ERR(retval);
  }

  free(lat);
  free(lont);
  return 0;
}
