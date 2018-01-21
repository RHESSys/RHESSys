/*------------------------------------------------------------------------------------------------------------------------------*/
/*                                                                                                                              */
/*                                              rhessys.h                                                                       */
/*                                                                                                                              */
/*      rhessys.h - global scope object definitions and macros                                                                  */
/*                                                                                                                              */
/*      NAME                                                                                                                    */
/*      not executable                                                                                                          */
/*                                                                                                                              */
/*      SYNOPSIS                                                                                                                */
/*      #include "rhessys.h"                                                                                                    */
/*                                                                                                                              */
/*      OPTIONS                                                                                                                 */
/*      none                                                                                                                    */
/*                                                                                                                              */
/*      DESCRIPTION                                                                                                             */
/*                                                                                                                              */
/*      This header represents an amalgamation of header structures                                                             */
/*      macros and dedinitions of dynamicgc and RHESSys2.0.                                                                     */
/*      This file contains the global scope object (variables and                                                               */
/*      structures) definitions for rhessys4.0.                                                                                 */
/*                                                                                                                              */
/*      The objects are structured in the following heirarchy:                                                                  */
/*                                                                                                                              */
/*                                                                                                                              */
/*      .world                                                                                                                  */
/*                                                                                                                              */
/*      .world.base_stations                                                                                                    */
/*                                                                                                                              */
/*      .world.base_stations.yearly_clim                                                                                        */
/*      .world.base_stations.monthly_clim                                                                                       */
/*      .world.base_stations.daily_clim                                                                                         */
/*      .world.base_stations.hourly_clim                                                                                        */
/*                                                                                                                              */
/*      .world.basin                                                                                                            */
/*                                                                                                                              */
/*      .world.basin.grow                                                                                                       */
/*                                                                                                                              */
/*      .world.basin.hillslopes                                                                                                 */
/*                                                                                                                              */
/*      .world.basin.hillslopes.base_stations                                                                                   */
/*      .world.basin.hillslopes.base_stations.clim                                                                              */
/*                                                                                                                              */
/*      .world.basin.hillslopes.grow                                                                                            */
/*                                                                                                                              */
/*      .world.basin.hillslopes.zones                                                                                           */
/*      .world.basin.hillslopes.zones.grow                                                                                      */
/*      .world.basin.hillslopes.zones.patches                                                                                   */
/*      .world.basin.hillslopes.zones.patches.grow                                                                              */
/*      .world.basin.hillslopes.zones.patches.layer                                                                             */
/*                                                                                                                              */
/*      .world.basin.hillslopes.zones.patches.layer.canopy_strata                                                               */
/*      .world.basin.hillslopes.zones.patches.layer.canopy_strata.grow                                                          */
/*                                                                                                                              */
/*      .world.basin_default                                                                                                    */
/*      .world.basin_default.basin_grow_default                                                                                 */
/*      .world.hillslope_default                                                                                                */
/*      .world.hillslope_default.hillslope_grow_default                                                                         */
/*      .world.zone_default                                                                                                     */
/*      .world.zone_default.zone_grow_default                                                                                   */
/*      .world.soil_default                                                                                                     */
/*      .world.soil_default.patch_grow_default                                                                                  */
/*      .world.landuse_default                                                                                                  */
/*      .world.landuse_default.patch_grow_default                                                                               */
/*      .world.stratum_default                                                                                                  */
/*      .world.stratum_default.stratum_grow_default                                                                             */
/*                                                                                                                              */
/*      .command_line                                                                                                           */
/*                                                                                                                              */
/*      .tecfile                                                                                                                */
/*                                                                                                                              */
/*      All of the root objects have their actual type names.                                                                   */
/*      All child objects have been written with their assigned                                                                 */
/*      names specified in their parent object.                                                                                 */
/*      In most cases the assigned name is the plural of the actual                                                             */
/*      name of the object (until we get past the patch level).                                                                 */
/*                                                                                                                              */
/*      PROGRAMMER'S NOTES                                                                                                      */
/*                                                                                                                              */
/*      The object heirarchy replaces old input, biome and state                                                                */
/*      variable structures.  No output objects have been defined.                                                              */
/*      In theory output routines should be defined as members                                                                  */
/*      of a given object  in the heirarchy.  If an accumulator                                                                 */
/*      array is also needed it should be added to the object                                                                   */
/*      at which the level of accumulation is being performed.  It                                                              */
/*      may be useful to use the lower level object in the heirarchy                                                            */
/*      to hold the accumulator values rather than making a custom                                                              */
/*      version.  This will take a bit more memory but will allow                                                               */
/*      member functions designed for the lower level object to be                                                              */
/*      used on the accumulated data as well.                                                                                   */
/*                                                                                                                              */
/*      A full description of the mapping between the source .h files                                                           */
/*      and this one is not given.  However, some points include:                                                               */
/*                                                                                                                              */
/*      1.  A firm spatial hierarchy.                                                                                           */
/*      2.  Vertical hierarchy only represented within a patch.                                                                 */
/*      3.  Base stations definable for any object.                                                                             */
/*      4.  Zones replacing elevation intervals.  Zones have                                                                    */
/*                      homogenous climate forcing above the canopy.                                                            */
/*      5.      The specification of multiple vertical layers per patch.                                                        */
/*      6.  The addition of an active surface layers to each strata.                                                            */
/*                      (The layers can be set to 0 if needed).                                                                 */
/*      7.  The addition of multiple canopy strata (life forms)                                                                 */
/*                      per layer in a patch.                                                                                   */
/*      8.      The use of default files which separate biome file data                                                         */
/*                      in a spatial hierarchy.                                                                                 */
/*      9. The use of a world object which has default model                                                                    */
/*                      control information.  This allows the basin object                                                      */
/*                      to have only data pertaining to processes.                                                              */
/*      10.  Provosion of pointers to a grow object at each level                                                               */
/*                      of the world heirarchy to permit easy addition of                                                       */
/*                      dynamicgc canopy growth simulation.                                                                     */
/*                                                                                                                              */
/*                                                                                                                              */
/*      Jan 8/96 - Original code.                                                                                               */
/*      Jan 15/96 - Added world object .                                                                                        */
/*      March 4/97 - added hillslope defaults - C. Tague                                                                        */
/*                                                                                                                              */
/*      Sep 2 1997      RAF                                                                                                     */
/*      Revision of documentation and objects to eliminate extended                                                             */
/*      object definitions.                                                                                                     */
/*      Added min and max heat flux fractions.                                                                                  */
/*      Changed rain and snow capacities to specific...                                                                         */
/*                                                                                                                              */
/*      Oct 22, 1997 CT                                         */
/*      added patch level water balance variable                */
/*      and preday_unsat_storage, delta_canopy_storage          */
/*      patch variables required for this calculation           */
/*                                                              */
/*      Feb 11, 1998 CLT                                        */
/*      removed obsolete default variables                      */
/*--------------------------------------------------------------*/

#ifndef _RHESSYS_H_
#define _RHESSYS_H_

/*----------------------------------------------------------*/
/*      Declare shared libraries.                           */
/*----------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../util/WMFireInterface.h" /* required for fire spread*/
/*----------------------------------------------------------*/
/*      Define macros.                                      */
/*----------------------------------------------------------*/
#define FILEPATH_LEN 1024
#define TEC_CMD_LEN 256
#define NULLVAL -9999   
#define TRUE    1
#define FALSE   0
#define N_OUTFIELDS     17
//#define MAXSTR        100
#define MAXSTR  1024
#define DtoR      0.01745329    
#define PI      3.14159265359
#define SECONDS_PER_DAY 86400
#define ess_snow        0.97
#define ess_soil        0.95
#define ess_veg         0.98
#define ess_water       0.97
#define PRE     "result"
#define state_output_filename   "world_state"
#define MAXNAME 60
#define INTERVAL_SIZE 0.001 
#define MAX_NUM_INTERVAL 5000 
#define STREAM 1
#define ROAD 2
#define NON_VEG 20
#define TREE 1
#define GRASS 2
#define C4GRASS 3
#define LITTER 3
#define DECID 1
#define EVERGREEN 0
#define STATIC 0
#define DYNAMIC 2
#define WARING 2
#define CONSTANT 1
#define DICKENSON 3
#define COMBINED 4
#define ZERO 0.000000001
#define URBANID         100
#define M 0
#define K 1
#define PA 0
#define PO 1
#define SOIL_DEPTH 2
#define SATDEF 0
#define STREAMFLOW 1
#define PTHRESH 0
#define PTYPELOW 1
#define P1LOW 2
#define P2LOW 3
#define PTYPEHIGH 4
#define P1HIGH 5
#define P2HIGH 6


/*----------------------------------------------------------*/
/*      Define min and max macros                           */
/*----------------------------------------------------------*/
#define max(a,b)    ((a) > (b) ? (a) : (b))
#define min(a,b)    ((a) < (b) ? (a) : (b))

#ifdef LIU_NETCDF_READER
int is_approximately(const double value,const double target,const double tolerance);
#endif
int read_record( FILE *, char *);
#ifdef LIU_NETCDF_READER
int get_netcdf_station_number(char *base_station_filename);                      /*160419LML*/
/*160624LML moved here*/
int get_netcdf_var_timeserias(char *, char *, char *, char *, float, float, float, int, int, int, int, float *);
int get_netcdf_xy(char *, char *, char *, float, float, float, float *, float *);
int get_netcdf_var(char *, char *, char *, char *, float, float, float, float *);
int get_indays(int,int,int,int,int);	//get days since XXXX-01-01
#endif

/*----------------------------------------------------------*/
/*      Define types                                        */
/*----------------------------------------------------------*/
typedef short bool;
static const short true = 1;
static const short false = 0;

/*----------------------------------------------------------*/
/*      Define a calendar date object.                      */
/*----------------------------------------------------------*/
struct date
        {
        long    year;
        long    month;
        long    day;
        long    hour;
        };

/*----------------------------------------------------------*/
/*      Define default object.                              */      
/*----------------------------------------------------------*/

struct  default_object
        {
        int             num_basin_default_files;
        struct  basin_default   *basin;
        int             num_hillslope_default_files;
        struct  hillslope_default       *hillslope;
        int             num_zone_default_files;
        struct  zone_default            *zone;
        int             num_soil_default_files;
        struct  soil_default            *soil;
        int             num_landuse_default_files;
        struct  landuse_default         *landuse;
        int             num_stratum_default_files;
        struct  stratum_default         *stratum;
        int             num_fire_default_files;
        struct  fire_default            *fire;
        int             num_surface_energy_default_files;
        struct  surface_energy_default          *surface_energy;
        int             num_spinup_default_files;
        struct  spinup_default            *spinup;
        };


/*----------------------------------------------------------*/
/*      Define the world object.                            */
/*----------------------------------------------------------*/
struct world_object 
        {
        int             num_base_stations;
        // Since using gridded data will only have a single base station file but
        // mutliple base stations, the new num_base_stations_files is necessary for
        // proper output of a world file when using gridded climate data.                       
        int             num_base_station_files;
        int             num_basin_files;
        int             simulation_cycles;      
        int             year_day;
        int             ID;    
        int             num_fire_grid_row;
        int             num_fire_grid_col;
        int             target_status;
        long    num_years;
        long    num_days;
        long    num_hours;
        char    **base_station_files;
        char    **basin_files;
        char    **basin_default_files;
        char    **hillslope_default_files;
        char    **zone_default_files;
        char    **soil_default_files;
        char    **landuse_default_files;
        char    **stratum_default_files;
        char    **fire_default_files;
        char    **surface_energy_default_files;
        char    **spinup_default_files;
        char    **spinup_thresholds_files;  
        double  declin;                 /*      rads    */
        double  cos_declin;             /*      DIM     */
        double  sin_declin;             /*      DIM     */
        double  Io;                     /* Wm-2         */
        struct  base_station_object     **base_stations;
        struct  base_station_ncheader_object    *base_station_ncheader;
        struct  basin_object            **basins;
        struct  date                    start_date;                             
        struct  date                    end_date;                               
        struct  date                    duration;                               
        struct  default_object          *defaults;
        struct  world_hourly_object     *hourly;
        struct  fire_object             **fire_grid;
	struct patch_fire_object **patch_fire_grid;  //mk
        struct  spinup_thresholds_list_object  *spinup_thresholds ;   
	struct  date			**master_hourly_date;	
        };


/*----------------------------------------------------------*/
/*      Define the world hourly parameter structure.        */
/*----------------------------------------------------------*/
struct  world_hourly_object
        {
        int             i;
        };

/*----------------------------------------------------------*/
/*      Define the world output files object.               */
/*----------------------------------------------------------*/
struct  world_output_file_object
        {
        struct  output_files_object             *basin;
        struct  output_files_object             *hillslope;
        struct  output_files_object             *zone;
        struct  output_files_object             *patch;
        struct  output_files_object             *canopy_stratum;
        struct  output_files_object             *shadow_strata;
        struct  output_files_object             *stream_routing;
        };

/*----------------------------------------------------------*/
/*      Defin the generic output files object               */
/*----------------------------------------------------------*/
struct  output_files_object
        {
        FILE    *yearly;
        FILE    *monthly;
        FILE    *daily;
        FILE    *hourly;
        };

/*----------------------------------------------------------*/
/*      accumlator variables for patch/basin_object         */
/*----------------------------------------------------------*/
struct accumulate_patch_object
{
   double length;
   int meltday;
   int peaksweday;
   int wyd;
   int rec_wyd;
   int rec_pet_wyd;
   int max_pet_wyd;
   int  num_threshold;
   int ndays_sat;
   int ndays_sat70;
   int midsm_wyd;
   double burn;
   double et;
   double trans;
   double day7trans;
   double day7pet;
   double maxtrans;
   double maxpet;
   double PET;
   double lai;
   double snowpack;
   double sm_deficit;
   double stream_DOC;
   double stream_DON;
   double stream_NO3;
   double stream_NH4;
   double streamflow;
   double leach;
   double denitrif;
   double nitrif;
   double psn;
   double DOC_loss;
   double DON_loss;
   double theta;
   double uptake;
   double mineralized;
   double pcp;
   double potential_recharge;
   double potential_recharge_wyd;
   double recharge;
   double recharge_wyd;
   double snowin;
   double Qin_total;
   double Qout_total;
};


/*----------------------------------------------------------*/
/*      rooting zone object for patch                       */      
/*----------------------------------------------------------*/
struct rooting_zone_object
{
        double field_capacity;
        double potential_sat;
        double sat;
        double unsat;
        double depth;
        double S;
        double T;               /* degrees C */

};

/*----------------------------------------------------------*/
/*      Define routing list object.                         */
/*----------------------------------------------------------*/
struct routing_list_object 
        {
        int num_patches;
        struct patch_object **list;
        };
/*----------------------------------------------------------*/
/*      Define spinup threshold list object.                */
/*----------------------------------------------------------*/
struct spinup_thresholds_list_object 
        {
        int num_stratum;
        struct stratum_object **list;
        };
/*----------------------------------------------------------*/
/*      Define reservoir object.                            */
/*----------------------------------------------------------*/

struct reservoir_object
{
int reservoir_ID;
int flag_min_flow_storage;/*If min_flow has higher priority than min_storage=0, else=1 */
double month_max_storage[12]; /* m3 */
double min_storage; /* m3 */
double min_outflow; /* m3/day */
double initial_storage; /* m3 */
};


/*----------------------------------------------------------*/
/*      Define stream  routing list object.                 */
/*----------------------------------------------------------*/

struct stream_network_object
{
int reach_ID;
int num_lateral_inputs;
int num_downstream_neighbours;
int num_upstream_neighbours;
int num_neighbour_hills;
int *downstream_neighbours;
int *upstream_neighbours;
int reservoir_ID;
struct reservoir_object reservoir;
struct patch_object **lateral_inputs;
struct hillslope_object **neighbour_hill;
double length; /* m */
double manning;
double bottom_width; /* m */
double top_width; /* m */
double max_height; /* m */
double stream_slope;
double initial_flow; /* m3/s */
double water_depth; /* m */
double previous_Qin; /* m3/s */
double Qin; /* m3/s */
double previous_lateral_input; /* m2/s */
double Qout; /* m3/s */
};

