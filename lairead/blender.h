/* 		BLENDER.H
        revision 6.0  29 April, 2005
*/

#define NAME   ("Create Flow Paths")

#define PI   3.1415927
#define SQRT2 1.4142135623

#define CELL   30		/* pixel dimensions in metres */

#define MAXS   80		/* size of filename string    */

#define FPTR_SIZE 8		/* number of elements in each pixel
				   for the fractional pointer	*/

#define LEN_GRASSHEADER 12
#define LEN_ARCHEADER	12 
#define MULTI_P   1.1		/* p paramter from Freeman (91) 
				   Comp. Geo. Sci. He suggests it
   				   should be 1.1  */


#define MAXCOL 200

#define ZERO	0.0000001
#define MIN_RISE 0.001

#define DtoR	  0.01745329 

#define STREAM_VAL  9999	/* set stream pixels to this value */



#define MAXTEMPLATELINE 100
#define INITIALWORLDFILENAME "world"
#define OUTFILENAME "world.new"
#define MAXFILENAME 100
#define MAXNAME 100
#define MAXEXTENT 10 
#define NUMLEVELS 6
#define NUMDEFLEVELS 6
#define BOTTOMLEVEL 5

static int  NUMVARS[6]= {0,6,7,12,32,54};

static char *LEVELNAME[NUMLEVELS] = {"world","basin","hillslope","zone","patch","stratum"};
static char *DEFLEVELNAME[NUMDEFLEVELS] = {"basin","hillslope","zone","soil","landuse","veg"};

/* STRUCTURE DEFINITIONS */

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



struct allom_struct
	{
	int	vegid;
	float	sla;
	float	lr;
	float	ls;
	float	lived;
	float	sc;
	float	cnl;
	float	cnr;
	float	cndw;
	float	cnlw;
	};

struct ID_struct
	{
	int	patch;
	int	hill;
	int	zone;
	};

struct	flow_struct
	{
	int	patchID;
	int	hillID;
	int	zoneID;
	int	ID_order;
	int area ;
	int vegid;
	float lai;
	struct allom_struct *veglink;
	struct tlevelstruct    *worldlink;
	};
