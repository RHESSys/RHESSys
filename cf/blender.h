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

#define	STREAM 1
#define ROAD 1
#define SEWER 4
#define OUTFALL 1
#define LAND 0

#define MAXCOL 200

#define ZERO	0.0000001

#define MIN_RISE 0.001

#define DtoR	  0.01745329 

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


struct ID_struct
	{
	int	patch;
	int	hill;
	int	zone;
	};

struct	adj_struct 
	{
	int	patchID;
	int	hillID;
	int	zoneID;
	int	inx;
	int	landtype;
	int	sewertype;
	double perimeter;
	float slope;
	float gamma;
	float z;
	struct adj_struct *next;
	};

struct	flow_struct
	{
	int	patchID;
	int	hillID;
	int	zoneID;
	int	ID_order;
	int area ;
	int land;
	int sewer;
	float x ;
	float y ;
	float z ;
	float flna;
	float acc_area;
	float total_str_gamma;
	float total_gamma;
	float gamma_neigh;
	float total_perimeter;
	float road_dist;    
	float slope;
	float internal_slope;
	float max_slope;
	double path_length;
	int	num_str;
	int	inflow_cnt;
	int	num_adjacent ;
	int	num_dsa;
	int	stream_inx;
	float stream_gamma;
	struct	ID_struct stream_ID;
	struct adj_struct *adj_list;
	struct adj_struct *adj_ptr;
	struct adj_struct *adj_str_ptr;
	struct adj_struct *adj_str_list;
	};
