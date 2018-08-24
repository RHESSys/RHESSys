/*
  Macaque V2.6
 
  Copyright (C) Fred Watson, October, 1996 
  Modifications	Chris Tague, Jan. 1997
 
*/

/*--------------------------------------------------------------------------
 THE FOLLOWING LINES DEFINE THE NAMING OF SPATIAL STRUCTURES (UNITS).
 THIS IS THE ONLY PLACE THAT THE NAMING AND CHARACTERISTICS OF SPATIAL
 UNITS ARE MENTIONED.
 THIS SECTION OF CODE ***MUST*** BE CONSISTENT WITH ITSELF.
--------------------------------------------------------------------------*/
#define MAXLEVELNAME 20
#define NUMLEVELS 6
#define TOPLEVEL 0
#define BOTTOMLEVEL (NUMLEVELS - 1)
static char *LEVELNAME[NUMLEVELS] = {"world","basin","hillslope","zone","patch","canopy_strata"};


 struct	date {
	int	year;
	int	month;
	int	day;
	int hour;
	};

struct defaultnamestruct {
	int num_defaults;
	char    **filenames;
	};

struct headerstruct {
	struct date    start_date;
	struct date    end_date;
	int     num_base_stations;
	char    **base_station_filenames;
	struct  defaultnamestruct   defaults[NUMLEVELS];
	};


