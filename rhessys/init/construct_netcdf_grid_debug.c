/*--------------------------------------------------------------*/
/*	construct_netcdf_grid.c - creates a zone object	*/
/*	SYNOPSIS   */
/*	struct base_station_object **construct_netcdf_grid(*/
/*		base_station_file_name, */
/*		start_date, */
/*		duration*/
/*		column_index);	*/
/*	OPTIONS	*/
/*	DESCRIPTION	*/
/*	PROGRAMMER NOTES*/
/***
	Read netcdf format met data to construct a zone object
	The netcdf cdf has three dimention (day, lat, lon)
	day is the day since 1900-1-1
	
	By Mingliang Liu
	Nov. 18,  2011,  WSU Pullman
***/



/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

int get_netcdf_var_timeserias(char *,  char *,  char *,  char *,  float,  float,  float,  int,  int,  int,  float *);
int get_netcdf_xy(char *,  char *,  char *,  float,  float,  float,  float *,  float *);
int get_netcdf_var(char *,  char *,  char *,  char *,  float,  float,  float,  float *);
int get_indays(int, int, int, int, int);	//get days since XXXX-01-01

struct base_station_object *construct_netcdf_grid (
								struct base_station_ncheader_object *base_station_ncheader, 
								int			*num_world_base_stations, 
								float		zone_x, 
								float		zone_y, 
								float		zone_z, 
								struct		date start_date, 
								struct		date duration)
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
	
	void	*alloc( size_t,  char *,  char *);
	
	struct	base_station_object *base_station;
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i;
	int		j;
	int		k;
	int 	inx;
	
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
	
	FILE*	base_station_file;

	int instartday;		//days since Jan 1,  STARTYEAR
	float *tempdata;	//temporary memory to read netcdf data
	
	int baseid;

	
	setvbuf(stdout, NULL, _IONBF, 0);
	/* allocate daily_optional_clim_sequence_flags struct and make sure set to 0 */
	memset(&daily_flags, 0, sizeof(struct daily_optional_clim_sequence_flags));

	printf("Construct netcdf cell: START lastID=%d lai=%lf \n", 
		   base_station_ncheader[0].lastID, 
		   base_station_ncheader[0].effective_lai);
	
	i = base_station_ncheader[0].lastID + 1;

	/* Allocate for the new base station cell */
	base_station = (struct base_station_object*) alloc(1 * sizeof(struct base_station_object), 
														   "base_station",  "construct_netcdf_grid");

	base_station[0].ID = base_station_ncheader[0].lastID + 1;
	base_station[0].x = zone_x;
	base_station[0].y = zone_y;
	base_station[0].effective_lai = base_station_ncheader[0].effective_lai;
	base_station[0].screen_height = base_station_ncheader[0].screen_height;
	
	
	net_x = zone_x;
	net_y = zone_y;
	
	/*--------------------------------------------------------------*/
	/* Allocate daily clim structures and clim seqs					*/
	/*--------------------------------------------------------------*/	

		/* For each daily clim structure allocate clim seqs for all required & optional clims */
		base_station[0].daily_clim = (struct daily_clim_object *)
			alloc(1*sizeof(struct daily_clim_object), "daily_clim", "construct_netcdf_grid" );
		//duration.day is a long that was passed into construct_ascii as a date struct
		base_station[0].daily_clim[0].tmax = (double *) alloc(duration.day * sizeof(double), "tmax",  "construct_netcdf_grid");
		base_station[0].daily_clim[0].tmin = (double *) alloc(duration.day * sizeof(double), "tmin",  "construct_netcdf_grid");
		base_station[0].daily_clim[0].rain = (double *) alloc(duration.day * sizeof(double), "rain",  "construct_netcdf_grid");
		base_station[0].daily_clim[0].wind = (double *) alloc(duration.day * sizeof(double), "wind",  "construct_netcdf_grid");
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
		base_station[0].daily_clim[0].relative_humidity = NULL;
		base_station[0].daily_clim[0].snow = NULL;
		base_station[0].daily_clim[0].tdewpoint = NULL;
		base_station[0].daily_clim[0].tday = NULL;
		base_station[0].daily_clim[0].tnight = NULL;
		base_station[0].daily_clim[0].tnightmax = NULL;
		base_station[0].daily_clim[0].tavg = NULL;
		base_station[0].daily_clim[0].tsoil = NULL;
		base_station[0].daily_clim[0].vpd = NULL;
		//base_station[0].daily_clim[0].wind = NULL;
		base_station[0].daily_clim[0].ndep_NO3 = NULL;
		base_station[0].daily_clim[0].ndep_NH4 = NULL;
		
		/*Check if any flags are set in the optional clim sequence struct*/
		if ( daily_flags.daytime_rain_duration == 1 ) {
			   base_station[0].daily_clim[0].daytime_rain_duration = (double *) 
			alloc(duration.day * sizeof(double), "day_rain_dur",  "construct_netcdf_grid");

		}
		/*--------------------------------------------------------------*/
		/*	Allocate the yearly clim object.								*/
		/*--------------------------------------------------------------*/
		base_station[0].yearly_clim = (struct yearly_clim_object *)
		alloc(1*sizeof(struct yearly_clim_object),  "yearly_clim",  "construct_netcdf_grid" );
		/*	Initialize non-critical sequences							*/
		base_station[0].yearly_clim[0].temp = NULL;		
		/*--------------------------------------------------------------*/
		/*	Allocate the monthly clim object.							*/	
		/*--------------------------------------------------------------*/
		base_station[0].monthly_clim = (struct monthly_clim_object *)
		alloc(1*sizeof(struct monthly_clim_object),  "monthly_clim",  "construct_netcdf_grid" );
		/*	Initialize non-critical sequences							*/
		base_station[0].monthly_clim[0].temp = NULL;
		/*--------------------------------------------------------------*/
		/*	Allocate the hourly clim object.							*/
		/*--------------------------------------------------------------*/
		base_station[0].hourly_clim = (struct hourly_clim_object *)
		alloc(1*sizeof(struct hourly_clim_object),  "hourly_clim",  "construct_netcdf_grid" );
		/*	Initialize non - critical sequences.						*/
		base_station[0].hourly_clim[0].rain.inx = -999;
		base_station[0].hourly_clim[0].rain_duration.inx = -999;
		
		/* Calculate start day index */
		instartday = get_indays(start_date.year, 
								start_date.month, 
								start_date.day, 
								base_station_ncheader[0].year_start, 
								base_station_ncheader[0].leap_year);
	   
		tempdata = (float *) alloc(duration.day * sizeof(float), "tempdata", "construct_netcdf_grid");
		printf("net_y:%f net_x:%f\n", net_y, net_x);
		printf("tmax filename:%s varname:%s sdist:%f instartday:%d dura:%d\n", base_station_ncheader[0].netcdf_tmax_filename,  base_station_ncheader[0].netcdf_tmax_varname, base_station_ncheader[0].sdist, instartday, duration.day);
		
		/* ------------------ TMAX ------------------ */
		k = get_netcdf_var_timeserias(
									  base_station_ncheader[0].netcdf_tmax_filename, 
									  base_station_ncheader[0].netcdf_tmax_varname, 
									  base_station_ncheader[0].netcdf_y_varname, 
									  base_station_ncheader[0].netcdf_x_varname, 
									  net_y, 
									  net_x, 
									  base_station_ncheader[0].sdist, 
									  instartday, 
									  base_station_ncheader[0].day_offset, 
									  (int)duration.day, 
									  tempdata);
		if (k == -1){
			fprintf(stderr, "Can't locate station data in netcdf for var tmax\n");
			printf("------------------------------\n");
			exit(0);
		}
		for (j = 0; j < duration.day; j++){
			base_station[0].daily_clim[0].tmax[j] = (double)tempdata[j];
			printf("day:%d tmax:%f\n", j, base_station[0].daily_clim[0].tmax[j]);
		}
	
		/* ------------------ TMIN ------------------ */
		k = get_netcdf_var_timeserias(
									  base_station_ncheader[0].netcdf_tmin_filename, 
									  base_station_ncheader[0].netcdf_tmin_varname, 
									  base_station_ncheader[0].netcdf_y_varname, 
									  base_station_ncheader[0].netcdf_x_varname, 
									  net_y, 
									  net_x, 
									  base_station_ncheader[0].sdist, 
									  instartday, 
									  base_station_ncheader[0].day_offset, 
									  duration.day, 
									  tempdata);
		if (k == -1){
			fprintf(stderr, "Can't locate station data in netcdf for var tmin\n");
			printf("------------------------------\n");
			exit(0);
		}
		for(j = 0; j < duration.day; j++){
			base_station[0].daily_clim[0].tmin[j] = (double)tempdata[j];
			printf("day:%d tmin:%f\n", j, base_station[0].daily_clim[0].tmin[j]);
		}
	
		/* ------------------ PRECIP ------------------ */
		k = get_netcdf_var_timeserias(
									  base_station_ncheader[0].netcdf_rain_filename, 
									  base_station_ncheader[0].netcdf_rain_varname, 
									  base_station_ncheader[0].netcdf_y_varname, 
									  base_station_ncheader[0].netcdf_x_varname, 
									  net_y, 
									  net_x, 
									  base_station_ncheader[0].sdist, 
									  instartday, 
									  base_station_ncheader[0].day_offset, 
									  duration.day, 
									  tempdata);
		if (k == -1){
			fprintf(stderr, "Can't locate station data in netcdf for var rain\n");
			printf("------------------------------\n");
			exit(0);
		}
		for(j = 0; j < duration.day; j++){
			base_station[0].daily_clim[0].rain[j] = (double)tempdata[j] * base_station_ncheader[0].precip_mult;
			printf("day:%d rain:%f\n", j, base_station[0].daily_clim[0].rain[j]);
		}
		
		/* T.N, Oct. 2015: include wind data */
		/* ------------------ WIND SPEED ------------------ */
		k = get_netcdf_var_timeserias(
									  base_station_ncheader[0].netcdf_wind_filename, 
									  base_station_ncheader[0].netcdf_wind_varname, 
									  base_station_ncheader[0].netcdf_y_varname, 
									  base_station_ncheader[0].netcdf_x_varname, 
									  net_y, 
									  net_x, 
									  base_station_ncheader[0].sdist, 
									  instartday, 
									  base_station_ncheader[0].day_offset, 
									  duration.day, 
									  tempdata);
		if (k == -1){
			fprintf(stderr, "Can't locate station data in netcdf for var wind\n");
			printf("------------------------------\n");
			exit(0);
		}
		for(j = 0; j < duration.day; j++){
			base_station[0].daily_clim[0].wind[j] = (double)tempdata[j];
			printf("day:%d wind:%f\n", j, base_station[0].daily_clim[0].wind[j]);
		}
		
		/* ------------------ ELEV ------------------ */
		if (base_station_ncheader[0].elevflag == 0) {
			base_station[0].z = zone_z;
		}
		else {
			tempdata = (float *) alloc(1 * sizeof(float), "tempdata", "construct_netcdf_grid");
			k = get_netcdf_var(
							   base_station_ncheader[0].netcdf_elev_filename, 
							   base_station_ncheader[0].netcdf_elev_varname, 
							   base_station_ncheader[0].netcdf_y_varname, 
							   base_station_ncheader[0].netcdf_x_varname, 
							   net_y, 
							   net_x, 
							   base_station_ncheader[0].sdist, 
							   tempdata);
			if (k == -1){
				fprintf(stderr, "Can't locate station data in netcdf for var elev\n");
				printf("------------------------------\n");
				exit(0);
				}
			base_station[0].z = (double)tempdata[0];
		}

		
		free(tempdata);
	
	printf("\nConstruct netcdf cell: END ID=%d x=%lf y=%lf lai=%lf i=%d \n", 
		   base_station[0].ID, 
		   base_station[0].x, 
		   base_station[0].y, 
		   base_station[0].effective_lai, 
		   i);
	
	*num_world_base_stations +=1;
	base_station_ncheader[0].lastID +=1;

	return(base_station);
}
