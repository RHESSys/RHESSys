/* 		BLENDER.H

*/

#define NAME   ("Create Flow Paths")

#define PI   3.1415927

/* #define RtoD	57.2958  //chu */

#define SQRT2 1.4142135623

#define CELL   30		/* pixel dimensions in metres */

#define MAXS   80		/* size of filename string    */

#define FPTR_SIZE 8		/* number of elements in each pixel
				   for the fractional pointer	*/

#define MULTI_P   1.1		/* p paramter from Freeman (91) 
				   Comp. Geo. Sci. He suggests it
   				   should be 1.1  */

#define MAXCOL 200

#define STREAM_VAL  9999	/* set stream pixels to this value */

#define FILL_DEPTH    20	/* when filling pits do not fill all
				   the way to top of basin. elevation
				   to stop at is pitel + FILL_DEPTH  

	NOTE: This was changed to 30 from 20 to facilitate the yahoo dem
	      and then changed back again...

                  hillslope -->  /      
                                /       
                               /   <-- stop filling here      -
                              /           		      |
 pour point at               /				      | FILL_DEPTH
 elevation pouht -->    /\  /             		      |
                       /  \/     <-- pit at elevation 'pitel' -
                      /
*/


/* STRUCTURE DEFINITIONS */

struct	adj_struct 
	{
	int	ID;
	int	num;
	double perimeter;
	double slope;
	double gamma;
	struct adj_struct *next;
	};

struct	flow_struct
	{
	int	ID;
	int	ID_order;
	int area ;
	double x ;
	double y ;
	double z ;
	double K ;
	double m_par ;
	double total_gamma;
	int	num_adjacent ;
	struct adj_struct *adj_list;
	struct adj_struct *adj_ptr;
	};

/*	GLOBAL VARIABLES	*/


/* filenames for each image and file */

char  	fnhdr[MAXS],    fndem[MAXS],  fnpartition[MAXS],    fnK[MAXS],     fnmpar[MAXS],
	fntable[MAXS], fnroot[MAXS];

/* command line options */
int	vflag;

/* image size */

int  maxr, maxc;

/* outlet coordinates */

int  startr, startc;

/* pointer coordinates (to the outlet) */

int  rpt, cpt;

/* cell size (linear, m), replaces #define CELL */

int     cell;

/* slope threshold */

float  flat;

/* pit threshold */

int  maxpit;

/* drainage area threshold */

int  dthreshold;

/* maximum of the lna image, used for reporting in *.info */

float  maxlna;


static int expo[3][3] = {
             {4, 3, 2},
             {5, 0, 1},
             {6, 7, 8}  };
 
static int drain[3][3] = {
             {8, 7, 6},
             {1, 0, 5},
             {2, 3, 4}   };
 


static int rvec[10] = { 0, 0, -1, -1, -1,  0,  1, 1, 1, 0};
static int cvec[10] = { 0, 1,  1,  0, -1, -1, -1, 0, 1, 1};



 