struct stream_list_object
        {
        int num_reaches;
        double streamflow;
        struct stream_network_object *stream_network;
        };

/*----------------------------------------------------------*/
/*	Define a snowpack object.								*/
/*----------------------------------------------------------*/

struct	snowpack_object
	{
	double  APAR_direct;                   	/* umol/(m2*day)*/
	double  APAR_diffuse;                   /* umol(m2*day) */
	double	energy_deficit;			/* degree days	*/
	double  evaporation;		        /* m water	*/
	double	ga;				/* m/s          */
	double	gap_fraction;			/* unitless 	*/
	double	height;				/* meters   	*/
	double  Kstar_direct;                   /* Kj/(m2*day)  */
	double  Kstar_diffuse;                  /* Kj/(m2*day)  */
	double	K_reflectance;			/* unitless 0-1	*/
	double	K_absorptance;			/* unitless 0-1 */
	double	PAR_reflectance;		/* unitless 0-1	*/
	double	PAR_absorptance;		/* unitless 0-1 */
	double	surface_age;			/* days		*/
	double	sublimation;			/* m water	*/
	double  T; 		                /*  degrees C  	*/
	double	water_depth;			/* m water	*/
	double  water_equivalent_depth;		/* m water   	*/
	double	overstory_fraction;		/* percent	*/
	double	overstory_height;		/* m		*/
	double Rnet;	/* net SW + LW radiation in KJ/m2/d */
	double Q_LE;     /* latent heat exchange with atmos in KJ/m2/d */
	double Q_H;     /* sensible heat flux in KJ/m2/d */
	double Q_rain;   /* rain advective heat flux in KJ/m2/d */
	double Q_melt;  /* net input for melt in KJ/m2/d */
	};


/*----------------------------------------------------------*/
/*      Define basin object.                                */
/*----------------------------------------------------------*/
struct basin_object
        {
        int             ID;                                                                     
        int             num_base_stations;
        int             num_hillslopes;
        double  area;                   /*  m2          */
	      double  area_withsnow;			/*  m2 		*/
        double  x;                      /*  meters      */      
        double  y;                      /*  meters      */
        double  z;                      /*  meters      */
        double  cos_latitude;           /*      DIM     */
        double  daylength;              /*      s       */
        double  latitude;               /* decimal degrees      */
        double  theta_noon;             /*      rads    */
        double  sin_latitude;           /*      DIM     */
        double  max_slope;              /*      degrees */
        
        /*      used in subsurface computation          */
        double basin_outflow;
        double basin_rz_storage;
        double basin_unsat_storage;
        double basin_sat_deficit;
        double basin_return_flow;
        double basin_detention_store;
        double basin_area;
        double preday_basin_unsat_storage;
        double preday_basin_rz_storage;
        double preday_basin_sat_deficit;
        double preday_sat_deficit;
        double preday_basin_return_flow;
        double preday_basin_detention_store;
        /*                                              */
        
        struct  base_station_object     **base_stations;
        struct  basin_default           **defaults;
        struct  basin_hourly_object     *hourly;
        struct  grow_basin_object       *grow;
        struct  hillslope_object        **hillslopes;
        struct  patch_object            *outside_region;
        struct  stream_list_object      stream_list;
        struct  routing_list_object     *route_list;
        struct  routing_list_object *surface_route_list;
        struct  accumulate_patch_object acc_month;
        struct  accumulate_patch_object acc_year;
        struct  snowpack_object snowpack;
        };

/*----------------------------------------------------------*/
/*      Define the basin hourly parameter structure.                    */
/*----------------------------------------------------------*/
struct  basin_hourly_object
        {
        double  cos_declin_cos_hourangle;       /*      DIM     */
        double  cos_declin_sin_hourangle;       /*      DIM     */
        double  cos_hour_angle;                 /*      DIM     */      
        double  cos_sza;                        /*      DIM     */
        double  hour_angle;                     /* rads         */      
        double  sin_hour_angle;                 /*      DIM     */      
        double  optical_air_mass;               /*      DIM     */
        };

/*----------------------------------------------------------*/
/*      Define grow_basin_object extension.                             */
/*----------------------------------------------------------*/
struct  grow_basin_object
        {
        int             temp;           /* degrees C    */
        };

/*----------------------------------------------------------*/
/*      Define basin default object.                                            */      
/*----------------------------------------------------------*/
struct basin_default
        {
        int                     ID;
        int             n_routing_timesteps;    /* number per day */ 
        struct          basin_grow_default      *grow_defaults;
        };

/*----------------------------------------------------------*/
/*      Define a base station object.                                                   */
/*----------------------------------------------------------*/
typedef struct base_station_object
        {
        int             ID;
        FILE    *base_station_file;
        #ifdef LIU_NETCDF_READER
        double lon;
        double lat;
        #endif
        double  proj_x;                                                          /* (meters) x coordinate of projection     */
        double  proj_y;                                                          /* (meters) y coordinate of projection     */
        double  z;                                                               /*   meters     */
        double  effective_lai;                                                   /* m^2/m^2      */
        double  screen_height;                                                   /* meters       */
        struct  hourly_clim_object       *hourly_clim;
        struct  daily_clim_object        *daily_clim;
        struct  monthly_clim_object      *monthly_clim;
        struct  yearly_clim_object       *yearly_clim;
        struct  dated_input_object       *dated_input;
        } base_station_object;
/*----------------------------------------------------------*/
/*      Define a netcdf base station header object.                                                     */
/*----------------------------------------------------------*/
typedef struct base_station_ncheader_object
{
        #ifndef LIU_NETCDF_READER
        FILE    *base_station_file;
        double  effective_lai;                  /* m^2/m^2      */
        double  screen_height;                  /* meters       */
        #endif
        int             lastID;
        //160517LML double  sdist;                                  /* search distance in native netcdf units */
        double          resolution_dd;                                           /*(DD) 160517LML resolution in geocoordinate. Estimated from grid cell distribution*/
        double          resolution_meter;                                        /*(meter) 160517LML resolution in projected coordinate. Estimated from grid cell distribution*/
        int             year_start;                             /* start year for netcdf time counter (NOT time series start date) */
        int             day_offset;                             /* day offset from January 1 for netcdf time counter */
        int             leap_year;                              /* 0 = no leap years, 1 = leap years in netcdf record */
        double  precip_mult;                    /* multiplier for precip if not in meters */
        char    temperature_unit;               /* 160517LML K: Kelvin C: Celsius */
        int             elevflag;                               /* set based on whether elev filename is given */
        char    netcdf_x_varname[MAXSTR];       /* variable name for x coordinate in nc file */
        char    netcdf_y_varname[MAXSTR];       /* variable name for y coordinate in nc file */
        char    netcdf_tmax_filename[MAXSTR];   /* filename for tmax nc file */
        char    netcdf_tmin_filename[MAXSTR];   /* filename for tmin nc file */
        char    netcdf_rain_filename[MAXSTR];   /* filename for rain nc file */
        char    netcdf_elev_filename[MAXSTR];   /* filename for elev nc file */
        char    netcdf_tmax_varname[MAXSTR];    /* variable name for tmax in nc file */
        char    netcdf_tmin_varname[MAXSTR];    /* variable name for tmin in nc file */
        char    netcdf_rain_varname[MAXSTR];    /* variable name for rain in nc file */
        char    netcdf_elev_varname[MAXSTR];    /* variable name for elev in nc file */
#ifdef LIU_EXTEND_CLIM_VAR
        double  rhum_mult;                    /* multiplier for relative humidity to 0-1 */
        char    netcdf_huss_filename[MAXSTR];   /* filename for specific humidity nc file */
        char    netcdf_huss_varname[MAXSTR];    /* variable name for specific humidity in nc file */
        char    netcdf_rmax_filename[MAXSTR];   /* filename for relative humidity max nc file */
        char    netcdf_rmax_varname[MAXSTR];    /* variable name for relative humidity max in nc file */
        char    netcdf_rmin_filename[MAXSTR];   /* filename for relative humidity min nc file */
        char    netcdf_rmin_varname[MAXSTR];    /* variable name for relative humidity min in nc file */
        char    netcdf_rsds_filename[MAXSTR];   /* filename for shortwave radiation nc file */
        char    netcdf_rsds_varname[MAXSTR];    /* variable name for shortwave radiation in nc file */
        char    netcdf_was_filename[MAXSTR];   /* filename for wind speed nc file */
        char    netcdf_was_varname[MAXSTR];    /* variable name for wind speed in nc file */
#endif
} base_station_ncheader_object;
/*----------------------------------------------------------*/
/*      Define dated climate sequence                       */
/*----------------------------------------------------------*/
struct  dated_sequence
        {
        struct  date    edate;
        double  value;
        };

struct  clim_event_sequence
        {
        int inx;
        struct dated_sequence *seq;
        };

/*----------------------------------------------------------*/
/*      Define base station annual climate record .                             */
/*----------------------------------------------------------*/
struct  yearly_clim_object
        {
        double  *temp;
        };

/*----------------------------------------------------------*/
/*      Define base station monthly climate record .                    */
/*----------------------------------------------------------*/
struct  monthly_clim_object
        {
        double  *temp;
        };

/*----------------------------------------------------------*/
/*      Define base station dated input record .                        */
/*----------------------------------------------------------*/
struct  dated_input_object
        {
        struct clim_event_sequence  fertilizer_NO3;                                     /* kg/m2/day    */
        struct clim_event_sequence  fertilizer_NH4;                                     /* kg/m2/day    */
        struct clim_event_sequence  irrigation;                                 /* m/day        */
        struct clim_event_sequence  snow_melt_input;                                 /* m/day        */
        struct clim_event_sequence  biomass_removal_percent;                           /* 0-1        */
        struct clim_event_sequence  pspread;   	 	   	                    /* 0-1        */
        struct clim_event_sequence  PH;                                 /* DIM  */
        struct clim_event_sequence  grazing_Closs;                                      /* kg/m2/day    */
        };

/*----------------------------------------------------------*/
/*      Define base station hourly  climate record .                    */
/*----------------------------------------------------------*/
struct  hourly_clim_object
        {
        struct clim_event_sequence rain;
        struct clim_event_sequence rain_duration;
        };

/*----------------------------------------------------------*/
/*      Define base station daily climate record .                              */
/*----------------------------------------------------------*/
struct  daily_clim_object
        {

/*----------------------------------------------------------*/
/*       Critical data.                                                                                 */
/*----------------------------------------------------------*/
        double  *tmax;                  /*   degrees C  */
        double  *tmin;                  /*   degrees C  */
        double  *rain;                  /*   m   water  */

/*----------------------------------------------------------*/
/*       Non - Critical data.                                                                   */
/*----------------------------------------------------------*/
        double  *atm_trans;             /*      0 - 1           */
        double  *CO2;                   /* ppm */
        double  *base_station_effective_lai;    /*      m^2/m^2         */
        double  *cloud_fraction;        /*      0 - 1           */              
        double  *cloud_opacity;         /*      0 - 1           */              
        double  *dayl;                  /* seconds / day */
        double  *daytime_rain_duration;         /* hours/day    */
        double  *Delta_T;               /*      degrees C / day         */
        double  *lapse_rate_precip;               /*      m / m           */
        double  *lapse_rate_tmin;               /*      degrees C / m           */
        double  *lapse_rate_tmax;               /*      degrees C / m           */
        double  *dewpoint;                      /*      degrees C       */
        double  *Kdown_diffuse;                 /* kJ/(m2*day)  */
        double  *Kdown_direct;                  /* kJ/(m2*day) */
        double  *LAI_scalar;                    /* unitless     */
        double  *Ldown;                         /* kJ/(m2*day)  */
        double  *ndep_NO3;                              /* kgN/(m2*day) */
        double  *ndep_NH4;                              /* kgN/(m2*day) */
        double  *surface_Tday;                  /*      deg C   */
        double  *surface_Tnight;                /*      deg C   */
        double  *PAR_diffuse;                   /*      molm-2day-1             */
        double  *PAR_direct;                    /*      molm-2day-1             */
        double  *relative_humidity;             /*      0 - 1 ; input 0 - 100   */
        double  *snow;                          /*      mm      */      
        double  *tdewpoint;                     /*   degrees C  */
        double  *tday;                          /*      degrees C       */
        double  *tnight;                        /*      degrees C       */
        double  *tnightmax;                     /*      degrees C       */
        double  *tavg;                          /*      degrees C       */
        double  *tsoil;                         /*      degrees C       */
        double  *vpd;                           /*      Pa              */
        double  *wind;                          /*      m/s             */
        double  *wind_direction;                /*      degrees         */
#ifdef LIU_EXTEND_CLIM_VAR
        double  *relative_humidity_max;         /*      0 - 1                 */
        double  *relative_humidity_min;         /*      0 - 1                 */
        double  *surface_shortwave_rad;         /*      W m-2  daily average  */
        double  *specific_humidity;             /*      kg/kg  daily average  */
#endif
        };    
        

/*----------------------------------------------------------*/
/*      Define hillslope default object.                                                */      
/*----------------------------------------------------------*/
struct hillslope_default
        {
        int                     ID;
        int     n_routing_timesteps;            /*  number per day */
        double  gw_loss_coeff;                                  /* percent/day */
        double  gw_loss_fast_coeff;                                     /* percent/day */
        double  gw_loss_fast_threshold;                                 /* m */
        };

/*----------------------------------------------------------*/
/*      Define hillslope object to aggregate patch .    */      
/*      default soil characteristics                            */
/*----------------------------------------------------------*/
struct  aggdefs_object
        {
        double  m;                              /* DIM  */
        double  Ksat_0;                         /* m/day */
        double  porosity_0;                             /* unitless */
        double  psi_air_entry;                  /* m */
        double  porosity_decay;                                         /* m^-1 */
        double  soil_depth;                                     /* m */
        double  soil_water_cap;                                 /* m */
        double  active_zone_z;                                  /* m */
        double  N_decay_rate;                                   /* kg N /m */
        double  NO3_adsorption_rate;                            /* kg /kg soil */
        double  NH4_adsorption_rate;                            /* kg /kg soil */
        double  DOM_decay_rate;                                 /* kg N /m */
        double  DON_adsorption_rate;                            /* kg /kg soil */
        double  DOC_adsorption_rate;                            /* kg /kg soil */
        };
/*----------------------------------------------------------*/
/*      Define a deep groundwater object                        */
/*----------------------------------------------------------*/

struct  gw_object
        {
        double  storage;                /* m (water) */
        double  NO3;            /* kgN/m2       */
        double  NH4;            /* kgN/m2       */
        double  DOC;            /* kgC/m2       */
        double  DON;            /* kgN/m2       */
        double  Qout;                   /* m/m2/day     */
        double  NO3out;                 /* kgN/m2/day   */
        double  NH4out;                 /* kgN/m2/day   */
        double  DONout;                 /* kgN/m2/day   */
        double  DOCout;                 /* kgC/m2/day   */
        double  hourly_Qout;            /* m/m2/day     */
        double  hourly_NO3out;          /* kgN/m2/day   */
        double  hourly_NH4out;          /* kgN/m2/day   */
        double  hourly_DONout;          /* kgN/m2/day   */
        double  hourly_DOCout;          /* kgC/m2/day   */

        };
