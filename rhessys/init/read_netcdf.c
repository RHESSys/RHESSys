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
  int ascnd;    //1: if ascending
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
    jm = (ju+jl) >> 1;  //compute a midpoint
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
/* wrapDateIndexPointer
** find first starting index in data to loop through
** that matches target data month/year, returns index integer.
**
** month - month of first data point to match
** day   - day of first data point to match
** start_date_index  - base index for starting search
** data_length - length of actual data array being used for generating repeats
*/
int wrap_repeat_date( int month, int day, int start_date_index, int data_length ) {
  int index = 0;
  int targetFound = 0;
  struct date candidateDate;

  while( targetFound == 0 && index < data_length ) {
    candidateDate = caldat( start_date_index + index );
    if( candidateDate.month == month && candidateDate.day == day ){
        targetFound = 1;
    }else{
        index++;
    }
  }

  if( targetFound == 0 ) {
    fprintf( stderr, 'error finding next date in repeat clim data.\n' );
    ERR(-1);
  }
  return index;
}

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
day_offset: this is an adjustment read in from the base station file to adjust for offset
   that occurs in some versions of netcdf
clim_repeat_flag: command line object that tells RHESSys to recycle through netcdf data
   for long simulations

   ************************************************************/

  int ncid, temp_varid,ndaysid,nlatid,nlontid;
  int dayid,latid,lontid;
  size_t nday,nlat,nlont;
  int ndims_in, nvars_in, ngatts_in, unlimdimid_in;
  int *days;
  float *lat,*lont;
  size_t start[3],count[3];
  int retval;
  int idlat,idlont;     //offset
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

  //fprintf(stderr, "days being measured: read_duration %d, nday %d\n", read_duration, nday );

  if((startday<days[0] || (duration+startday) > days[nday-1])){
    if( clim_repeat_flag == 0) {
      fprintf(stderr,"time period is out of the range of metdata\n");
      free(days);
      free(lat);
      free(lont);
      return -1;
    }else{
      //read_duration = nday;
      clim_repeat_flag = 1;
    }
  }

  //fprintf( stderr, "start day:%d daysNeeded:%d\n", startday-days[0]+day_offset, daysNeeded );

  /* Here we need to determine how much data we need to read from the netcdf file. If the clim repeat
     flag is set then we assume that we need to read through all the data (so we can loop through
     if the flag is not set, we read the amount requested by the user, assuming that they requested
     a valid range of data. If they did not, return an error */

  // if clim_repeat_flag, start reading from 0 index
  start[0] = clim_repeat_flag ? 0 : startday-days[0]+day_offset;                //netcdf 4.1.3 problem: there is 1 day offset
  start[1] = idlat;           //lat
  start[2] = idlont;

  // if clim_repeat_flag, read all the available data (nday stores the total number of available days)
  count[0] = clim_repeat_flag ? nday : duration;//nday - startday-days[0]+day_offset;//read_duration;
  count[1] = 1;
  count[2] = 1;
  /***Read netcdf data***/
  float * allActualData;

  if( !clim_repeat_flag ) {
    if ((retval = nc_get_vara_float(ncid,temp_varid,start,count,&data[0]))){
      free(days);
      free(lat);
      free(lont);
      ERR(retval);
    }
  }else{
    //fprintf( stderr, "READING ALL AVAILABLE DATA\n" );
    allActualData  = (float *) alloc(nday * sizeof(float),"allActualData","get_netcdf_var_timeserias");
    if ((retval = nc_get_vara_float(ncid,temp_varid,start,count,&allActualData[0]))){
      free(days);
      free(lat);
      free(lont);
      ERR(retval);
    }
  }
  //fprintf( stderr, "WE HAVE READ NETCDF\n" );

