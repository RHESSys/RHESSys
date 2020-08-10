/*--------------------------------------------------------------*/
/*	construct_netcdf_grid.c - creates a zone object	*/
/*	SYNOPSIS   */
/*	struct base_station_object **construct_netcdf_grid(*/
/*		base_station_file_name,*/
/*		start_date,*/
/*		duration*/
/*		column_index);	*/
/*	OPTIONS	*/
/*	DESCRIPTION	*/
/*	PROGRAMMER NOTES*/
/***
  Read netcdf format met data to construct a zone object
  The netcdf cdf has three dimention (day,lat,lon)
  day is the day since 1900-1-1

  By Mingliang Liu
  Nov. 18, 2011, WSU Pullman
 ***/



/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "UTM.h"  

struct base_station_object *construct_netcdf_grid (
#ifdef LIU_NETCDF_READER
                struct base_station_object *base_station_in,
#endif
                struct base_station_ncheader_object *base_station_ncheader,
                int			*num_world_base_stations,
                float		zone_x,
                float		zone_y,
                float		zone_z,
                struct		date *start_date,
                struct		date *duration,
                struct command_line_object *command_line
                )
{
        /*--------------------------------------------------------------*/
        /*	Local function definition.									*/
        /*--------------------------------------------------------------*/


        struct daily_optional_clim_sequence_flags
        {
                int atm_trans;
                int CO2;
                int cloud_fration;
                int cloud_opacity;
                int dayl;
                int Delta_T;
                int dewpoint;
                int base_station_effective_lai;
                int Kdown_diffuse;
                int Kdown_direct;
                int LAI_scalar;
                int Ldown;
                int PAR_diffuse;
                int PAR_direct;
                int daytime_rain_duration;
                int relative_humidity;
                int snow;
                int tdewpoint;
                int tday;
                int tnight;
                int tnightmax;
                int tavg;
                int tsoil;
                int vpd;
                int wind;
                int ndep_NO3;
                int ndep_NH4;
                int lapse_rate_tmax;
                int lapse_rate_tmin;
        };

        void	*alloc( 	size_t, char *, char *);
        struct	base_station_object *base_station;
        /*--------------------------------------------------------------*/
        /*	Local variable definition.									*/
        /*--------------------------------------------------------------*/
        int		i;
        int		j;
        int		k;
        int inx;

        int year_start, leap_year;
        float net_x, net_y;
        float sdist, day_offset, precip_mult;

        struct	daily_optional_clim_sequence_flags	daily_flags;
        struct	date first_date;

        char	first[MAXSTR];		//I use first & second for the while loop that reads the first base station file
        char	second[MAXSTR];	
        double	eff_lai;
        double	screen_height;
        char	buffer[MAXSTR*100];
        char	buffertmax[MAXSTR*100];
        char	buffertmin[MAXSTR*100];
        char	bufferrain[MAXSTR*100];
        char *lat_name = "lat";       
        char *lon_name = "lon";    
        //Temporalily add threshold for tmax and tmin to check for errors in input data
        //TO DO: put these into one of the def files or one extra input files
        double max_tmax;
        double min_tmin;  

        //For WA, the threshold are below
        //http://www.ncdc.noaa.gov/extremes/scec/records
        max_tmax = 50;
        min_tmin = -50;

        FILE*	base_station_file;

        int instartday;		//days since Jan 1, STARTYEAR
        float *tempdata;	//temporary memory to read netcdf data
        int baseid;
        setvbuf(stdout,NULL,_IONBF,0);
        /* allocate daily_optional_clim_sequence_flags struct and make sure set to 0 */


        memset(&daily_flags, 0, sizeof(struct daily_optional_clim_sequence_flags));

        /*printf("\n      Construct netcdf cell: START lastID=%d lai=%lf",
          base_station_ncheader[0].lastID,
          base_station_ncheader[0].effective_lai);*/

        i = base_station_ncheader[0].lastID + 1;

        /* Allocate for the new base station cell */
#ifdef LIU_NETCDF_READER
        base_station = base_station_in;
#else
        base_station = (struct base_station_object*) alloc(1 * sizeof(struct base_station_object),
                        "base_station", "construct_netcdf_grid");
#endif

#ifndef LIU_NETCDF_READER
        base_station[0].ID = base_station_ncheader[0].lastID + 1;
        base_station[0].x = zone_x;
        base_station[0].y = zone_y;
        base_station[0].effective_lai = base_station_ncheader[0].effective_lai;
        base_station[0].screen_height = base_station_ncheader[0].screen_height;
        net_x = zone_x;
        net_y = zone_y;
#else
        net_x = base_station[0].lon;                                                 // x;
        net_y = base_station[0].lat;                                                 // y;
#endif

        /*--------------------------------------------------------------*/
        /* Allocate daily clim structures and clim seqs					*/
        /*--------------------------------------------------------------*/	

        /* For each daily clim structure allocate clim seqs for all required & optional clims */
        base_station[0].daily_clim = (struct daily_clim_object *)
                alloc(1*sizeof(struct daily_clim_object),"daily_clim","construct_netcdf_grid" );
        //duration.day is a long that was passed into construct_ascii as a date struct
        base_station[0].daily_clim[0].tmax = (double *) alloc(duration->day * sizeof(double),"tmax", "construct_netcdf_grid");
        base_station[0].daily_clim[0].tmin = (double *) alloc(duration->day * sizeof(double),"tmin", "construct_netcdf_grid");
        base_station[0].daily_clim[0].rain = (double *) alloc(duration->day * sizeof(double),"rain", "construct_netcdf_grid");
#ifdef LIU_EXTEND_CLIM_VAR
        base_station[0].daily_clim[0].relative_humidity_max = (double *) alloc(duration->day * sizeof(double),"relative_humidity_max", "construct_netcdf_grid");
        base_station[0].daily_clim[0].relative_humidity_min = (double *) alloc(duration->day * sizeof(double),"relative_humidity_min", "construct_netcdf_grid");
        base_station[0].daily_clim[0].relative_humidity     = (double *) alloc(duration->day * sizeof(double),"relative_humidity", "construct_netcdf_grid");
        base_station[0].daily_clim[0].specific_humidity     = (double *) alloc(duration->day * sizeof(double),"specific_humidity", "construct_netcdf_grid");
        base_station[0].daily_clim[0].surface_shortwave_rad = (double *) alloc(duration->day * sizeof(double),"surface_shortwave_rad", "construct_netcdf_grid");
        base_station[0].daily_clim[0].wind                  = (double *) alloc(duration->day * sizeof(double),"wind", "construct_netcdf_grid");
#else
        base_station[0].daily_clim[0].relative_humidity = NULL;
        base_station[0].daily_clim[0].wind              = NULL;
#endif
        /*--------------------------------------------------------------*/
        /*	initialize the rest of the clim sequences as null	*/
        /*--------------------------------------------------------------*/
        base_station[0].daily_clim[0].atm_trans = NULL;
        base_station[0].daily_clim[0].CO2 = NULL;
        base_station[0].daily_clim[0].cloud_fraction = NULL;
        base_station[0].daily_clim[0].cloud_opacity = NULL;
        base_station[0].daily_clim[0].dayl = NULL;
        base_station[0].daily_clim[0].Delta_T = NULL;
        base_station[0].daily_clim[0].dewpoint = NULL;
        base_station[0].daily_clim[0].base_station_effective_lai = NULL;
        base_station[0].daily_clim[0].Kdown_diffuse = NULL;
        base_station[0].daily_clim[0].Kdown_direct = NULL;
        base_station[0].daily_clim[0].LAI_scalar = NULL;
        base_station[0].daily_clim[0].Ldown = NULL;
        base_station[0].daily_clim[0].PAR_diffuse = NULL;
        base_station[0].daily_clim[0].PAR_direct = NULL;
        base_station[0].daily_clim[0].daytime_rain_duration = NULL; 
        base_station[0].daily_clim[0].snow = NULL;
        base_station[0].daily_clim[0].tdewpoint = NULL;
        base_station[0].daily_clim[0].tday = NULL;
        base_station[0].daily_clim[0].tnight = NULL;
        base_station[0].daily_clim[0].tnightmax = NULL;
        base_station[0].daily_clim[0].tavg = NULL;
        base_station[0].daily_clim[0].tsoil = NULL;
        base_station[0].daily_clim[0].vpd = NULL;
        base_station[0].daily_clim[0].ndep_NO3 = NULL;
        base_station[0].daily_clim[0].ndep_NH4 = NULL;

        /*Check if any flags are set in the optional clim sequence struct*/
        if ( daily_flags.daytime_rain_duration == 1 ) {
                base_station[0].daily_clim[0].daytime_rain_duration = (double *) 
                        alloc(duration->day * sizeof(double),"day_rain_dur", "construct_netcdf_grid");

        }
       /*------------------------------------------------------------*/
       /* convert the WGS84 lon and lat to UTM                      */
       /*-----------------------------------------------------------*/
        int utm_zone;
        float x_utm;
        float y_utm;
        float lat;
        float lon;
        int return_value;

        lat = base_station[0].lat;
        lon = base_station[0].lon;
        if (command_line[0].ncgridinterp_flag != NULL) {
        utm_zone = command_line[0].utm_zone;
        printf("\n Read in the UTM zone from command line for climate data interpolation, the zone is %d \n", utm_zone);
        }
        else {
        utm_zone = 12;
       // printf("\n Not specify the UTM zone in command line, using default zone UTM 12, if you want to change the UTM zone using -ncgridinterp NUM \n");
        }

         return_value= LatLonToUTMXY(lat, lon, utm_zone, &x_utm, &y_utm);
         printf(" the utm x and utm y are %f, %f \n",x_utm, y_utm);
         base_station[0].proj_x = (double)x_utm;
         base_station[0].proj_y = (double)y_utm;


        /*--------------------------------------------------------------*/
        /*	Allocate the yearly clim object.								*/
        /*--------------------------------------------------------------*/
        base_station[0].yearly_clim = (struct yearly_clim_object *)
                alloc(1*sizeof(struct yearly_clim_object), "yearly_clim", "construct_netcdf_grid" );
        /*	Initialize non-critical sequences							*/
        base_station[0].yearly_clim[0].temp = NULL;		
        /*--------------------------------------------------------------*/
        /*	Allocate the monthly clim object.							*/	
        /*--------------------------------------------------------------*/
        base_station[0].monthly_clim = (struct monthly_clim_object *)
                alloc(1*sizeof(struct monthly_clim_object), "monthly_clim", "construct_netcdf_grid" );
        /*	Initialize non-critical sequences							*/
        base_station[0].monthly_clim[0].temp = NULL;
        /*--------------------------------------------------------------*/
        /*	Allocate the hourly clim object.							*/
        /*--------------------------------------------------------------*/
        base_station[0].hourly_clim = (struct hourly_clim_object *)
                alloc(1*sizeof(struct hourly_clim_object), "hourly_clim", "construct_netcdf_grid" );
        /*	Initialize non - critical sequences.						*/
        base_station[0].hourly_clim[0].rain.inx = -999;
        base_station[0].hourly_clim[0].rain_duration.inx = -999;
        /* Calculate start day index */
        instartday = get_indays((int)start_date->year,
                        (int)start_date->month,
                        (int)start_date->day,
                        base_station_ncheader[0].year_start,
                        base_station_ncheader[0].leap_year);

        //if( command_line[0].clim_repeat_flag ) { 
                tempdata = (float *) alloc(duration->day * sizeof(float),"tempdata","construct_netcdf_grid");
        //}
        /* printf("net_y:%f net_x:%f\n",base_station[0].net_y,base_station[0].net_x);
           printf("tmax filename:%s varname:%s sdist:%f instartday:%d dura:%d\n",base_station[0].netcdf_tmax_filename, base_station[0].netcdf_tmax_varname,base_station[0].sdist,instartday,duration.day); */
        enum CLIM_VARS {CLM_TMAX, CLM_TMIN, CLM_RAIN, CLM_HUSS, CLM_RMAX,
                        CLM_RMIN, CLM_RSDS, CLM_WAS, clim_vars_counts};
        for (int var = 0; var < clim_vars_counts; var ++) {
            char *filename;
            char *var_name;
            switch (var) {
            case CLM_TMAX:
                filename = base_station_ncheader[0].netcdf_tmax_filename;
                var_name = base_station_ncheader[0].netcdf_tmax_varname;
                break;
            case CLM_TMIN:
                filename = base_station_ncheader[0].netcdf_tmin_filename;
                var_name = base_station_ncheader[0].netcdf_tmin_varname;
                break;
            case CLM_RAIN:
                filename = base_station_ncheader[0].netcdf_rain_filename;
                var_name = base_station_ncheader[0].netcdf_rain_varname;
                break;
#ifdef LIU_EXTEND_CLIM_VAR
            case CLM_HUSS:
                filename = base_station_ncheader[0].netcdf_huss_filename;
                var_name = base_station_ncheader[0].netcdf_huss_varname;
                break;
            case CLM_RMAX:
                filename = base_station_ncheader[0].netcdf_rmax_filename;
                var_name = base_station_ncheader[0].netcdf_rmax_varname;
                break;
            case CLM_RMIN:
                filename = base_station_ncheader[0].netcdf_rmin_filename;
                var_name = base_station_ncheader[0].netcdf_rmin_varname;
                break;
            case CLM_RSDS:
                filename = base_station_ncheader[0].netcdf_rsds_filename;
                var_name = base_station_ncheader[0].netcdf_rsds_varname;
                break;
            case CLM_WAS:
                filename = base_station_ncheader[0].netcdf_was_filename;
                var_name = base_station_ncheader[0].netcdf_was_varname;
                break;
#endif
            default:
                break;
            } //switch
            k = get_netcdf_var_timeserias(filename, var_name, lat_name,
                   lon_name, net_y, net_x,
                   (float)base_station_ncheader[0].resolution_dd, instartday,
                   base_station_ncheader[0].day_offset, (int)duration->day,
                   command_line[0].clim_repeat_flag, tempdata);
            if (k == -1){
                fprintf(stderr,"can't locate station data in netcdf for var %s\n", var_name);
                exit(0);
            }
            for (j=0;j<duration->day;j++){
                if (var == CLM_TMAX) {
                    if ((base_station_ncheader[0].temperature_unit == 'K') || (tempdata[j] > 150.0)) // kind of hard coded for temperature > 150
                        base_station[0].daily_clim[0].tmax[j] =  (double)tempdata[j] - 273.15;
                    else
                        base_station[0].daily_clim[0].tmax[j] =  (double)tempdata[j];
                } else if (var == CLM_TMIN) {
                    if ((base_station_ncheader[0].temperature_unit == 'K') || (tempdata[j] > 150.0)) // kind of hard coded for temperature > 150
                        base_station[0].daily_clim[0].tmin[j] =  (double)tempdata[j] - 273.15;
                    else
                        base_station[0].daily_clim[0].tmin[j] =  (double)tempdata[j];
                } else if (var == CLM_RAIN) {
                    base_station[0].daily_clim[0].rain[j] = (double)tempdata[j] * base_station_ncheader[0].precip_mult;
                }
#ifdef LIU_EXTEND_CLIM_VAR
                else if (var == CLM_HUSS) {
                    base_station[0].daily_clim[0].specific_humidity[j] = (double)tempdata[j];
                } else if (var == CLM_RMAX) {
                    base_station[0].daily_clim[0].relative_humidity_max[j] = (double)tempdata[j] * base_station_ncheader[0].rhum_mult;
                } else if (var == CLM_RMIN) {
                    base_station[0].daily_clim[0].relative_humidity_min[j] = (double)tempdata[j] * base_station_ncheader[0].rhum_mult;
                } else if (var == CLM_RSDS) {
                    base_station[0].daily_clim[0].surface_shortwave_rad[j] = (double)tempdata[j];
                } else if (var == CLM_WAS) {
                    base_station[0].daily_clim[0].wind[j] = (double)tempdata[j];
                }
#endif
            } //j
        } //var
#ifdef LIU_EXTEND_CLIM_VAR
        for (j=0;j<duration->day;j++) {
            struct  daily_clim_object *daily_clim = &base_station[0].daily_clim[0];
            daily_clim->relative_humidity[j] =
                (daily_clim->relative_humidity_max[j]
                 + daily_clim->relative_humidity_min[j]) / 2.0;
        }
#endif
/*#ifdef CHECK_NCCLIM_DATA
           for (j = 0; j < (duration->day < 60 ? duration->day : 60); j++) {
#ifndef LIU_EXTEND_CLIM_VAR
           fprintf(stdout,"day:%d\tid:%d\tx:%lf\ty:%lf\tlon:%lf\tlat:%lf\ttmax:%lf\ttmin:%lf\tppt:%lf\n"
           ,j,base_station[0].ID,base_station[0].proj_x,base_station[0].proj_y,base_station[0].lon, base_station[0].lat
           ,base_station[0].daily_clim[0].tmax[j],base_station[0].daily_clim[0].tmin[j],base_station[0].daily_clim[0].rain[j]);
#else
           fprintf(stdout,"day:%d\tid:%d\tx:%lf\ty:%lf\tlon:%lf\tlat:%lf\ttmax:%lf\ttmin:%lf\tppt:%lf\thuss:%lf\trmax:%lf\trmin:%lf\trsds:%lf\twas:%lf\n"
               ,j,base_station[0].ID,base_station[0].proj_x,base_station[0].proj_y,base_station[0].lon, base_station[0].lat
               ,base_station[0].daily_clim[0].tmax[j],base_station[0].daily_clim[0].tmin[j],base_station[0].daily_clim[0].rain[j]
               ,base_station[0].daily_clim[0].specific_humidity[j],base_station[0].daily_clim[0].relative_humidity_max[j]
               ,base_station[0].daily_clim[0].relative_humidity_min[j],base_station[0].daily_clim[0].surface_shortwave_rad[j]
               ,base_station[0].daily_clim[0].wind[j]);

#endif
           }
#endif */

       /*Check for abnormal values in tmin tmax */

        for (j=0; j<duration->day;j++){

            if ((base_station[0].daily_clim[0].tmin[j] < min_tmin) || (base_station[0].daily_clim[0].tmin[j] > max_tmax))
            {
                printf("WARNING: day:%d tmin: %f smaller than tmin & bigger than tmax thresholds for ID %f\n", j, base_station[0].daily_clim[0].tmin[j],  min_tmin);
                if ( j==0 || j==duration->day) // if first day or last day
                {
                    base_station[0].daily_clim[0].tmin[j] = min_tmin;
                    printf("Tmin after fixing 1 (tmin) : %f \n", base_station[0].daily_clim[0].tmin[j]);

                }
                else  //do interpolation between the previous day and the next day
                {
                    if (base_station[0].daily_clim[0].tmin[j+1] > min_tmin) // if the next day doesn't have error
                    {
                        base_station[0].daily_clim[0].tmin[j] = 0.5* (base_station[0].daily_clim[0].tmin[j+1] + base_station[0].daily_clim[0].tmin[j-1]);
                        printf("Tmin after fixing 2 (interpolation): %f \n", base_station[0].daily_clim[0].tmin[j]);

                    }
                    else
                    {
                        base_station[0].daily_clim[0].tmin[j] = base_station[0].daily_clim[0].tmin[j-1];
                        printf("Tmin after fixing 3 (using previous non NA values): %f \n", base_station[0].daily_clim[0].tmin[j]);

                    }


                }

            }

            // check the abnormal values in tmax
            if ((base_station[0].daily_clim[0].tmax[j] > max_tmax) || (base_station[0].daily_clim[0].tmax[j] < min_tmin))
            {
                printf("WARNING: day:%d tmax:%f bigger than tmax & smaller than tmin thresholds for WA %f\n", j, base_station[0].daily_clim[0].tmax[j], max_tmax);
                if (j==0 || j==duration->day)  // if the first day or last day
                {
                    base_station[0].daily_clim[0].tmax[j] = max_tmax;
                    printf(" Tmax after fixing 1 (tmax WA): %f \n", base_station[0].daily_clim[0].tmax[j]);
                }
                else  //do interpolation between the previous day & the next day

                {
                    if (base_station[0].daily_clim[0].tmax[j+1] < max_tmax)  // if the next day doesn't have error
                    {
                        base_station[0].daily_clim[0].tmax[j] = 0.5*(base_station[0].daily_clim[0].tmax[j+1] + base_station[0].daily_clim[0].tmax[j-1]);
                        printf("Tmax after fixing 2 (interpolation): %f \n", base_station[0].daily_clim[0].tmax[j]);
                    }
                    else
                    {
                        base_station[0].daily_clim[0].tmax[j] = base_station[0].daily_clim[0].tmax[j-1];
                        printf("Tmax after fixing 3 (previous non NA value): %f \n", base_station[0].daily_clim[0].tmax[j]);

                    }


                }


            }

            // check tmax and tmin
            if ((base_station[0].daily_clim[0].tmax[j])< base_station[0].daily_clim[0].tmin[j]) {
                printf("\n WARNING: day: %d, tmax %f is smaller than tmin %f \n", j, base_station[0].daily_clim[0].tmax[j], base_station[0].daily_clim[0].tmin[j]);
            }

        }



        /* ------------------ ELEV ------------------ */
        if (base_station_ncheader[0].elevflag == 0) {
#ifndef LIU_NETCDF_READER
                base_station[0].z = zone_z;
#endif
        }
        else {
                float *elev_tempdata = (float *) alloc(1 * sizeof(float),"tempdata","construct_netcdf_grid");
                k = get_netcdf_var(
                                base_station_ncheader[0].netcdf_elev_filename,
                                base_station_ncheader[0].netcdf_elev_varname,
                                lat_name, //"lat",
                                lon_name, // "lon",
                                /*base_station_ncheader[0].netcdf_y_varname,
                                  base_station_ncheader[0].netcdf_x_varname,*/
                                net_y,
                                net_x,
                                (float)base_station_ncheader[0].resolution_dd/*sdist*/,
                                elev_tempdata);
                if (k == -1){
                        fprintf(stderr,"can't locate station data in netcdf for var elev\n");
                        exit(0);
                }
                base_station[0].z = (double)elev_tempdata[0];
                free(elev_tempdata);
        }


        //if( command_line[0].clim_repeat_flag ) { 
                free(tempdata);
        //}
        /*printf("\n      Construct netcdf cell: END ID=%d x=%lf y=%lf lai=%lf i=%d",
          base_station[0].ID,
          base_station[0].x,
          base_station[0].y,
          base_station[0].effective_lai,
          i);*/
#ifndef LIU_NETCDF_READER
        *num_world_base_stations +=1;
        base_station_ncheader[0].lastID +=1;
#endif
        printf( "BASE STATION ID? %d\n", base_station[0].ID );
        return(base_station);
}