/*----------------------------------------------------------*/
/*      Define a hillslope object.                                                              */      
/*----------------------------------------------------------*/
struct hillslope_object
        {
        int             basin_ID;
        int             ID;
        int             num_base_stations;
        int             num_zones;
        double  x;                      /*      meters  */
        double  y;                      /*      meters  */
        double  z;                      /* meters       */
        double  area;                   /* sq meters */
        double  riparian_area;          /* sq meters */
        double  slope;                  /* degrees */
        double  base_flow;              /* meters               */
        double  hourly_base_flow;       /* meters   */
        double  streamflow_NO3;         /* kgN/m2/day           */
        double  streamflow_NH4;         /* kgN/m2/day           */
        double  streamflow_DON;         /* kgN/m2/day           */
        double  streamflow_DOC;         /* kgC/m2/day           */
        double  hourly_streamflow_NO3;  /* kgN/m2/day           */
        double  hourly_streamflow_NH4;  /* kgN/m2/day           */
        double  hourly_streamflow_DON;  /* kgN/m2/day           */
        double  hourly_streamflow_DOC;  /* kgN/m2/day           */
        struct  gw_object               gw;
        struct  aggdefs_object          aggdefs;
        struct  base_station_object     **base_stations;
        struct  grow_hillslope_object   *grow;
        struct  hillslope_default       **defaults;
        struct  hillslope_hourly_object *hourly;
        struct  routing_list_object     routing_order;
        struct  zone_object             **zones;
        struct  accumulate_patch_object acc_month;
        struct  accumulate_patch_object acc_year;
        };

/*----------------------------------------------------------*/
/*      Define the hillslope hourly parameter structure.                */
/*----------------------------------------------------------*/
struct  hillslope_hourly_object
        {
        int     i;
        };

/*----------------------------------------------------------*/
/*      Define grow_hillslope_object extension.                                 */
/*----------------------------------------------------------*/
struct  grow_hillslope_object
        {
        int             temp;
        };


/* daily values that are passed to daily model subroutines */
struct metvar_struct
{
        double abshd;          /* (kg/m3) absolute humidity deficit */
        double dayl;           /* (s)     daylength */
        double dayl_ravg;      /* (s) 21 day running average of daylength */
        double pa;             /* (Pa)    atmospheric pressure */
        double ppfd;           /* (umol/m2/s) PAR photon flux density */
        double prcp;           /* (m) precipitation                     */
        double rootup_temp;    /* temperature multiplier for root N uptake */
        double swavgfd;        /* (W/m2)  daylight average shortwave flux */
        double swabs;          /* (W/m2)  canopy absorbed shortwave flux */
        double swtrans;        /* (W/m2)  transmitted shortwave flux */
        double temp_mult;      /* (DIM) runningsavg temperature for decomp (NEW) */
        double tmax;           /* (deg C) daily maximum air temperature */
        double tmin;           /* (deg C) daily minimum air temperature */
        double tmin_ravg;      /* (def C) 21 day running avg of daily min air temperature */
        double tavg;           /* (deg C) daily average air temperature */
        double tday;           /* (deg C) daylight average air temperature */
        double tnight;         /* (deg C) nightime average air temperature */
        double tnightmax;      /* (deg C) nighttime maximum air temperature */
        double tsoil;          /* (deg C) daily soil temperature, avg, top 10 cm */
        double tsoil_avg;      /* avg soil temp over the branching period */
        double tsoil_sum;      /* (deg C) daily summation of soil temp     (NEW) */
        double tnight_max;     /* (deg C) nighttime max average air temperature */
        double vpd;            /* (Pa)    vapor pressure deficit */
        double vpd_ravg;       /* (Pa) 21 day running average of vpd */
        double vpd_day;		   /* (Pa)    vapor pressure deficit during day time hours */
        double vpd_night;		   /* (Pa)    vapor pressure deficit during night time hours */
};

/*----------------------------------------------------------*/
/*      Define a zone default           object.                                         */
/*----------------------------------------------------------*/
struct  zone_default
        { 
        int             ID;
        double  atm_trans_lapse_rate;   /* 1/m  */
        double  dewpoint_lapse_rate;    /*      degrees C / m           */
        double  lapse_rate;             /* Celcius degrees/m    */      
        double  wet_lapse_rate;         /* Celcius degrees/m    */      
        double  lapse_rate_precip_default;              /* k by m       */      
        double  lapse_rate_tmin;                /* Celcius degrees/m    */      
        double  lapse_rate_tmax;                /* Celcius degrees/m    */      
        double  max_effective_lai;      /* m^2/m^2      */
        double  pptmin;                 /*      m       */
        double  sea_level_clear_sky_trans;      /* 0-1  */
        double  temcf;                  /*      DIM     */
        double  trans_coeff1;           /*      DIM     */
        double  trans_coeff2;           /*      DIM     */
        double  trans_coeff1_sum;               /*      DIM     */
        double  trans_coeff2_sum;               /*      DIM     */
        int             trans_startmonth_sum;           /*      month 1-12      */
        double  trans_coeff1_win;               /*      DIM     */
        double  trans_coeff2_win;               /*      DIM     */
        int             trans_startmonth_win;           /*      month 1-12      */
        double  wind;                   /* m/s          */
        double  wind_direction;                 /* degrees              */
        double  max_snow_temp;                                          /* degrees C */
        double  min_rain_temp;                                          /* degrees C */
        double  ndep_NO3;               /* kgN/m2/day   */
        double  atm_CO2;                /* ppm */ 
        double  psen[7];
        double  ravg_days; /* (days) number of days for running average */
        struct  zone_grow_default       *grow_defaults;
        };

/*----------------------------------------------------------*/
/*      accumlator variables for zone_object                    */
/*----------------------------------------------------------*/

struct accumulate_zone_object
{
   int  length;
   double K_direct;
   double K_diffuse;
   double precip;
   double tmax;
   double tmin;
};

/*----------------------------------------------------------*/
/*      Define a microclimate zone object.                                              */
/*----------------------------------------------------------*/
struct zone_object
        {
        int             hillslope_ID;
        int             ID;
        int             daylength_flag;                     /*  0 or 1 */
        int             hourly_rain_flag;                   /*  0 or 1 */
        int             Kdown_diffuse_flag;                 /*  0 or 1  */
        int             Kdown_direct_flag;                  /*  0 or 1  */
        int             num_base_stations;                              
        int             num_patches;
        double  x;                                      /* meters       */
        double  y;                                      /* meters       */
        double  z;                                      /* meters       */
        double  area;                                   /*      m2      */
        double  aspect;                                 /* degrees      */
        double  atm_trans;                                  /* 0 - 1    */
        double  base_station_effective_lai;             /* m^2 / m^2    */
        double  cloud;                          /* 0 - 1        */
        double  cloud_fraction;                         /* 0 - 1        */
        double  cloud_opacity;                          /* 0 - 1        */
        double  CO2;                                            /* ppm  */
        double  cos_aspect;                             /*      DIM     */
        double  cos_slope;                              /*      DIM     */
        double  rain_duration;                  		/* hours/day    */
        double  Delta_T;                                /* C degrees    */
        double  e_dewpoint;                             /* Pa           */
        double  e_horizon;      /* cos of angle to normal of flat       */
        double  effective_lai;          /* area wt. average m^2/m^2     */      
        double  Kdown_diffuse;                          /* Kj/(m^2*day) */
        double  Kdown_diffuse_calc;                             /* Kj/(m^2*day) */
        double  Kdown_diffuse_adjustment;               /*  0-1 */
        double  Kdown_diffuse_flat;                     /* Kj/(m^2*day) */
        double  Kdown_diffuse_flat_calc;                        /* Kj/(m^2*day) */
        double  Kdown_direct;                           /* Kj/(m^2*day) */
        double  Kdown_direct_calc;                              /* Kj/(m^2*day) */
        double  Kdown_direct_adjustment;                /*  0-1 */
        double  Kdown_direct_flat;                      /* Kj/(m^2*day) */
        double  Kdown_direct_flat_calc;                 /* Kj/(m^2*day) */
        double  LAI_temp_adjustment;                    /* 0 - 1        */
        double  LAI_scalar;                             /* DIM          */
        double  Ldown;                                  /* W/m2         */
        double  Ldown_night;							/* W/m2			*/
        double  Ldown_day;								/* W/m2			*/
        double  ndep_NO3;                               /* kg/m2/day    */
        double  ndep_NH4;                               /* kg/m2/day    */
        double  PAR_direct;                             /* umol(m^2*day)        */
        double  PAR_diffuse;                            /* umol(m^2*day)        */
        double  precip_lapse_rate;                      /* m water /m/m */
        double  radrat;                                 /* ratio        */
        double  rain;                                   /* m water      */
        double  rain_hourly_total;                      /* m water      */
        double  relative_humidity;                      /*  0 - 1       */
        double  sin_aspect;                             /*      DIM     */
        double  sin_slope;                              /*      DIM     */
        double  slope;                                  /* degrees      */
        double  snow;                                   /* m water      */
        double  snow_hourly_total;
        double  surface_Tday;                           /*      deg C   */
        double  surface_Tnight;                         /*      deg C   */
        double  tdewpoint;                              /* degrees C    */
        double  vpd_max;                                /* Pa           */
        double  w_horizon;      /* cos of angle to normal of flat       */
        double  wind;                                   /* m/s          */
        double  wind_direction;                                 /* degrees      */
        struct  base_station_object     **base_stations;
        struct  grow_zone_object        *grow;
        struct  metvar_struct           metv;
        struct  patch_object            **patches;
        struct  zone_default            **defaults;
        struct  zone_hourly_object      *hourly;
        struct  accumulate_zone_object  acc_month;
        struct  accumulate_zone_object  acc_year;

        };

/*----------------------------------------------------------*/
/*      Define the zone hourly parameter structure.                             */
/*----------------------------------------------------------*/
struct  zone_hourly_object
        {
        double  rain;                                   /*      m       */
        double  rain_duration;                          /*      s       */
        double  cos_aspect;                             /*      DIM     */
        double  cos_beam_slope;                         /*      DIM     */
        double  direct_attenuation;                     /*      0 - 1   */
        double  Kdown_BOA;                              /*      W/m^2   */
        double  Kdown_direct;                           /*      W/m^2   */
        double  Kdown_direct_flat;                      /*      W/m^2   */
        double  Kdown_diffuse;                          /*      W/m^2   */
        double  Kdown_diffuse_flat;                     /*      W/m^2   */
        double  sin_aspect;                             /*      DIM     */
        double  sin_slope;                              /*      DIM     */
        double  snow;                                   /*      m       */
        };


/*----------------------------------------------------------*/
/*      Define grow_zone_object extension.                                              */
/*----------------------------------------------------------*/
struct  grow_zone_object
        {
        int             temp;
        };

struct soil_class
        {
        double sand;    /* 0 to 1 */
        double silt;    /* 0 to 1 */
        double clay;    /* 0 to 1 */
        };
/*----------------------------------------------------------*/
/*      Define a land use       default object.                                         */
/*----------------------------------------------------------*/
struct  landuse_default 
{
        int ID;
        double  fertilizer_NO3;                                 /* kg/m2/day    */
        double  fertilizer_NH4;                                 /* kg/m2/day    */
        double  irrigation;                                     /* m/day        */
        double  PH;                                     /* DIM  */
        double  septic_NO3_load;                                        /* kg/m2/day */
        double  septic_water_load;                                      /* m/m2/day */
        double  detention_store_size;                   /* m */
        double  lai_cut;                                /* m2/m2 */
        double  percent_impervious;                     /* 0-1 */
        double  grazing_Closs;                  /* kgC/m2/day */
};
/*----------------------------------------------------------*/
/*	Define an soil 	default object.						*/
/*----------------------------------------------------------*/
struct	soil_default
	{
	int		ID;								
	int	theta_psi_curve;				/* unitless */
	double	albedo;						/* 0 to 1   */
	double	interval_size;					/* m */
	double	Ksat_0;						/* meters/day */
	double	Ksat_0_v;					/* meters/day */
	double	m;						/* m^-1	*/
	double	m_v;						/* m^-1	*/
	double	m_z;						/* m^-1	*/
	double	mz_v;						/* m^-1	*/
	double	porosity_0;						/* unitless */
	double	porosity_decay;						/* m^-1 */
	double	p3;						/* unitless */
	double	p4;						/* unitless */
	double	pore_size_index;				/* unitless */
	double	psi_air_entry;					/* m */
	double	psi_max;					/* m */
	double	sat_to_gw_coeff;				/* percent/day */
	double	soil_depth;					/* m */
	double	effective_soil_depth;					/* m */
	double	soil_water_cap;					/* m of water */
	double	deltaz;						/* m */
	double	min_heat_capacity;				/* J/m3/K */
	double	detention_store_size;				/* m water */
	double	max_heat_capacity;				/* J/m3/K */
	double	maximum_snow_energy_deficit;			/* degree days */
	double  snow_water_capacity;				/* m */
	double  snow_light_ext_coef;				/* (DIM) radiation extinction */
	double  snow_melt_Tcoef;				/* unitless */
	double	fs_spill;					/* multiplier*/
	double	fs_percolation;					/* multiplier */
	double	fs_threshold;					/* percent of max sat_deficit, for fill and spill  */	
	int	snow_albedo_flag;	/* (DIM) set as 1 for age model and 2 for BATS model */
	double  bats_b;				/* unitless */
	double  bats_r3;				/* unitless */
	double  active_zone_z;					/* m */
	double  DOM_decay_rate;					/* kg N /m */
	double  DON_adsorption_rate;				/* kg /kg soil */
	double  DOC_adsorption_rate;				/* kg /kg soil */
	double  N_decay_rate;					/* kg N /m */
	double  NO3_adsorption_rate;				/* kg /kg soil */
	double  NH4_adsorption_rate;				/* kg /kg soil */
	double  denitrif_proportion;				/* (DIM) 0-1 */
	double	DON_production_rate;					/* (DIM) 0-1 */
	double	gl_c;						/* m/s */
	double	gsurf_slope;					/* (DIM) */
	double  gsurf_intercept;				/* m/s */
	double  theta_mean_std_p1;				/* DIM */
	double  theta_mean_std_p2;				/* DIM */
	double  overstory_height_thresh;        /* Defines lower limit of overstory (m) */
	double  understory_height_thresh;       /* Defines upper limit of understory (m) */
	struct soil_class	soil_type;
	};


/*----------------------------------------------------------*/
/*      Define an innundation depth object.                                                             */
/*----------------------------------------------------------*/
struct  innundation_object
        {
        double  critical_depth;         /* m */
        double  gamma;
        int     num_neighbours;
        struct  neighbour_object *neighbours;
        };
/*----------------------------------------------------------*/
/*      Define a neighbours object.                                                             */
/*----------------------------------------------------------*/
struct  neighbour_object
        {
        double gamma;           /* m**2 / day */
        struct  patch_object *patch;
        };
/*----------------------------------------------------------*/
/*      Define litter  and soil cn flux objects                     */
/*----------------------------------------------------------*/