/* Check for Climate repeat flag. If flag is set, cycle through clim data
 *
 * Variables:
 *    nday : the total number of days in the actual netcdf file
 *    startday: the start of the metdata, the starting index to read from allActualData... the first date requested when this function is called
 *    start_date_index : index that says where in the netcdf data array we begin to read from
 *    allActualData: the entire dataset from the netcdf file, irrespective of how much data is requested by the user
 *    duration: the number of days of requested data
 *    data: an array passed as an argument to this function to be populated with the requested netcdf data
 */

  if( clim_repeat_flag ) {
    float * real_netcdf_data = allActualData;
    float * output_data = data;
    int total_days_in_netcdf_data = nday;
    struct date target_date;
    struct date curr_date;

    int requested_output_data_length = duration;

    // index that says where in the netcdf data array we begin to read from
    int read_start_index = startday - days[0] + day_offset;
    int start_date_index = read_start_index;

    // how many days of existing, sequential, real netcdf data to copy
    // directly into the beginning of our output_data array.
    int amount_to_memcpy = total_days_in_netcdf_data - read_start_index;

    fprintf( stderr, "start with copying %d days of %d total netcdf.\n", amount_to_memcpy, nday);
    memcpy( &output_data[ 0 ], &real_netcdf_data[ read_start_index ], amount_to_memcpy * sizeof(float) );

    // now we should have all the data from the start date to the end of the actual data copied over.
    // next comes looping through and creating repeated data as needed...

    int next_write_index = amount_to_memcpy;

    // index inside of netcdf data where we are getting records to repeat
    int read_data_index = 0;

    // get date object for next day after the last day held in days[]
    int last_date_in_netcdf_data = days[ total_days_in_netcdf_data - 1];
    struct date first_date_for_new_data = caldat( last_date_in_netcdf_data + 1 );

    // determine initial index to start drawing repeated data from
    read_data_index = wrap_repeat_date( first_date_for_new_data.month,
                                        first_date_for_new_data.day,
                                        days[0],
                                        total_days_in_netcdf_data );

    struct date next_date_to_fill;
    struct date candidate_repeat_date;

    for( int i = next_write_index; i < requested_output_data_length; i++ ) {
      next_date_to_fill  = caldat( last_date_in_netcdf_data + i - next_write_index );
      candidate_repeat_date = caldat( days[0] + read_data_index ); //day[0] is the point we start reading netcdfdata (it doesn't change)

      // Test to see if next day is feb. 29th in a leap year
      if( next_date_to_fill.month == 2 && next_date_to_fill.day == 29 ) {
        // if the current year of netcdf data is also a leap year...
        if( LEAPYR( candidate_repeat_date.year ) ) {
          if( read_data_index >= total_days_in_netcdf_data ) {
            read_data_index = wrap_repeat_date( next_date_to_fill.month,
                              next_date_to_fill.day,
                              days[0],
                              total_days_in_netcdf_data );
          }
          output_data[ i ] = real_netcdf_data[ read_data_index++ ];
        }else{
          // use previous day of data for feb. 29th
          output_data[ i ] = output_data[ i - 1 ];
        }
      }else{
        // if the repeat day is feb. 29th, just skip it.
        if( candidate_repeat_date.month == 2 && candidate_repeat_date.day == 29 ) {
          read_data_index++;
          candidate_repeat_date = caldat( days[0] + read_data_index );
        }
        if( read_data_index >= total_days_in_netcdf_data ) {
          read_data_index = wrap_repeat_date( next_date_to_fill.month,
                                              next_date_to_fill.day,
                                              days[0],
                                              total_days_in_netcdf_data );

          candidate_repeat_date = caldat( days[0] + read_data_index );
        }

        output_data[ i ] = real_netcdf_data[ read_data_index++ ];
     
        /*if( candidate_repeat_date.month != next_date_to_fill.month) {
            fprintf( stderr, "candidate month: %d, target month %d, target year %d\n", candidate_repeat_date.month, next_date_to_fill.month, next_date_to_fill.year );
        }*/
      } // end last else
    } // end for loop

    fprintf( stderr, "read_start_index %d, startday %d, durationRequest %d, days in dataset %d\n", read_start_index, startday, duration, nday );
  
 } // end if clim_repeat_flag

  if ((retval = nc_close(ncid))){
    free(days);
    free(lat);
    free(lont);
    ERR(retval);
  }

  free(days);
  free(lat);
  free(lont);
  if( clim_repeat_flag ) {
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
  int idlat,idlont;     //offset
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
  int idlat,idlont;     //offset
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
