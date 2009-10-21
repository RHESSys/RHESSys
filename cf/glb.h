
/*	GLOBAL VARIABLES	*/


/* filenames for each image and file */


/* command line options */
int	vflag;


/* outlet coordinates */

int  startr, startc;

/* pointer coordinates (to the outlet) */

int  rpt, cpt;


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



 