struct  cdayflux_patch_struct
        {

    /* dissolved organic fluxes */
    double do_litr1c_loss;      /* (kgC/m2/day) labile DOC loss */ 
    double do_litr2c_loss;      /* (kgC/m2/day) cellulose DOC loss */ 
    double do_litr3c_loss;      /* (kgC/m2/day) shielded cellulose DOC loss */ 
    double do_litr4c_loss;      /* (kgC/m2/day) lignan DOC loss */ 

    double do_soil1c_loss;      /* (kgC/m2/day) fast soil carbon DOC loss */ 
    double do_soil2c_loss;      /* (kgC/m2/day) medium soil DOC loss */ 
    double do_soil3c_loss;      /* (kgC/m2/day) slow DOC loss */ 
    double do_soil4c_loss;      /* (kgC/m2/day) recalcitrant DOC loss */ 
    double total_DOC_loss;      /* (kgC/m2/day) */
    double DOC_to_gw;           /* (kgC/m2/day) */


    /* potential decomp fluxes */
    double plitr1c_loss;        /* (kgC/m2/d) microbial assim. of labile lit */
    double plitr2c_loss;        /* (kgC/m2/d) microbial assim. of cellulose lit */
    double plitr3c_loss;        /* (kgC/m2/d) microbial assim. of shielded cellulose lit */
    double plitr4c_loss;        /* (kgC/m2/d) microbial assim. of lignin lit */
    double psoil1c_loss;         /* (kgC/m2/d) fast microbial recycling */
    double psoil2c_loss;        /* (kgC/m2/d) slow microbial recycling */
    double psoil3c_loss;        /* (kgC/m2/d) release of shielded cellulose */
    double psoil4c_loss;        /* (kgC/m2/d) recalcitrant SOM formation */
    double kl4;                 /* (1/day) rate constant for lignin litter decomp */    

    /* daily turnover fluxes */
    double leafc_to_litr1c;  /* (kgC/m2/d) leaf litfall (labile) */
    double leafc_to_litr2c;  /* (kgC/m2/d) leaf litfall (cell., unshielded) */
    double leafc_to_litr3c;  /* (kgC/m2/d) leaf litfall (cell., shielded) */
    double leafc_to_litr4c;  /* (kgC/m2/d) leaf litfall (lignin) */
    double frootc_to_litr1c; /* (kgC/m2/d) froot litfall (labile) */
    double frootc_to_litr2c; /* (kgC/m2/d) froot litfall (cell., unshielded) */
    double frootc_to_litr3c; /* (kgC/m2/d) froot litfall (cell., shielded) */
    double frootc_to_litr4c; /* (kgC/m2/d) froot litfall (lignin) */
    double litr1c_to_soil1c; /* (kgC/m2/d) microbial assim. of labile lit */
    double litr2c_to_soil2c; /* (kgC/m2/d) microbial assim. of cellulose lit */
    double litr3c_to_litr2c; /* (kgC/m2/d) release of shielded cellulose */
    double litr4c_to_soil3c; /* (kgC/m2/d) microbial assim. of lignin lit */
    double soil1c_to_soil2c; /* (kgC/m2/d) fast microbial recycling */
    double soil2c_to_soil3c; /* (kgC/m2/d) slow microbial recycling */
    double soil3c_to_soil4c; /* (kgC/m2/d) recalcitrant SOM formation */
    double cwdc_to_litr2c;   /* (kgC/m2/d) CWD to unshielded cellulose litter */
    double cwdc_to_litr3c;   /* (kgC/m2/d) CWD to shielded cellulose litter */
    double cwdc_to_litr4c;   /* (kgC/m2/d) CWD to lignin litter */

    /* daily heterotroph respiration fluxes */
    double litr1c_hr;         /* (kgC/m2/d) labile litter respiration */
    double litr2c_hr;         /* (kgC/m2/d) unshielded cellulose litter resp */
    double litr3c_hr;         /* (kgC/m2/d) cellulose litter resp */
    double litr4c_hr;         /* (kgC/m2/d) lignin litter respiration */
    double soil1c_hr;         /* (kgC/m2/d) fast microbial recycling resp */
    double soil2c_hr;         /* (kgC/m2/d) medium microbial recycling resp */
    double soil3c_hr;         /* (kgC/m2/d) slow microbial recycling resp */
    double soil4c_hr;         /* (kgC/m2/d) recalcitrant SOM respiration */
    /* daily MORTALITY fluxes */

    double m_leafc_to_litr1c;       /* (kgC/m2/d) leaf to labile */
    double m_leafc_to_litr2c;       /* (kgC/m2/d) leaf to unshielded cell. */
    double m_leafc_to_litr3c;       /* (kgC/m2/d) leaf to shielded cell. */
    double m_leafc_to_litr4c;       /* (kgC/m2/d) leaf to lignin */
    double m_frootc_to_litr1c;      /* (kgC/m2/d) froot to labile */
    double m_frootc_to_litr2c;      /* (kgC/m2/d) froot to un shielded cell. */
    double m_frootc_to_litr3c;      /* (kgC/m2/d) froot to shielded cell. */
    double m_frootc_to_litr4c;      /* (kgC/m2/d) froot to lignin */

    double m_leafc_store_to_litr1c; /* (kgC/m2/d) leaf store to labile */
    double m_frootc_store_to_litr1c; /* (kgC/m2/d) froot store to labile */
    double m_livestemc_store_to_litr1c; /* (kgC/m2/d) live stem store to labile */
    double m_deadstemc_store_to_litr1c; /* (kgC/m2/d) dead stem store to labile */
    double m_livecrootc_store_to_litr1c; /* (kgC/m2/d) live coarse root store to labile */
    double m_deadcrootc_store_to_litr1c; /* (kgC/m2/d) dead coarse root store to labile */

    double m_leafc_transfer_to_litr1c; /* (kgC/m2/d) leaf transfer to labile */
    double m_frootc_transfer_to_litr1c; /* (kgC/m2/d) froot transfer to labile */
    double m_livestemc_transfer_to_litr1c; /* (kgC/m2/d) live stem transfer to labile */
    double m_deadstemc_transfer_to_litr1c; /* (kgC/m2/d) dead stem transfer to labile */
    double m_livecrootc_transfer_to_litr1c; /* (kgC/m2/d) live coarse root transfer to labile */
    double m_deadcrootc_transfer_to_litr1c; /* (kgC/m2/d) dead coarse root transfer to labile */


    double m_gresp_store_to_litr1c;      /* (kgC/m2/d) */
    double m_gresp_transfer_to_litr1c;     /* (kgC/m2/d) */

    /* fire fluxes */
    double m_litr1c_to_atmos;       /* (kgC/m2) labile to atmosphere */
    double m_litr2c_to_atmos;       /* (kgC/m2) unshielded cellulose to atmosphere */
    double m_litr3c_to_atmos;       /* (kgC/m2) shielded cellulose to atmosphere */
    double m_litr4c_to_atmos;       /* (kgC/m2) lignin to atmosphere */
    double m_soil1c_to_atmos;       /* (kgC/m2) microbial recycling pool to atmosphere */
    double m_soil2c_to_atmos;       /* (kgC/m2) microbial recycling pool to atmosphere */
    double m_soil3c_to_atmos;       /* (kgC/m2) microbial recycling pool to atmosphere */
    double m_soil4c_to_atmos;       /* (kgC/m2) microbial recycling pool to atmosphere */

        };

struct  ndayflux_patch_struct
        {

    /* dissolved organic fluxes */
    double do_litr1n_loss;      /* (kgN/m2/day) labile DON loss */ 
    double do_litr2n_loss;      /* (kgN/m2/day) cellulose DON loss */ 
    double do_litr3n_loss;      /* (kgN/m2/day) shielded cellulose DON loss */ 
    double do_litr4n_loss;      /* (kgN/m2/day) lignan DON loss */ 

    double do_soil1n_loss;      /* (kgN/m2/day) fast soil DON loss */ 
    double do_soil2n_loss;      /* (kgN/m2/day) medium soil DON loss */ 
    double do_soil3n_loss;      /* (kgN/m2/day) slow DON loss */ 
    double do_soil4n_loss;      /* (kgN/m2/day) recalcitrant DON loss */ 

    double total_DON_loss;      /* (kgN/m2/day) */
    double DON_to_gw;   /* (kgN/m2/day) */

    /* potential decomp fluxes */
    double mineralized;         /* (kgN/m2/d) total mineralized N */
    double potential_immob;     /* (kgN/m2/d) potential imobilized N */
    double net_mineralized;   /* (kgN/m2/d) net mineralization */

    /* potential mineralization */
    double pmnf_l1s1;           /* (kgN/m2/d) microbial assim. of labile lit */
    double pmnf_l2s2;           /* (kgN/m2/d) microbial assim. of cellulose lit */
    double pmnf_l3l2;           /* (kgN/m2/d) microbial assim. of cellulose lit */
    double pmnf_l4s3;           /* (kgN/m2/d) microbial assim. of lignin lit */
    double pmnf_s1s2;            /* (kgN/m2/d) fast microbial recycling */
    double pmnf_s2s3;           /* (kgN/m2/d) slow microbial recycling */
    double pmnf_s3s4;           /* (kgN/m2/d) recalcitrant SOM formation */
    double pmnf_s4;             /* (kgN/m2/d) recalcitrant SOM decay */


        /* daily turnover fluxes */
    double leafn_to_litr1n;  /* (kgN/m2/d) leaf litfall labile */
    double leafn_to_litr2n;  /* (kgN/m2/d) leaf litfall unshielded cellulose */
    double leafn_to_litr3n;  /* (kgN/m2/d) leaf litfall shielded cellulose */
    double leafn_to_litr4n;  /* (kgN/m2/d) leaf litfall lignin */
    double frootn_to_litr1n; /* (kgN/m2/d) froot litfall labile */
    double frootn_to_litr2n; /* (kgN/m2/d) froot litfall unshielded cellulose */
    double frootn_to_litr3n; /* (kgN/m2/d) froot litfall shielded cellulose */
    double frootn_to_litr4n; /* (kgN/m2/d) froot litfall lignin */
    double litr1n_to_soil1n; /* (kgN/m2/d) microbial assim. of labile N */
    double litr2n_to_soil2n; /* (kgN/m2/d) microbial assim. of cellulose N */
    double litr4n_to_soil3n; /* (kgN/m2/d) microbial assim. of lignin N */
    double litr3n_to_litr2n; /* (kgN/m2/d) release of shielded cellulose N */
    double soil1n_to_soil2n; /* (kgN/m2/d) fast microbial recycling */
    double soil2n_to_soil3n; /* (kgN/m2/d) slow microbial recycling */
    double soil3n_to_soil4n; /* (kgN/m2/d) formation of recalcitrant SOM N */
    double cwdn_to_litr2n;   /* (kgN/m2/d) CWD to unshielded cellulose N */
    double cwdn_to_litr3n;   /* (kgN/m2/d) CWD to shielded cellulose N */
    double cwdn_to_litr4n;   /* (kgN/m2/d) CWD to lignin N */

    /* daily N immobilization & mineralization fluxes */
    double plant_potential_ndemand; /* (kgN/m2/d) potential N demand from plants              */
    double plant_avail_uptake; /* (kgN/m2/d) available N uptake from plants           */
    double sminn_to_soil1n_l1; /* (kgN/m2/d) N immob. between litr1 and soil1 */
    double sminn_to_soil2n_l2; /* (kgN/m2/d) N immob. between litr2 and soil2 */
    double sminn_to_soil2n_l3; /* (kgN/m2/d) N immob. between litr3 and soil2 */
    double sminn_to_soil3n_l4; /* (kgN/m2/d) N immob. between litr4 and soil3 */
    double sminn_to_soil2n_s1; /* (kgN/m2/d) N immob. between soil1 and soil2 */
    double sminn_to_soil3n_s2; /* (kgN/m2/d) N immob. between soil2 and soil3 */
    double sminn_to_soil4n_s3; /* (kgN/m2/d) N immob. between soil3 and soil4 */
    double sminn_to_npool;     /* (kgN/m2/d) N mineralized to plant pool uptake */
    double soil4n_to_sminn;    /* (kgN/m2/d) N from soil4 to mineral pool */
    double sminn_to_nitrate;     /* (kgN/m2/d) nitrification  */

    /* daily N MORTALITY fluxes */
    double m_leafn_to_litr1n;       /* (kgN/m2/d) leaf to labile */
    double m_leafn_to_litr2n;       /* (kgN/m2/d) leaf to unshielded cell. */
    double m_leafn_to_litr3n;       /* (kgN/m2/d) leaf to shielded cell. */
    double m_leafn_to_litr4n;       /* (kgN/m2/d) leaf to lignin */
    double m_frootn_to_litr1n;      /* (kgN/m2/d) froot to labile */
    double m_frootn_to_litr2n;      /* (kgN/m2/d) froot to un shielded cell. */
    double m_frootn_to_litr3n;      /* (kgN/m2/d) froot to shielded cell. */
    double m_frootn_to_litr4n;      /* (kgN/m2/d) froot to lignin */

    double m_livestemn_to_litr1n;   /*(kgN/m2/d) live stem to labile */
    double m_livecrootn_to_litr1n;   /*(kgN/m2/d) live coarse root to labile */
    double m_leafn_store_to_litr1n; /* (kgN/m2/d) leaf store to labile */
    double m_frootn_store_to_litr1n; /* (kgN/m2/d) froot store to labile */
    double m_livestemn_store_to_litr1n; /* (kgN/m2/d) live stem store to labile */
    double m_deadstemn_store_to_litr1n; /* (kgN/m2/d) dead stem store to labile */
    double m_livecrootn_store_to_litr1n; /* (kgN/m2/d) live noarse root store to labile */
    double m_deadcrootn_store_to_litr1n; /* (kgN/m2/d) dead noarse root store to labile */

    double m_leafn_transfer_to_litr1n; /* (kgN/m2/d) leaf transfer to labile */
    double m_frootn_transfer_to_litr1n; /* (kgN/m2/d) froot transfer to labile */
    double m_livestemn_transfer_to_litr1n; /* (kgN/m2/d) live stem transfer to labile */
    double m_deadstemn_transfer_to_litr1n; /* (kgN/m2/d) dead stem transfer to labile */
    double m_livecrootn_transfer_to_litr1n; /* (kgN/m2/d) live coarse root transfer to labile */

    double m_deadcrootn_transfer_to_litr1n; /* (kgN/m2/d) dead coarse root transfer to labile */

    double m_retransn_to_litr1n;           /* (kgN/m2/d) */


    /* denitrification  (volatilization) */
        double sminn_to_nvol;                /* (kgN/ms/d) */
        double denitrif;                        /* (kgN/m2/day) */


    /* daily N sources and sinks */
    double ndep_to_sminn;    /* (kgN/m2/d) deposition to soil min N pool */
    double nfix_to_sminn;             /* (kgN/m2/d) biological n fixation */
    double N_to_gw;        /* (kgN/m2/day) loss due to leaching to gw */

    /* fire fluxes */
    double m_litr1n_to_atmos;       /* (kgN/m2) labile to atmosphere */
    double m_litr2n_to_atmos;       /* (kgN/m2) unshielded cellulose to atmosphere */
    double m_litr3n_to_atmos;       /* (kgN/m2) shielded cellulose to atmosphere */
    double m_litr4n_to_atmos;       /* (kgN/m2) lignin to atmosphere */
    double m_soil1n_to_atmos;       /* (kgN/m2) microbial recycling pool to atmosphere */
    double m_soil2n_to_atmos;       /* (kgN/m2) microbial recycling pool to atmosphere */
    double m_soil3n_to_atmos;       /* (kgN/m2) microbial recycling pool to atmosphere */
    double m_soil4n_to_atmos;       /* (kgN/m2) microbial recycling pool to atmosphere */

        };
/*----------------------------------------------------------*/
/*      Define a litter objects                             */
/*----------------------------------------------------------*/

struct  litter_object
        {
        
        double cover_fraction;                  /* 0-1 */
        double depth;                   /* m */
        double density;                 /* m/kgC */
        double rain_stored;             /* m */
	double NO3_stored;		/* kg/m2 */
        double gsurf;
        double proj_pai;
        double rain_capacity;
        double  gl_c;                                           /* m/s */
        double  gsurf_slope;                                    /* (DIM) */
        double  gsurf_intercept;                                /* m/s */
        double  moist_coef;                                     /* m/kg */
        double T;               /* degress C */

        };
                
struct  litter_c_object
        {
    double litr1c;         /* (kgC/m2) litter labile C */
    double litr2c;         /* (kgC/m2) litter unshielded cellulose C */
    double litr3c;         /* (kgC/m2) litter shielded cellulose C */
    double litr4c;         /* (kgC/m2) litter lignin C */
    double litr1c_hr_snk;     /* (kgC/m2) SUM of labile litr microbial resp */
    double litr2c_hr_snk;     /* (kgC/m2) SUM of cellulose litr microbial resp */
    double litr4c_hr_snk;     /* (kgC/m2) SUM of lignin litr microbial resp */
    double fire_snk;          /* (kgC/m2) SUM of losses due to fire */
    double t_scalar;    /* (DIM) tempertaure scalar for decomposition */
    double w_scalar;    /* (DIM) tempertaure scalar for decomposition */
        };

struct  litter_n_object
        {
    double litr1n;          /* (kgN/m2) litter labile N */
    double litr2n;          /* (kgN/m2) litter unshielded cellulose N */
    double litr3n;          /* (kgN/m2) litter shielded cellulose N */
    double litr4n;          /* (kgN/m2) litter lignin N */
    double fire_snk;          /* (kgC/m2) SUM of losses due to fire */
        };

