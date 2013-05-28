
#define MAXTEMPLATELINE 100
#define INITIALWORLDFILENAME "world"
#define OUTFILENAME "world.new"
#define MAXFILENAME 100
#define MAXNAME 100
#define MAXEXTENT 10 
#define NUMLEVELS 6
#define NUMDEFLEVELS 6
#define BOTTOMLEVEL 5

static int  NUMVARS[6]= {0,6,6,12,30,53};

static char *LEVELNAME[NUMLEVELS] = {"world","basin","hillslope","zone","patch","stratum"};
static char *DEFLEVELNAME[NUMDEFLEVELS] = {"basin","hillslope","zone","soil","landuse","veg"};

struct lookuptablestruct {
	char 	name[MAXNAME];
	float 	*link;
	int	ID;
	int	level;
	float	newvalue;
	};

struct tlevelstruct {
	int ID;
	int level;
	int nchildren;
	char **namelist;
	float *valuelist;
	struct tlevelstruct **children;
	};


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