/*----------------------------------------------------------*/
/*      Define a soil   objects                             */
/*----------------------------------------------------------*/
struct  soil_c_object
        {

    double frootc;         /* (kgC/m2) total soil fine root C           */
    double DOC;            /* (kgC/m2) DOC */
    double DOC_Qin;     /* (kgC/m2/day) DON */
    double DOC_Qout;    /* (kgC/m2/day) DON */
    double DOC_Qin_total;       /* (kgC/m2/day) DON */
    double DOC_Qout_total;      /* (kgC/m2/day) DON */
    double totalc;         /* (kgC/m2) total soil  C    */
    double soil_cpool;     /* (kgC/m2) temporary soil carbon pool       */
    double soil1c;         /* (kgC/m2) microbial recycling pool C (fast) */
    double soil2c;         /* (kgC/m2) microbial recycling pool C (medium) */
    double soil3c;         /* (kgC/m2) microbial recycling pool C (slow) */
    double soil4c;         /* (kgC/m2) recalcitrant SOM C (humus, slowest) */
    double soil1c_hr_snk;     /* (kgC/m2) SUM of fast microbial respiration */
    double soil2c_hr_snk;     /* (kgC/m2) SUM of medium microbial respiration */
    double soil3c_hr_snk;     /* (kgC/m2) SUM of slow microbial respiration */
    double soil4c_hr_snk;     /* (kgC/m2) SUM of recalcitrant SOM respiration */
        };

struct  soil_n_object
        {
    int nlimit;                         /* DIM (0 or 1) */
    double  fract_potential_immob;                              /* DIM (0-1) */
    double fract_potential_uptake;                              /* DIM (0-1) */
    double totaln;         /* (kgN/m2) total soil  N    */
    double soil_npool;      /* (kgN/m2) temporary soil nitrogen pool    */
    double DON;            /* (kgN/m2) DON */
    double DON_Qin;     /* (kgM/m2/day) DON */
    double DON_Qout;    /* (kgN/m2/day) DON */
    double DON_Qin_total;       /* (kgN/m2/day) DON */
    double DON_Qout_total;      /* (kgN/m2/day) DON */
    double soil1n;          /* (kgN/m2) microbial recycling pool N (fast) */
    double soil2n;          /* (kgN/m2) microbial recycling pool N (medium) */
    double soil3n;          /* (kgN/m2) microbial recycling pool N (slow) */
    double soil4n;          /* (kgN/m2) recalcitrant SOM N (humus, slowest) */
    double sminn;           /* (kgN/m2) soil mineral N */
    double nitrate;          /* (kgN/m2) soil mineral N in nitrate form */
    double NO3_Qin_total;             /* (kgN/m2/day) soil mineral N output */
    double NO3_Qout_total;            /* (kgN/m2/day) soil mineral N input */
    double NO3_Qin;             /* (kgN/m2/day) soil mineral N output */
    double NO3_Qout;            /* (kgN/m2/day) soil mineral N input */
    double NH4_Qin;             /* (kgN/m2/day) soil mineral N output */
    double NH4_Qout;            /* (kgN/m2/day) soil mineral N input */
    double NH4_Qin_total;             /* (kgN/m2/day) soil mineral N output */
    double NH4_Qout_total;            /* (kgN/m2/day) soil mineral N input */
    double leach;            /* (kgN/m2) soil mineral N input */
    double nfix_src;        /* (kgN/m2) SUM of biological N fixation */
    double ndep_src;        /* (kgN/m2) SUM of N deposition inputs */
    double nleached_snk;    /* (kgN/m2) SUM of N leached */
    double nvolatilized_snk; /* (kgN/m2) SUM of N lost to volatilization */
        };

/*----------------------------------------------------------*/
/*      Define the fire structure.      */
/*----------------------------------------------------------*/


struct patch_fire_water_object
{
        double pet;                     /* mm */
        double et;                      /* mm */
	double understory_et; /*mm; understory layer et*/
	double understory_pet; /*mm; understory layer pet*/

};


/*----------------------------------------------------------*/
/*      Define the fire litter soil loss structure.        */
/*----------------------------------------------------------*/
struct fire_litter_soil_loss_struct
{
        double loss_litr1c;
        double loss_litr2c;
        double loss_litr3c;
        double loss_litr4c;
	double loss_soil1c;
	double loss_soil2c;
	double loss_soil3c;
	double loss_soil4c;
        double loss_litr1n;
        double loss_litr2n;
        double loss_litr3n;
        double loss_litr4n;
	double loss_soil1n;
	double loss_soil2n;
	double loss_soil3n;
	double loss_soil4n;
};


/*----------------------------------------------------------*/
/*      Define the spinup structure.                        */
/*----------------------------------------------------------*/

struct spinup_default {
        int ID;
        double tolerance;   // percent as fraction of 1
        double max_years;   /* years */
        };

struct spinup_object 
{
        struct spinup_default_object *defaults;
};      

struct stratum_spinup_object
{
        double tolerance;                      /* percent as a fraction of 1 */
        double max_years;                      /* years */

};

/*----------------------------------------------------------*/
/*      Define an patch object                              */      
/*----------------------------------------------------------*/
struct patch_object

        {
        int             zone_ID;
        int             default_flag;
        int             ID;     
        int             num_base_stations;                              
        int             num_innundation_depths;
        int             num_canopy_strata;
        int             num_shadow_strata; 
        int             num_layers;
        int             num_soil_intervals;                             /* unitless */
        int             target_status;
        double  x;                                                                      /* meters       */
        double  y;                                                                      /* meters       */
        double  z;                                                                      /* meters       */
        double  area;                   /* sq meters    */
        double  acc_year_trans;         /* m water      */
        double  base_flow;              /* m water */
        double  cap_rise;               /* m water / day */
        double  tmp;                    /* diagnostic variable - open units */
        double  daily_fire_litter_turnover;                     /* (DIM) 0-1 */
        double  delta_rain_stored;      /* m water      */
        double  delta_snow_stored;      /* m water      */
        double  detention_store;        /* m water      */
        double  effective_lai;          /* avg of strata m^2/m^2        */
        double  evaporation;            /* m  water*/
        double  evaporation_surf;       /* m  water*/
        double  ga;                     /* m/s */         
        double  ga_final;               /* m/s */  
        double  gasnow;                 /* m/s */         
        double  gasnow_final;           /* m/s */         
        double  gw_drainage;            /* m/day */
	double	gw_drainage_hourly;     /* m/day  */	
        double  hourly_rz_drainage;     /* m water  */
        double  hourly_unsat_drainage;  /* m water  */
        double  hourly_subsur2stream_flow;      /* m water */
        double  hourly_sur2stream_flow;  /* m water  */
        double  hourly_stream_flow;     /* m water */
        double  interim_sat;            /* m */
        double  stream_gamma;           /* meters**2/day        */
        double  Kdown_direct;           /* Kj/(m^2*day) */
        double  Kup_direct;             /* Kj/(m^2*day) */
        double  Kdown_diffuse;          /* Kj/(m^2*day) */
        double  Kup_diffuse;            /* Kj/(m^2*day) */
        double  Kdown_direct_final;     /* Kj/(m^2*day) */
        double  Kup_direct_final;       /* Kj/(m^2*day) */
        double  Kdown_diffuse_final;    /* Kj/(m^2*day) */
        double  Kup_diffuse_final;      /* Kj/(m^2*day) */
        double  Ldown;  /* Kj/(m^2*day) */
        double  Ldown_night; 	/* Kj/(m^2*day) */
        double	Ldown_day;		/* Kj/(m^2*day) */
        double  Ldown_final;    /* Kj/(m^2*day) */
        double  Ldown_final_night;	/* Kj/(m^2*day) */
        double 	Ldown_final_day;	/* Kj/(m^2*day) */
        double Kdown_direct_ovund;
        double Kup_direct_ovund;
        double Kdown_diffuse_ovund;
        double Kup_diffuse_ovund;
        double Kdown_direct_und;
        double Kup_direct_und;
        double Kdown_diffuse_und;
        double Kup_diffuse_und;         
        double Kdown_direct_bare;
        double Kup_direct_bare;
        double Kdown_diffuse_bare;
        double Kup_diffuse_bare;
        double  Kstar_canopy;                   /* Kj/(m^2*day) */
        double  Kstar_canopy_final;     /* Kj/(m^2*day) */
        double LE_canopy;  /* Kj/(m^2*day)      */
        double LE_canopy_final;  /* Kj/(m^2*day)        */
        double LE_soil;         /* Kj/(m^2*day) */
        double  Kstar_soil;                     /* Kj/(m^2*day) */
        double  Kdown_direct_subcanopy;         /* Kj/(m^2*day) */
        double  Kdown_diffuse_subcanopy;                /* Kj/(m^2*day) */
        double  Ksat_0;                 /* meteres/day  */
        double  Ksat_vertical;          /* meters/day   */
        double  lna;                    /* unitless     */
        double  lai;                    /* unitless     */
        double  Lup_soil;               /* Kj/(m^2*day) */
        double  Lup;            		/* Kj/(m^2*day) */
        double  Lstar_canopy;           /* Kj/(m^2*day) */
        double  Lstar_canopy_night;     /* Kj/(m^2*day) */
        double  Lstar_canopy_day;       /* Kj/(m^2*day) */
        double  Lstar_snow;             /* Kj/(m^2*day) */
        double  Lstar_snow_night;       /* Kj/(m^2*day) */
        double  Lstar_snow_day;         /* Kj/(m^2*day) */
        double  Lstar_soil;             /* Kj/(m^2*day) */
        double  Lstar_soil_night;       /* Kj/(m^2*day) */
        double  Lstar_soil_day;         /* Kj/(m^2*day) */
        double  Lstar_pond;             /* Kj/(m^2*day) */
        double  Lstar_pond_night;       /* Kj/(m^2*day) */
        double  Lstar_pond_day;         /* Kj/(m^2*day) */
        double  Ldown_subcanopy;        /* Kj/(m^2*day) */
        double  m;              /* m^-1 */
        double  m_z;            /* m^-1 */
        double  original_m;             /* m^-1 */
        double  PAR_direct;             /* umol/(m^2*day)       */
        double  PAR_diffuse;            /* umol/(m^2*day)       */
        double  PAR_direct_final;       /* umol/(m^2*day)       */
        double  PAR_diffuse_final;      /* umol(m^2*day)        */
        double  PH;                     /* DIM */
        double  potential_cap_rise;     /* m water/ day */
        double  potential_exfiltration; /* m water/ day */
        double  potential_evaporation;  /* m water/ day */
        double  psi;                    /* MPa          */
        double  psi_max_veg; /* MPa */
        double  Qin_total;                      /* m /day       */
        double  Qout_total;                     /* m /day       */
        double  Qin;                    /* m /day       */
        double  Qout;                   /* m /day       */
        double  streamflow;             /* m /day       */
        double  streamflow_DOC;         /* kgC/m2/day   */
        double  streamflow_DON;         /* kgN/m2/day   */
        double  streamflow_NO3;         /* kg/m2/day    */
        double  streamflow_NH4;         /* kg/m2/day    */
        double  road_cut_depth;         /* m */
        double  rain_throughfall;       /* m water      */      
        double  recharge;       /* m water      */      
        double  return_flow;            /* m water      */
        double  snow_throughfall;       /* m water      */
        double  rain_throughfall_24hours;       /* m water,used for 24 hours accumulated throughfall    */      
        double  rain_throughfall_final; /* m water      */      
        double  NO3_throughfall;        /* kg/m2 day  */
        double  NO3_throughfall_final;  /* kg/m2 day */
        double  rain_stored;            /* m water      */
        double  slope;                  /* degrees              */
        double  S;                      /* m/m          */
        double  sat_zone_storage;       /* m water      */
        double  snow_redist_scale;      /* multiplier   */ 
        double  std;                    /* m water      */
        double  theta_std;                      /* m water      */
        double  surface_NO3;            /* kg/m2        */
        double  streamNO3_from_surface; /* kg/m2        */
        double  streamNO3_from_sub;     /* kg/m2        */
        double  surface_NH4;            /* kg/m2        */
        double  grazing_Closs;          /* kgC/m2       */
        double  grazing_mean_nc;        /* ratio N:Co   */
        double  fertilizer_NO3_in;         /* kg/m2        */
        double  fertilizer_NH4_in;         /* kg/m2        */
        double  fertilizer_NO3;         /* kg/m2        */
        double  fertilizer_NH4;         /* kg/m2        */
        double  surface_DOC_Qin_total;  /* kgC/m2 day   */
        double  surface_DOC_Qout_total; /* kgC/m2 day   */
        double  surface_DON_Qin_total;  /* kgN/m2 day   */
        double  surface_DON_Qout_total; /* kgN/m2 day   */
        double  surface_DOC_Qin;        /* kgC/m2 day   */
        double  surface_DOC_Qout;       /* kgC/m2 day   */
        double  surface_DON_Qin;        /* kgN/m2 day   */
        double  surface_DON_Qout;       /* kgN/m2 day   */
        double  surface_NH4_Qin;                /* kg/m2 day    */
        double  surface_NH4_Qout;       /* kg/m2 day    */
        double  surface_NO3_Qin;                /* kg/m2 day    */
        double  surface_NO3_Qout;       /* kg/m2 day    */
        double  surface_ns_leach;       /* kg/m2 day    */
        double  surface_Qin;            /* m day        */
        double  surface_Qout;           /* m day        */
        double  surface_DON;            /* kgN/m2       */
        double  surface_DOC;            /* kgC/m2       */
        double  infiltration_excess;    /* m water      */
        double  snow_throughfall_final; /* m water      */      
        double  snow_melt;              /* m water      */
        double  snow_stored;            /* m water      */
        double  surface_flow;           /* m water      */
        double  surface_heat_flux;      /* kJ           */
        double  transpiration_sat_zone; /* m water      */
        double  transpiration_unsat_zone;       /* m water      */
        double  exfiltration_sat_zone;  /* m water      */
        double  exfiltration_unsat_zone;/* m water      */
        double  unsat_drainage;         /* m water      */
        double  PET;            /* m water      */
        double  PE;             /* m water      */
        double  precip_with_assim;    /* m water */
        double  rz_drainage;            /* m water      */
        double  wind;                   /* m/s          */
        double  wind_final;             /* m/s          */
        double  windsnow;                       /* m/s          */
        double  windsnow_final;         /* m/s          */
        double  ustar;                  /* m/s          */
        double  ustar_final;            /* m/s          */
        double  wilting_point;          /* mm */
        double overstory_fraction; /* 0-1 */
        double trans_reduc_perc; /*0-1*/
        double overland_flow; /* m/s */
        double  T_canopy;  /* deg C */
        double  T_canopy_final;  /* deg C */
        struct  base_station_object     **base_stations;
        struct  soil_default            **soil_defaults;
        struct  landuse_default         **landuse_defaults;
        struct  fire_default            **fire_defaults;
        struct  surface_energy_default  **surface_energy_defaults;
        struct  grow_patch_object       *grow;
        struct  canopy_strata_object    **canopy_strata;
        struct  canopy_strata_object    **shadow_strata;
        struct  patch_object            *shadow_litter;
        struct  patch_hourly_object     *hourly;
        struct  layer_object            *layers;
        struct  innundation_object      *innundation_list; // Used for subsurface routing, and surface routing when no surface table is provided
        struct  innundation_object      *surface_innundation_list; // Used for surface routing
        struct  neighbour_object        *neighbours;
        struct  patch_object            *next_stream;
        struct  surface_energy_object   *surface_energy_profile;
        struct  patch_fire_water_object       fire;
        struct  accumulate_patch_object acc_month;
        struct  accumulate_patch_object acc_year;
        struct  rooting_zone_object     rootzone;
        struct  zone_object             *zone; /* parent zone */


/*----------------------------------------------------------*/
/*      Surface Hydrology  stuff                        */
/*----------------------------------------------------------*/
        int     drainage_type;                          /* unitless 1 stream, 0 land, 2, road */        
        double  water_balance;                          /* meters water         */
        double  delta_snowpack;                         /* meters               */
        double  delta_canopy_storage;                   /* meters water         */
        double  deltaS;                                 /* meters water         */
        double  evap_potential;                         /* Joules               */
        double  field_capacity;                         /* meters water         */
        double  percent_soil_water_unfrozen;            /* 0-1          */
        double  preday_snow_stored;                     /* meters water         */
        double  preday_detention_store;                 /* meters water         */
        double  preday_rain_stored;                     /* meters water         */
        double  preday_snowpack;                        /* meters water         */
        double  preday_sat_deficit;                     /* meters water         */
        double  preday_sat_deficit_z;                   /* meters               */
        double  sat_deficit;                            /* meters water         */
        double  sat_deficit_z;                          /* meters               */
        double  *transmissivity_profile;                /* array (m/day) */
        struct  snowpack_object snowpack;               /* meters               */
        double  preday_unsat_storage;                   /* meters water         */
        double  preday_rz_storage;                      /* meters water by Taehee Hwang */
        double  unsat_storage;                          /* meters water         */
        double  rz_storage;                             /* meters water by Taehee Hwang */
        double  unsat_zone_volume;                      /* meters water         */

/*----------------------------------------------------------*/
/*      Forest floor stuff                                  */
/*----------------------------------------------------------*/
        double  surface_Tday;                   /* deg C                */
        double  surface_Tnight;                 /* deg C                */
        double  soil_height;                    /* m relative to surface */
        double  gsurf;                          /* m/s          */
        double  stability_correction;           /* DIM */
        double  Tday_surface_offset;            /* deg C        */
        double  Tday_surface_offset_final;      /* deg C        */
        double  Tsoil;                          /* degrees C    */
/*----------------------------------------------------------*/
/*      carbon and nitrogen objects for patch soil and litter */
/*----------------------------------------------------------*/

        double  burn;                           /* 0-1 % burned */
        double  net_plant_psn;                  /* kgC/m2 net carbon flux into patch */
        double  preday_totalc;                  /* kgC/m2 total carbon */
        double  totalc;                         /* kgC/m2 total carbon */
        double  carbon_balance;                 /* kgC/m2 */

        double  preday_totaln;                  /* kgC/m2 total nitrogen */
        double  totaln;                         /* kgC/m2 total nitrogen */
        double  nitrogen_balance;               /* kgC/m2 */
        double  satzone_nitrate;                /* kgN/m2 saturated zone */

        struct  soil_c_object   soil_cs;
        struct  soil_n_object   soil_ns;
        struct  litter_object   litter;
        struct  litter_c_object litter_cs;
        struct  litter_n_object litter_ns;
        struct  soil_c_object   *shadow_soil_cs;
        struct  soil_n_object   *shadow_soil_ns;
        struct  litter_c_object *shadow_litter_cs;
        struct  litter_n_object *shadow_litter_ns;
        struct cdayflux_patch_struct    cdf;
        struct ndayflux_patch_struct    ndf;
        };

/*----------------------------------------------------------*/
/*      Define the l;ayer object structure.                     */
/*----------------------------------------------------------*/
struct  layer_object
        {
        int     count;
        long    *strata;
        double  height;         /* m */
        double  base;           /* m */
        double  null_cover;
        };


/*----------------------------------------------------------*/
/*      Define the patch hourly parameter structure.                    */
/*----------------------------------------------------------*/
struct  patch_hourly_object
        {
        int     i;
        double  rain_throughfall;
        double  NO3_throughfall;
        double  NO3_throughfall_final;
        double  infiltration;
        double  streamflow_NO3_from_surface;
        double  streamflow_NO3_from_sub;
        double  streamflow_NO3;
        };

/*----------------------------------------------------------*/
/*      Define the grow object for the forest floor.                    */
/*----------------------------------------------------------*/
struct  grow_patch_object
        {
        double  litter_C;
        double  litter_N;
        double  surface_C;
        double  surface_N;
        double  humic_C;
        double  humic_N;
        double  avail_N;
        double  N_loss;
        double  resp_autr;      
        };







/*----------------------------------------------------------*/
/*      Define a basin_grow_default object.                                     */
/*----------------------------------------------------------*/
struct  basin_grow_default
        {
        int             temp;
        };

/*----------------------------------------------------------*/
/*      Define a hillslope_grow_default object.                                 */
/*----------------------------------------------------------*/
struct  hillslope_grow_default
        {
        int             temp;
        };

/*----------------------------------------------------------*/
/*      Define a zone_grow_default object.                                      */
/*----------------------------------------------------------*/
struct  zone_grow_default
        {
        int             temp;
        };


/*----------------------------------------------------------*/
/*      Define a patch_grow_default object.                                     */
/*----------------------------------------------------------*/
struct  patch_grow_default
        {
        int             temp;
        };


/*----------------------------------------------------------*/
/*      Define valid options.                                                                   */
/*----------------------------------------------------------*/
/*----------------------------------------------------------*/
/*      Define a stro_option object.                                                            */
/*----------------------------------------------------------*/
struct  stro_option
        {
        int             reachID;
        };


/*----------------------------------------------------------*/
/*      Define a b_option object.                                                               */
/*----------------------------------------------------------*/
struct  b_option
        {
        int             basinID;
        };

/*----------------------------------------------------------*/
/*      Define a h_option object.                                                               */
/*----------------------------------------------------------*/
struct  h_option
        {
        int             basinID;
        int             hillID;
        };

/*----------------------------------------------------------*/
/*      Define a z_option object.                                                               */
/*----------------------------------------------------------*/
struct  z_option
        {
        int             basinID;
        int             hillID;
        int             zoneID;
        };


/*----------------------------------------------------------*/
/*      Define a p_option object.                                                               */
/*----------------------------------------------------------*/
struct  p_option
        {
        int             basinID;
        int             hillID;
        int             zoneID;
        int             patchID;
        };

/*----------------------------------------------------------*/
/*      Define a c_option object.                                                               */
/*----------------------------------------------------------*/
struct  c_option
        {
        int             basinID;
        int             hillID;
        int             zoneID;
        int             patchID;
        int             stratumID;
        };

/*----------------------------------------------------------*/
/*      Define output flags object.                                                             */
/*----------------------------------------------------------*/
struct  output_flag
        {
        int             yearly;
        int             yearly_growth;
        int             monthly;
        int             daily;
        int             daily_growth;
        int             hourly;
        int             hourly_growth;
        };

/*----------------------------------------------------------*/
/*      Define a command_line_object object.                */
/*----------------------------------------------------------*/
struct  command_line_object
        {
        int             gridded_ascii_flag;
        int             gridded_netcdf_flag;
        int             grow_flag;
        int             vegspinup_flag; 
        int             routing_flag;
        int             surface_routing_flag;
        int             stream_routing_flag;
        int             gwtoriparian_flag;
        int             reservoir_operation_flag;
        int             ddn_routing_flag;
        int             dclim_flag;
        int             clim_repeat_flag;
        int             road_flag;
        int             vsen_flag;
        int             vsen_alt_flag;
        int             sen_flag;
        int             vgsen_flag;
        int             prefix_flag;
        int             verbose_flag;
        int             threshold_flag;
        int             tec_flag;
        int             world_flag;
        int             world_header_flag;
        int             start_flag;
        int             end_flag;
        int             firespread_flag;
        int             prev_flag;
        int             gw_flag;
        int             tchange_flag;
        int             stdev_flag;
        int             surface_energy_flag;
        int             precip_scale_flag;
        int             snow_scale_flag;
        int             noredist_flag;
        int             vmort_flag;
        int             version_flag;
        int		FillSpill_flag;
        int		evap_use_longwave_flag;
        char    *output_prefix;
        char    routing_filename[FILEPATH_LEN];
        char    surface_routing_filename[FILEPATH_LEN];
        char    stream_routing_filename[FILEPATH_LEN];
        char    reservoir_operation_filename[FILEPATH_LEN];
        char    world_filename[FILEPATH_LEN];
        char    world_header_filename[FILEPATH_LEN];
        char    tec_filename[FILEPATH_LEN];
        char    vegspinup_filename[FILEPATH_LEN];
        double  tmp_value;
        double  cpool_mort_fract;
        double  veg_sen1;
        double  veg_sen2;
        double  veg_sen3;
        double  tmax_add;
        double  tmin_add;
        double  fire_grid_res;
        double  sat_to_gw_coeff_mult;
        double  gw_loss_coeff_mult;
        double  snow_scale_tol;
        double  sen[3];
        double  vsen[2];
        double  vsen_alt[2];
        double  std_scale;
        double  thresholds[2];
	double	fs_spill;
	double	fs_percolation;
	double	fs_threshold;
        struct  output_flag     output_flags;
        struct  b_option        *b;
        struct  h_option        *h;
        struct  z_option        *z;
        struct  p_option        *p;
        struct  c_option        *c;
        struct  stro_option     *stro;
        struct  date            output_yearly_date;
        struct  date            start_date;
        struct  date            end_date;
        };



/*----------------------------------------------------------*/
/*      Define a tec file object.                                                               */
/*----------------------------------------------------------*/
struct  tec_object
        {
        FILE    *tfile;
        };


/*----------------------------------------------------------*/
/*      Define a tec file entry object.                                                 */
/*----------------------------------------------------------*/
struct tec_entry 
        {
        struct  date cal_date;
        char    command[TEC_CMD_LEN];
        };


/*----------------------------------------------------------*/
/*      Define min and max functions                                                    */
/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
/*      Define arrays which hold useful look up tables.                 */
/*      FOr now we keep them in this .h file but later it               */
/*      can be separated.                                                                               */
/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
/*      Arry of monthly solar constatnt adjusted for solar              */
/*      orbit radius factor.  From C version or Rhessys.                */
/*----------------------------------------------------------*/
/*define Io_array[13] = { 0.0, 1.445, 1.431, 1.410, 1.389, 1.354,
                                        1.354, 1.375, 1.403, 1.424, 1.438, 1.445 }
*/

/*----------------------------------------------------------*/
/*      Array of solar declinations every 8th day                               */
/*      starting on  Jan 1.                                                                             */
/*      From C version of Rhessys.                                                              */
/*----------------------------------------------------------*/
/*define declination_array[47] = { 0.0, -23.0, -22.0, -21.0, -19.0,
                        -17.0, -15.0, -12.0, -9.0, -6.0, -3.0, 0.0, 3.0, 
                        6.0, 9.0, 12.0, 14.0, 17.0, 19.0, 21.0, 22.0, 
                        23.0, 23.5, 23.0, 21.5, 20.0, 18.0, 16.0, 14.0,  
                        12.0, 9.0, 6.0, 3.0, 0.0, -3.0, -6.0, -9.0, -12.0,
                        -15.0, -17.0, -19.0, -21.0, -22.0, -23.0, -23.5, 
                        -23.5 }
*/

/*----------------------------------------------------------*/
/*      Array of optical air mass numbers.      */
/*----------------------------------------------------------*/
/*define  air_mass_array[22]  =
         {0.0, 2.90,3.05,3.21,3.39, 3.69, 3.82, 4.07, 4.37, 4.72, 5.12,
         5.60,6.18,6.88,7.77,8.90,10.39,12.44,15.36,19.79,26.96,30.00}
          
*/




/* phenological control arrays */
/* note this is modified from BIOME_bgc approach by Thornten (1998) */
/* to allow for daily rather than annual control of phenology           */

struct phenology_struct
{
        double leaflitfallc; /* (kgC/m2) current growth year leaflitter carbon */
        double leaflitfallc_wstress; /* (kgC/m2) accumulated leaf drop due to water stress for that year */
        double leaflitfalln; /* (kgN/m2) current growth year leaflitter nitrogen */
        double leafretransn; /* (kgN/m2) current growth year leaf N retranslocation */
        double frootlitfallc; /* (kgC/m2) current growth year leaflitter carbon */
        double frootlitfalln; /* (kgN/m2) current growth year leaflitter nitrogen */
        double daily_allocation;    /* (DIM) signal to allocate when set to 1 */
        double gsi;             /* (0 to 1) growing season phenology index */
                int annual_allocation;    /* (DIM) signal to allocate when set to 1 */
        int expand_startday;       /* (yday) yearday of first leaf growth */
        int litfall_startday;       /* (yday) yearday of litterfall growth */
        int expand_stopday;       /* (yday) yearday of last leaf growth */
        int litfall_stopday;       /* (yday) yearday of last litterfall growth */
        int ngrowthdays; /* (days) days between onday and next offday */
        int nretdays;    /* (days) days between allocations */
                int gwseasonday; /* (day) day within the growing season */
                int lfseasonday; /* (day) day within litter fall period */
                
                
};

        

/* carbon state variables (including sums for sources and sinks) */
struct cstate_struct
{
        int     age; /* (num years) */
        int     num_resprout; /* (num years) running index of years of resprouting */   
    
    double mortality_fract;   /* percentage lost to carbonhydrate storage mortality this year */
    double preday_totalc;   /* (kgC/m2) previous days plant carbon total */
    double totalc;          /* (kgC/m2) previous days plant carbon total */
    double net_psn;         /* (kgC/m2)  net photosynthesis (psn-respiration) */
    double cpool;           /* (kgC/m2) temporary plant C pool */
    double availc;         /* (kgC/m2) plant C from photosynthesis available for growth*/
    double leafc;           /* (kgC/m2) leaf C */
    double dead_leafc;      /* (kgC/m2) standing dead leaf C for grasses */
    double live_stemc;      /* (kgC/m2) live stem C */
    double dead_stemc;      /* (kgC/m2) dead stem C */
    double live_crootc;     /* (kgC/m2) live coarse root C */
    double dead_crootc;     /* (kgC/m2) dead coarse root C */
    double frootc;          /* (kgC/m2) fine root C */ 

    double leafc_transfer;      /* (kgC/m2) leaf C to be allocated from last season */
    double livestemc_transfer; /* (kgC/m2) live stemwood C to be allocated from last season */
    double deadstemc_transfer; /* (kgC/m2) dead stemwood C to be allocated from last season */
    double livecrootc_transfer;/* (kgC/m2) live coarse root C to be allocated from last season */
    double deadcrootc_transfer;/* (kgC/m2) dead coarse root C to be allocated from last season */
    double frootc_transfer;     /* (kgC/m2) leaf C to be allocated from last season */
    double gresp_transfer;    /* (kgC/m2) growth respiration C to be allocated from last season*/ 

    double leafc_store;     /* (kgC/m2) stored leaf C stored from year's growth */
    double livestemc_store; /* (kgC/m2) live stemwood C stored from this years growth */
    double deadstemc_store; /* (kgC/m2) dead stemwood C  stored from this years growth*/
    double livecrootc_store;/* (kgC/m2) live coarse root C  stored from this years growth*/
    double deadcrootc_store;/* (kgC/m2) dead coarse root C  stored from this years growth*/
    double frootc_store;    /* (kgC/m2) fine root C  stored from this years growth */ 

    double gresp_store;    /* (kgC/m2) growth respiration C stored from this years growth */ 
    double cwdc;           /* (kgC/m2) coarse woody debris C*/

/* sink for respiration and fire losses */
    double gpsn_src;       /* (kgC/m2) SUM of gross PSN carbon inputs */
    double fire_snk;       /* (kgC/m2) SUM of carbon loss due to fire */
    double leaf_mr_snk;      /* (kgC/m2) SUM of leaf maint resp */
    double leaf_gr_snk;      /* (kgC/m2) SUM of leaf growth resp */
    double froot_mr_snk;     /* (kgC/m2) SUM of fine root maint resp */
    double froot_gr_snk;     /* (kgC/m2) SUM of fine root growth resp */
    double livestem_mr_snk;  /* (kgC/m2) SUM of live stem maint resp */
    double livestem_gr_snk;  /* (kgC/m2) SUM of live stem growth resp */
    double deadstem_gr_snk;  /* (kgC/m2) SUM of dead stem growth resp */
    double livecroot_mr_snk; /* (kgC/m2) SUM of live coarse root maint resp */
    double livecroot_gr_snk; /* (kgC/m2) SUM of live coarse root growth resp */
    double deadcroot_gr_snk; /* (kgC/m2) SUM of dead coarse root growth resp */
 
    double Tacc;     /* degrees C, temperature during growth/acclimation period */
} ;



/* maintenance respiration constants */
struct  mrconst_struct
{
    double q10;            /* (DIM)    Q10 for maintenance respiration */
    double per_N;          /* (kgC/kgN/day)   maintenance resp per kg Nitrogen ie. kgC/kgN/day */
    double sapw_max;       /* (kgC/m2) maximum sapwood */
} ;



/* special input and output structures for the photosynthesis routine */
struct  psnin_struct
{
        int     c3;             /* C3/C4 plant flag     */
        double pa;              /* (Pa) atmospheric pressure */
        double co2;             /* (ppm) atmospheric [CO2] */
        double t;               /* (deg C) temperature */
        double irad;            /* (umol/m2/s) shortwave irradiance */
        double g;               /* (m * 10**3/s) conductance to CO2 */
        double Rd;              /* (umol/m2/s) dark respiration rate */
        double lnc;             /* (kg Nleaf/m2) leaf nitrogen per unit area */
        double flnr;            /* (kg NRub/kg Nleaf) fract. of leaf N in Rubisco */
} ;

struct  psnout_struct
{
        double g;               /* (umol/m2/s/Pa) conductance to CO2 */
        double O2;              /* (Pa) atmospheric [O2] */
        double Ca;              /* (Pa) atmospheric [CO2] */
        double Ci;              /* (Pa) intercellular [CO2] */
        double gamma;           /* (Pa) CO2 compensation point, no Rd */
        double Kc;              /* (Pa) MM constant carboxylation */
        double Ko;              /* (Pa) MM constant oxygenation */
        double act;             /* (umol/kg/s) Rubisco activity */
        double Vmax;            /* (umol/m2/s) max rate carboxylation */
        double Jmax;            /* (umol/m2/s) max rate electron transport */
        double J;               /* (umol/m2/s) rate of RuBP regeneration */
        double Av;              /* (umol/m2/s) carboxylation limited assimilation */
        double Aj;              /* (umol/m2/s) RuBP regen limited assimilation */
        double A;               /* (umol/m2/s) final assimilation rate */
        double dC13;            /* % discrimination of C13 */
} ;







/* ecophysiological variables */
struct epvar_struct
{
    double height;                                              /* meters       */
    double all_pai;        /* (DIM) all-sided plant area index */
    double proj_pai;       /* (DIM) projected plant area index */
    double all_lai;        /* (DIM) live all-sided leaf area index */
    double proj_lai;       /* (DIM) live projected leaf area index */
    double proj_sla_sunlit;       /* (DIM) live projected leaf area index */
    double proj_sla_shade;       /* (DIM) live projected leaf area index */
    double proj_lai_sunlit;       /* (DIM) live projected leaf area index */
    double proj_lai_shade;       /* (DIM) live projected leaf area index */
    double max_proj_lai;   /* (DIM) this year's max growing season LAI */
    double prev_leafcalloc; /* (kgC/m2/y) last year's leaf C allocation */
    double canopy_w;       /* (kgH2O/m2) water store on canopy */
    double psi;            /* (MPa) water potential of soil and leaves */
    double psi_ravg;            /* (MPa) water potential of soil and leaves */
    int   wstress_days;    /* # of days in which water stress occurs */
    double crit_vwc;       /* (m/m) water content at stomatal closure */
    double  min_vwc;       /* (m/m) this growing season minimum water content */
    double max_fparabs;    /* (DIM) maximum fraction of PAR absorbed */
    int leafon;            /* (flag) binary flag, leaves on=1, not on=0 */
    double day_livestem_turnover; /* (kgC/m2/day) livestem turnover to dead wood */
    double day_livecroot_turnover; /* (kgC/m2/day) livestem turnover to dead wood */
    double day_deadleaf_turnover; /* (kgC/m2/day) dead grass turnover to soil */

};



 struct cdayflux_struct
{


        double fleaf; /* 0-1 */
        double froot; /* 0-1 */
        double fwood; /* 0-1 */
        
        /* gross PSN input */
    double psn_to_cpool;    /* (kgC/m2/d) gross photosynthesis */
    double potential_psn_to_cpool;    /* (kgC/m2/d) potential gross photosynthesis */
    double DOC_to_gw;   /* (kgC/m2/day) */

    /* daily phenology fluxes */
    double leafc_to_deadleafc;     /* (kgC/m2/d) standing dead grass accumulation */
    double leafc_store_to_leafc;   /* (kgC/m2/d) leaf expansion */
    double frootc_store_to_frootc; /* (kgC/m2/d) froot expansion */

    /* phenology fluxes from transfer pool */
    double leafc_transfer_to_leafc;          /* (kgC/m2/d) */
    double frootc_transfer_to_frootc;        /* (kgC/m2/d) */
    double livestemc_transfer_to_livestemc;  /* (kgC/m2/d) */
    double deadstemc_transfer_to_deadstemc;  /* (kgC/m2/d) */
    double livecrootc_transfer_to_livecrootc;/* (kgC/m2/d) */
    double deadcrootc_transfer_to_deadcrootc;/* (kgC/m2/d) */

    /* daily growth fluxes */
        double cpool_to_leafc;               /* (kgC/m2/d) */
        double cpool_to_leafc_store;       /* (kgC/m2/d) */
        double cpool_to_frootc;              /* (kgC/m2/d) */
        double cpool_to_frootc_store;      /* (kgC/m2/d) */
        double cpool_to_livestemc;           /* (kgC/m2/d) */
        double cpool_to_livestemc_store;   /* (kgC/m2/d) */
        double cpool_to_deadstemc;           /* (kgC/m2/d) */
        double cpool_to_deadstemc_store;   /* (kgC/m2/d) */
        double cpool_to_livecrootc;          /* (kgC/m2/d) */
        double cpool_to_livecrootc_store;  /* (kgC/m2/d) */
        double cpool_to_deadcrootc;          /* (kgC/m2/d) */
        double cpool_to_deadcrootc_store;  /* (kgC/m2/d) */
        double cpool_to_gresp_store;       /* (kgC/m2/d) */

        /* annual turnover of storage to transfer pools */
        double leafc_store_to_leafc_transfer;           /* (kgC/m2/d) */
        double frootc_store_to_frootc_transfer;         /* (kgC/m2/d) */
        double livestemc_store_to_livestemc_transfer;    /* (kgC/m2/d) */
        double deadstemc_store_to_deadstemc_transfer;    /* (kgC/m2/d) */
        double livecrootc_store_to_livecrootc_transfer; /* (kgC/m2/d) */
        double deadcrootc_store_to_deadcrootc_transfer; /* (kgC/m2/d) */
        double gresp_store_to_gresp_transfer;           /* (kgC/m2/d) */

        /* turnover of live wood to dead wood */
        double livestemc_to_deadstemc;        /* (kgC/m2/d) */
        double livecrootc_to_deadcrootc;      /* (kgC/m2/d) */



    /* daily autotroph respiration fluxes */
    double leaf_day_mr;     /* (kgC/m2/d) day leaf maintenance respiration */
    double leaf_night_mr;   /* (kgC/m2/d) night leaf maintenance respiration */
    double froot_mr;        /* (kgC/m2/d) froot maintenance respiration */
    double livestem_mr;    /* (kgC/m2/d) live stem maintenance respiration */
    double livecroot_mr;   /* (kgC/m2/d) live croot maint. respiration */
    double total_mr;       /* (kgC/m2/d) total maintenance respiration */

        /* daily growth respiration fluxes */
        double cpool_leaf_gr;                /* (kgC/m2/d) */
        double transfer_leaf_gr;             /* (kgC/m2/d) */
        double cpool_froot_gr;               /* (kgC/m2/d) */
        double transfer_froot_gr;            /* (kgC/m2/d) */
        double cpool_livestem_gr;            /* (kgC/m2/d) */
        double transfer_livestem_gr;         /* (kgC/m2/d) */
        double cpool_deadstem_gr;            /* (kgC/m2/d) */
        double transfer_deadstem_gr;         /* (kgC/m2/d) */
        double cpool_livecroot_gr;           /* (kgC/m2/d) */
        double transfer_livecroot_gr;        /* (kgC/m2/d) */
        double cpool_deadcroot_gr;           /* (kgC/m2/d) */
        double transfer_deadcroot_gr;        /* (kgC/m2/d) */
        double transfer_gr;        /* (kgC/m2/d) */
        double total_gr;        /* (kgC/m2/d) */
};

/* annual carbon flux variables */
 struct cannflux_struct
{
    double cpool_to_leafc_store; /* (kgC/m2/yr) leaf carbon allocation */
    double cpool_to_live_stemc;  /* (kgC/m2/yr) stem carbon allocation */
    double cpool_to_live_crootc; /* (kgC/m2/yr) croot carbon allocation */
    double cpool_to_frootc_store; /* (kgC/m2/yr) froot corbon allocation */
    double cpool_to_dead_stemc;  /* (kgC/m2/yr) excess C to dead wood */
    double cpool_to_dead_crootc; /* (kgC/m2/yr) excess C to dead wood */
    double cpool_to_luxc;        /* (kgC/m2/yr) grass luxury C to litr labile */
    double live_stem_gr;         /* (kgC/m2/yr) stem growth respiration */
    double live_croot_gr;        /* (kgC/m2/yr) croot growth respiration */
    double luxury_stem_gr;       /* (kgC/m2/yr) dead stem luxury growth */
    double luxury_croot_gr;      /* (kgC/m2/yr) dead croot luxury growth */
    double live_stemc_to_dead_stemc; /* (kgC/m2/yr) live stem turnover */
    double live_crootc_to_dead_crootc; /* (kgC/m2/yr) live croot turnover */
};

/* nitrogen state variables (including sums for sources and sinks) */ 
 struct nstate_struct
{       
    int    nlimit;          /* (0-1) 0 is not limited on that day */
    double preday_totaln;   /* (kgN/m2) previous days plant nitrogen total */
    double totaln;          /* (kgN/m2) previous days plant nitrogen total */
    double npool;           /* (kgN/m2) temporary plant N pool */
    double leafn;           /* (kgN/m2) leaf N */
    double dead_leafn;      /* (kgN/m2) standing dead leaf N for grasses */
    double live_stemn;      /* (kgN/m2) live stem N */
    double dead_stemn;      /* (kgN/m2) dead stem N */
    double live_crootn;     /* (kgN/m2) live coarse root N */
    double dead_crootn;     /* (kgN/m2) dead coarse root N */
    double frootn;          /* (kgN/m2) fine root N */ 
    double retransn;        /* (kgN/m2) retranslocated N */

    double leafn_transfer;      /* (kgN/m2) leaf C to be allocated from last season */
    double livestemn_transfer; /* (kgN/m2) live stemwood C to be allocated from last season */
    double deadstemn_transfer; /* (kgN/m2) dead stemwood C to be allocated from last season */
    double livecrootn_transfer;/* (kgN/m2) live coarse root C to be allocated from last season */
    double deadcrootn_transfer;/* (kgN/m2) dead coarse root C to be allocated from last season */
    double frootn_transfer;     /* (kgN/m2) leaf C to be allocated from last season */

    double leafn_store;     /* (kgN/m2) stored leaf N stored from year's growth */
    double livestemn_store; /* (kgN/m2) live stemwood C stored from this years growth */
    double deadstemn_store; /* (kgN/m2) dead stemwood C  stored from this years growth*/
    double livecrootn_store;/* (kgN/m2) live coarse root C  stored from this years growth*/
    double deadcrootn_store;/* (kgN/m2) dead coarse root C  stored from this years growth*/
    double frootn_store;    /* (kgN/m2) fine root N  stored from this years growth */ 

    double cwdn;           /* (kgN/m2) coarse woody debris N*/
    double fire_snk;       /* (kgC/m2) SUM of nitrogen loss due to fire */
};

/* daily nitrogen flux variables */
 struct ndayflux_struct
{
        /* daily flux to plant pool */
    double sminn_to_npool;   /* (kgN/m2/d) plant N uptake */


        /* daily phenology fluxes */
    double leafn_to_deadleafn;     /* (kgN/m2/d) standing dead grass accumulation */
    double leafn_store_to_leafn;   /* (kgN/m2/d) leaf expansion N */
    double frootn_store_to_frootn; /* (kgN/m2/d) fine root expansion N */ 

    /* phenology fluxes from transfer pool */
   double leafn_transfer_to_leafn;           /* (kgN/m2/d) */
   double frootn_transfer_to_frootn;         /* (kgN/m2/d) */
   double livestemn_transfer_to_livestemn;   /* (kgN/m2/d) */
   double deadstemn_transfer_to_deadstemn;   /* (kgN/m2/d) */
   double livecrootn_transfer_to_livecrootn; /* (kgN/m2/d) */
   double deadcrootn_transfer_to_deadcrootn; /* (kgN/m2/d) */


    /* daily growth fluxes */
        double potential_N_uptake; /* (kgN/m2) potential uptake from soil */
        double actual_N_uptake; /* (kgN/m2) potential uptake from soil */
        double retransn_to_npool;             /* (kgN/m2/d) */
        double npool_to_leafn;                /* (kgN/m2/d) */
        double npool_to_leafn_store;        /* (kgN/m2/d) */
        double npool_to_frootn;               /* (kgN/m2/d) */
        double npool_to_frootn_store;       /* (kgN/m2/d) */
        double npool_to_livestemn;            /* (kgN/m2/d) */
        double npool_to_livestemn_store;    /* (kgN/m2/d) */
        double npool_to_deadstemn;            /* (kgN/m2/d) */
        double npool_to_deadstemn_store;    /* (kgN/m2/d) */
        double npool_to_livecrootn;           /* (kgN/m2/d) */
        double npool_to_livecrootn_store;   /* (kgN/m2/d) */
        double npool_to_deadcrootn;           /* (kgN/m2/d) */
        double npool_to_deadcrootn_store;   /* (kgN/m2/d) */

     /* annual turnover of storage to transfer */
        double leafn_store_to_leafn_transfer;           /* (kgN/m2/d) */
        double frootn_store_to_frootn_transfer;         /* (kgN/m2/d) */
        double livestemn_store_to_livestemn_transfer;   /* (kgN/m2/d) */
        double deadstemn_store_to_deadstemn_transfer;   /* (kgN/m2/d) */
        double livecrootn_store_to_livecrootn_transfer; /* (kgN/m2/d) */
        double deadcrootn_store_to_deadcrootn_transfer; /* (kgN/m2/d) */

        /* turnover of live wood to dead wood, with retranslocation */
        double livestemn_to_deadstemn;        /* (kgN/m2/d) */
        double livestemn_to_retransn;         /* (kgN/m2/d) */
        double livecrootn_to_deadcrootn;      /* (kgN/m2/d) */
        double livecrootn_to_retransn;        /* (kgN/m2/d) */


} ;

/* annual nitrogen fluxes */
 struct nannflux_struct
{
    double npool_to_leafn_store;  /* (kgN/m2/yr) leaf N allocation */
    double npool_to_live_stemn;  /* (kgN/m2/yr) stem N allocation */
    double npool_to_live_crootn; /* (kgN/m2/yr) coarse root N allocation */
    double npool_to_frootn_store; /* (kgN/m2/yr) fine root N allocation */
    double npool_to_dead_stemn;  /* (kgN/m2/yr) follows excess C to dead wood */
    double npool_to_dead_crootn; /* (kgN/m2/yr) follows excess C to dead wood */
    double npool_to_luxn;        /* (kgN/m2/yr) return of luxury N */
    double live_stemn_to_dead_stemn; /* (kgN/m2/yr) live stem turnover */
    double live_crootn_to_dead_crootn; /* (kgN/m2/yr) live croot turnover */
    double live_stemn_to_npool;  /* (kgN/m2/yr) live stem N retranslocation */
    double live_crootn_to_npool; /* (kgN/m2/yr) live croot N retranslocation */
} ;


/* daily conductance multipliers */
struct mult_conduct_struct
        {
        double APAR;    /* 0-1 */
        double tavg;    /* 0-1 */
        double LWP;     /* 0-1 */
        double  CO2;    /* 0-1 */
        double  tmin;   /* 0-1 */
        double  vpd;    /* 0-1 */
        };

/* canopy ecophysiological constants */
/*--------------------------------------------------------------*/
/*      Notes:                                                  */
/*      lai_ratio currently IGNORED and the input projected     */
/*                and all sided lai and pai are used.           */
/*      int_coef  is assumed to be kg/m2/PAI/d                  */
/*--------------------------------------------------------------*/
struct epconst_struct
{

        double max_lai;       /* (m2/m2) maximum leaf area */
        double max_storage_percent; /* 0-1 */
        double min_percent_leafg; /* 0-1 */
        double proj_sla;   /* (m2/kgC) projected specific leaf area */
        double shade_sla_mult;   /* (DIM) ratio of shade sla to sunlit sla */
        double proj_swa;        /* (m2/kgC) all-sided specific stem wood area */
        double lai_ratio;      /* (DIM) all-sided LA / one-sided LA ratio */
        double int_coef;       /* (kg/m2/LAI/d) canopy precip interception coef */
        double ext_coef;       /* (DIM) canopy light extinction coefficient */
        double flnr;           /* (kg NRub/kg Nleaf) leaf N in Rubisco */
        double ppfd_coef;      /* (s/m2/umol) shape parameter for ppfd hyperbola */
        double topt;           /* (deg C) optimum air temperature for gs */
        double tmax;           /* (deg C) maximum air temperature for gs */
        double tcoef;          /* (NONE) coefficient for tempcurve function */
        double psi_open;       /* (MPa) psi at start of conductance reduction */
        double psi_close;      /* (MPa) psi at complete conductance reduction */
        double psi_threshold;      /* (MPa) psi at complete conductance reduction begins */
        double psi_slp;      /* (/MPa) slope of conductance reduction curve */
        double psi_intercpt;      /* (0-1) intercept of conductance reduction curve */
        double vpd_open;       /* (Pa) vpd at start of conductance reduction */
        double vpd_close;      /* (Pa) vpd at complete conductance reduction */
        double gl_smax;        /* (m/s) maximum leaf-scale stomatal conductance */
        double gl_c;           /* (m/s) leaf-scale cuticular conductance */
        double gl_bl;          /* (m/s) leaf-scale boundary layer conductance */
        double gs_tmin;            /* (deg C) lower temperature theshold for leaf onset */
        double gs_tmax;        /* (deg C) upper temperature theshold for leaf onset */
        double gs_trange;        /* (deg C) temperature range for leaf onset */
        double gs_vpd_min;         /* (Pa) lower vpd threshold for leaf onset */
        double gs_vpd_max;         /* (Pa) upper vpd threshold for leaf onset */
        double gs_vpd_range;       /* (Pa)  vpd range for leaf onset */
        double gs_dayl_min;        /* (s) lower day length threshold for leaf onset */
        double gs_dayl_max;        /* (s) upper day length threshold for leaf onset */
        double gs_dayl_range;      /* (s)  day length range for leaf onset */
        double gs_psi_min;         /* (mPa) lower soil moisture psi threshold for leaf onset mPa */
        double gs_psi_max;         /* (mPa) upper soil moisture psi  threshold for leaf onset mPa */
        double gs_psi_range;       /* (mPa)  psi range for leaf onset */
        double gs_ravg_days;       /* (days)  length of averaging window for gs controls  */
        double coef_CO2;        /* DIM 0-1  conductance sensitivity to CO2 */ 
        int day_leafon;        /* (DIM) yearday leaves on */
        int day_leafoff;       /* (DIM) yearday leaves off - set to 0 for no leaf drop cond.  */
        int ndays_expand;      /* (DIM) number of transition days leaf on to/from off  */
        int ndays_litfall;  /* (DIM) number of transition days for full leaf drop  */
        int phenology_flag;     /* (DIM) set as 1 for dynamic phenology */
        int allocation_flag;    /* (DIM) set as 1 for dynamic allocation */
        int veg_type;           /* (DIM) set as 1 for tree; 0 for grass */
        int phenology_type;     /* (DIM) set as 1 for decid; 0 for evergreen    */
        int nfix;               /* (DIM) set a 1 for n-fixers; 0 for not nfixers */
        int psi_curve;          /* (DIM) set to 0 for biome-bgc psi-conductance curve, other values give type of model 1=linear, 2=squared etc */
        int edible;             /* (DIM) set to 1 for edible plants */
        int  Tacclim;           /* (DIM) set to 1  for temperature acclimation of respiration Q10  */
    double gr_perc;        /* (DIM 0-1) percent of growth allocated to respiration */
    double leaf_turnover;     /* (1/yr) annual leaf turnover fraction */
    double livewood_turnover; /* (1/yr) annual live wood turnover fraction */
    double deadleaf_turnover; /* (1/yr) annual turnover of standing dead grass fraction */
    double froot_turnover;    /* (1/yr) annual fine root turnover fraction */
    double kfrag_base;        /* (1/day) daily coarse woody debris fragmentation rate */
    double storage_transfer_prop; /* (DIM) percent of storage allocated in a given year */
    double branch_turnover; /* (1/day) daily mortality turnover */
    double min_daily_mortality; /* (1/day) daily mortality turnover */
    double max_daily_mortality; /* (1/day) daily mortality turnover */
    double daily_mortality_threshold; /* years, age at which mortality starts to decline */
    double daily_fire_turnover; /* (1/day) daily fire loss */
    double froot_cn;     /* (kgC/kgN) C:N for fine roots */
    double leaf_cn;      /* (kgC/kgN) C:N for leaves */
    double livewood_cn;  /* (kgC/kgN) C:N for live wood */
    double leaflitr_cn;      /* (kgC/kgN) constant C:N for leaf litter */
    double leaflitr_flab;    /* (DIM) leaf litter labile fraction */
    double leaflitr_fucel;   /* (DIM) leaf litter unshielded cellulose fract. */
    double leaflitr_fscel;   /* (DIM) leaf litter shielded cellulose fract. */
    double leaflitr_flig;    /* (DIM) leaf litter lignin fraction */
    double litter_gsurf_intercept;  /* m/2 - intercept of conductance vs litter theta */
    double litter_gsurf_slope;  /* (DIM) - slope of conductance vs litter theta */
    double litter_moist_coef;  /* m/kg  water holding capacity of litter in m/kg/m2 */
        double litter_density;  /* kg/m  density of litter in kg/m/m2 */
    double frootlitr_flab;   /* (DIM) froot litter labile fraction */
    double frootlitr_fucel;  /* (DIM) froot litter unshielded cellulose fract */
    double frootlitr_fscel;  /* (DIM) froot litter shielded cellulose fract */
    double frootlitr_flig;   /* (DIM) froot litter lignin fraction */
    double deadwood_fucel;   /* (DIM) dead wood unshileded cellulose fraction */
    double deadwood_fscel;   /* (DIM) dead wood shielded cellulose fraction */
    double deadwood_flig;    /* (DIM) dead wood lignin fraction */
    double deadwood_cn;      /* (kgC/kgN) dead wood C:N (calc. internally) */
    double alloc_frootc_leafc; /* (ratio) new fine root C to new leaf C */
    double alloc_crootc_stemc; /* (ratio) new live croot C to new live stem C */
    double alloc_stemc_leafc; /* (ratio) new live stem C to new leaf C */
    double alloc_livewoodc_woodc; /* (ratio) new live wood to total wood C */
    double height_to_stem_coef;  /* (DIM) height vs stemC, parameter 1 (multiplier) */
    double height_to_stem_exp;  /* (DIM) height vs stemC, parameter 2 (exponent) */
    double phloemcsa_per_alllai; /* phloem CSA per unit all-sided LAI */
    double alloc_cpool_fhold;  /* (DIM) fraction cpool held for next year */
    double alloc_npool_fhold;  /* (DIM) fraction npool held for next year */
    double alloc_maxlgf;       /* (DIM) maximum leaf growth fraction */
    double alloc_prop_day_growth; /* (DIM) fraction of daily photosynthate used for daily growth */
    int dynamic_alloc_prop_day_growth; /* (0-1) 1 proportional allocated daily can be dynamic 0 static */
    double  min_leaf_carbon; /* kgC minimum leaf carbon before death */
    double  resprout_leaf_carbon; /* kgC leaf carbon to assign for resprouting */
    double root_growth_direction; /* (0-1) 1 is full vertical, 0 fully horizontal */
    double root_distrib_parm; /*  (DIM) used with root biomass in kg/m2 */
        double crown_ratio; /*  (DIM) ratio of crown height to total tree height */
    int     max_years_resprout; /* num years of resprouting before death */
    double waring_pa; /* parameter for Waring allometric equation */
    double waring_pb; /* parameter for Waring allometric equation */
    double dickenson_pa; /* parameter for Dickenson allometric equation */
    double Tacclim_days;  /* num days for temperature acclimation */
    double Tacclim_slp;  /* slope for temperature acclimation adjutment to Q10 */
    double Tacclim_intercpt;  /* intercept for temperature acclimation for temperature acclimation adjustment to Q10 */
} ;





        




        

/*----------------------------------------------------------*/
/*      Define a stratum default object.                                                */
/*----------------------------------------------------------*/
struct  stratum_default
        {
        int             ID;
        int     lai_end_day;                                            /*      days    */
        int     lai_start_day;                                          /*      days    */
        int     lai_seasonal_flag;                                      /* boolean      */
        double  albedo;                         /* unitless     */                      
        double  APAR_coef;                      /*  (m/s*LAI) ( s /m2 / umol photon )   */
        double  gsurf_slope;                    /* DIM */
        double  gsurf_intercept;                        /* DIM */
        double  K_absorptance;                  /* DIM */
        double  K_reflectance;                  /* DIM */
        double  K_transmittance;                /* DIM */
        double  lai_stomatal_fraction;          /* DIM  */
        double  min_heat_capacity;      /* J/m3/K */
        double  max_heat_capacity;      /* J/m3/K */
        double  PAR_absorptance;                /* DIM */ 
        double  PAR_reflectance;                /* DIM */
        double  PAR_transmittance;              /* DIM */
        double  specific_rain_capacity;                 /* m / LAI      */
        double  specific_snow_capacity;                 /* m /  LAI     */
        double  wind_attenuation_coeff;         /* 1/meters     */
        double  ustar_overu;                    /* DIM  */
        struct  epconst_struct  epc;
        struct  mrconst_struct  mrc;
	double pspread_loss_rel; 		/* Relation between probability of spread and percent carbon loss */
	double vapor_loss_rel; 			/* Relation between percent carbon loss and percent carbon vaporized */
	double biomass_loss_rel_k1; 		/* k1 for the biomass sigmoid function (steepness of curve) */
	double biomass_loss_rel_k2; 		/* k2 for the biomass sigmoid function (centerpoint of curve) */
};

/*----------------------------------------------------------*/
/*      Define target object                                */
/*----------------------------------------------------------*/
       struct target_object { 
              double lai;
              double total_stemc;
              double height;
              double age;
              int    met;
       };
/*----------------------------------------------------------*/
/*      Define accumulator object                           */
/*----------------------------------------------------------*/

        struct accumulate_strata_object {

        int length;
        double psn;
        double lai;
        double lwp;
        double minNSC;
        };


/*----------------------------------------------------------*/
/*      Define a canopy strata object.                      */
/*----------------------------------------------------------*/
struct  canopy_strata_object
        {
        int             patch_ID;
        int             ID;
        int             num_base_stations;                              
        double  APAR_direct;                                    /* (umol photon/m2*day) */
        double  APAR_diffuse;                                   /* */
        double  cover_fraction;
        double  dC13;           /* % discrimination of C13 */
        double  evaporation;                                    /* meters/day   */
        double  ga;                                             /* m/s          */
        double  gap_fraction;                                   /* unitless     */
        double  gs;                                             /* m/s          */
        double  potential_gs_sunlit;                                    /* m/s          */
        double  potential_gs_shade;                                     /* m/s          */
        double  gs_sunlit;                                      /* m/s          */
        double  gs_shade;                                       /* m/s          */
        double  gsurf;                                          /* m/s          */
        double  Kstar_direct;                                   /* Kj/(m2*day)  */
        double  Kstar_diffuse;                                  /* Kj/(m2*day)  */
        double  Lstar;                                          /* Kj/(m2*day)  */
        double  Lstar_night;									/* Kj/(m2*day)  */
        double  Lstar_day;										/* Kj/(m2*day)  */
        double  NO3_stored;                                     /* kg/m2        */
        double  PAR_after_reflection;                           /* (umol photon/m2*day) */
        double  ppfd_sunlit;                    /*  (umol/m2/s) PAR photon flux density */
        double  ppfd_shade;                     /*  (umol/m2/s) PAR photon flux density */
        double  potential_evaporation;                          /*  meters/day  */
        double  rain_stored;                                    /*  meters      */
        double  snow_stored;                                    /*  meters      */
        double  sublimation;                                    /*  meters      */
        double  surface_heat_flux;                              /*  kJ/day      */
        double  PET;                                            /*  m water/day */
        double  PE;                                             /*  m water/day */
        double  transpiration_unsat_zone;                       /*  m water/day */
        double  transpiration_sat_zone;                         /* m water /day */
        double  wind;                                           /* 1/meters     */
        double  canopy_drip;
        struct  rooting_zone_object     rootzone;
        struct  cdayflux_struct cdf;                            
        struct  cstate_struct   cs;
        struct  target_object   target;
        struct  epvar_struct epv;                               
        struct  nstate_struct   ns;
        struct  ndayflux_struct ndf;                            
        struct  phenology_struct phen;
        struct  base_station_object     **base_stations;
        struct  stratum_default **defaults;
        struct  spinup_default  **spinup_defaults;  
        struct  canopy_strata_hourly_object     *hourly;
        struct  accumulate_strata_object        acc_month;
        struct  accumulate_strata_object        acc_year;
        struct  mult_conduct_struct     mult_conductance;
        };


/*----------------------------------------------------------*/
/*      Define the canopy_strata hourly parameter structure */
/*----------------------------------------------------------*/
struct  canopy_strata_hourly_object
        {
        int i;
        };


/*----------------------------------------------------------*/
/*      Define the mortality fraction structure.        */
/*----------------------------------------------------------*/
struct mortality_struct
{
        double mort_cpool;
        double mort_leafc;
        double mort_deadleafc;
        double mort_livestemc;
        double mort_deadstemc;
        double mort_livecrootc;
        double mort_deadcrootc;
        double mort_frootc;
};


/*******************************************/
/* fire object specific to rhessys patch structure		*/
/*******************************************/
struct patch_fire_object 
{
	int num_patches;
	int tmp_patch; // which patch among the num_patches are we on?
	struct patch_object **patches;
	double *prop_patch_in_grid; /* proportion of total cell area occupied by this patch, this array matches the patch pointer array, for updating cell fuel and moisture values*/
	double *prop_grid_in_patch; 	/* 0-1, proportion of total patch area that overlaps with this cell, this array matches the patch pointer array, for updating patch mortality */
	double occupied_area; /*gives the total patch area in the current grid	*/
	struct fire_default_object *defaults;
	double elev; // elevation if read in from grid
};	

/*----------------------------------------------------------*/
/* Define Surface Temperature Object */
/*----------------------------------------------------------*/


struct surface_energy_object {

        double T;               /* degrees C */
        double depth; /* m */
        double moisture; /* m of water */
        double organic;         /* 0-1 */
        double quartz; /* 0-1 */
        double porosity;        /* 0-1 */
        double  psi_air_entry;                  /* m */
        double  pore_size_index;                                /* unitless */
};


        
/*----------------------------------------------------------*/
/* Define Surface Energy Default                        */
/*----------------------------------------------------------*/
struct surface_energy_default {
        int ID;
        int N_thermal_nodes;  
        int exp_dist;           /* 0, 1 */
        double damping_depth; /* m */
        double iteration_threshold; /* degrees C */
        };

#endif

