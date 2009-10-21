/*--------------------------------------------------------------*/
/*                                                              */
/*		create_flowpaths										*/
/*                                                              */
/*  NAME                                                        */
/*		 create_flowpaths										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 create_flowpaths( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*		-fl    roads to lowest flna interval 					*/
/*		-fh  	roads to highest flna interval					*/
/*		-s print drainage statistics							*/
/*		-r 	road flag for drainage statistics					*/
/*		-sc	stream connectivity is assumed						*/
/*			1	random slope value								*/
/*			2	internal slope value							*/
/*			0	no connectivity (default)						*/
/*		-sd scale dem values by this amount                     */
/*		-st	scale streamside transmissivity						*/
/*		-bi	basin ID				*/
/*		-sw	use a sewer image to route water from roads     	*/
/*		-pst	print stream table	*/
/*		-pre	input image file name prefix					*/
/*		-w 	road width (default is 5m)							*/
/*		-a arcview ascii data files (default is GRASS ascii)	*/
/*		-slp slope flag - change use of slope		            */
/*			in computation of gamma			                    */
/*			0  standard				                            */
/*			1  internal slpe of patch		                    */
/*			2 max slope of patch		                    	*/
/*		-o output file name (default -pre opt + _flow_table.dat	*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*  revision:  6.0  29 April, 2005                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "glb.h"
#include "sub.h"




main(int argc, char *argv[])  {
  

	
/* local variable declarations */
    int		i/*j*/ /*r*/ /*c*/ /*area*/, num_stream, num_patches;   
	FILE	/**in1*/ *out1, *out2;    
	int		basinid, tmp, maxr, maxc;
	double	cell, width;
	int		pst_flag, f_flag, fl_flag, fh_flag, vflag;
	int		s_flag, r_flag, slp_flag, sc_flag, arc_flag;
	int		st_flag, sewer_flag, ewhr_flag;
	int		prefix_flag;
	int		suffix_flag;
	double	scale_trans, scale_dem;
	char    *input_prefix;
	char    *output_suffix;
	float	scale_factor;



/* filenames for each image and file */

char    fndem[MAXS],  fnpartition[MAXS],    fnK[MAXS],     fnmpar[MAXS],
        fntable[MAXS], fnroot[MAXS], fnpatch[MAXS], fnhill[MAXS], fnzone[MAXS], fnslope[MAXS];
char	fnstream[MAXS];
char	fnroads[MAXS], fnsewers[MAXS];
char	fnflna[MAXS];
char	fnehr[MAXS], fnwhr[MAXS];
char	name[MAXS], name2[MAXS];


/* set pointers for images */

    int    *dem;
    int          *patch;	
    float        *slope;	
    int          *hill;	
    int          *zone;	
    int			 *stream;
    int			 *roads;
    int			 *sewers;
    float		 *K;
	float		 *m_par;
	float		 *flna;
	float		 *ehr;
	float		 *whr;

	struct		flow_struct	*flow_table;

    vflag    = 0;		/* verbose flag					 */
    fl_flag  = 0;		/* roads to lowest flna			 */
    fh_flag  = 0;		/* roads to highest flna		 */
	ewhr_flag	 = 0;	/* printing ew horizon flag			 */
	s_flag	 = 0;		/* printing stats flag			 */
	r_flag 	 = 0;		/* road stats flag				 */
	sc_flag  = 1;		/* stream connectivity flag		 */
	slp_flag = 0;		/* slope use flag		         */
	st_flag  = 0;		/* scaling stream side patches 	 */
	sewer_flag = 0;		/* route through a sewer network */
    arc_flag = 0;		/* arcview input data flag		 */
	scale_trans = 1.0;
	scale_dem = 1.0;	/* scaling for dem values        */
	prefix_flag = 0;	/* input prefix flag             */
	suffix_flag = 0;	/* output suffix flag            */
	pst_flag = 0;		/* print stream table flag            */
	cell = 30.0;		/* default resolution            */
	width = 5;			/* default road width            */
	basinid = 1;

	i = 1;

	while (i < argc) 
	{
		if (strcmp(argv[i],"-v") == 0 )
			{
			vflag = 1;
			}
		if (strcmp(argv[i], "-fl") == 0 )
			{
			fl_flag = 1;
			}
		if (strcmp(argv[i], "-pst") == 0 )
			{
			pst_flag = 1;
			}
		if (strcmp(argv[i], "-fh") == 0 )
			{
			fh_flag = 1;
			}
		if (strcmp(argv[i], "-ewhr") == 0 )
			{
			ewhr_flag = 1;
			}
		if (strcmp(argv[i], "-sw") == 0 )
			{
			sewer_flag = 1;
			}
			
		if (strcmp(argv[i], "-bi") == 0 )
			{
			i += 1;
			if   (i == argc)  {
                           fprintf(stderr,
				"FATAL ERROR: Basin ID not specified\n");
                                    exit(1);
                            } /*end if*/
			basinid = (int)atoi(argv[i]);
			}
		if (strcmp(argv[i], "-sc") == 0 )
			{
			i += 1;
			if   (i == argc)  {
                           fprintf(stderr,
				"FATAL ERROR: Output prefix not specified\n");
                                    exit(1);
                            } /*end if*/
			sc_flag = (int)atoi(argv[i]);
			}
		if (strcmp(argv[i], "-sd") == 0 )
			{
			i += 1;
			if (strncmp(argv[i], "-",1) != 0) 
				scale_dem = (double)atof(argv[i]); 
				printf("\n Using dem scaling of %s as %lf \n",
					argv[i], scale_dem);
			}
		if (strcmp(argv[i], "-slp") == 0 )
			{
			i += 1;
			if   (i == argc)  {
                           fprintf(stderr,
				"FATAL ERROR: Slope flag not specified\n");
                                    exit(1);
                            } /*end if*/
			slp_flag = (int)atoi(argv[i]);
			}
		if (strcmp(argv[i], "-st") == 0 )
			{
			st_flag = 1;
			i += 1;
			if (strncmp(argv[i], "-",1) != 0) 
				scale_trans = (double)atof(argv[i]); 
			printf("\n Using streamside scaling of %s as %lf \n",
					argv[i], scale_trans);
			}
		if (strcmp(argv[i], "-s") == 0 )
			{
			s_flag = 1;
			}

		if (strcmp(argv[i], "-w") == 0 )
			{
			i += 1;
			if   (i == argc)  {
                           fprintf(stderr,
				"FATAL ERROR: Output prefix not specified\n");
                                    exit(1);
                            } /*end if*/
			width = (double)atof(argv[i]);
			}

		if (strcmp(argv[i], "-res") == 0 )
			{
			i += 1;
			if   (i == argc)  {
                           fprintf(stderr,
				"FATAL ERROR: Output prefix not specified\n");
                                    exit(1);
                            } /*end if*/
			cell = (double)atof(argv[i]);
			}

		if (strcmp(argv[i], "-o") == 0 )
		{
				i += 1;
				if (i == argc) {
						fprintf(stderr, "FATAL ERROR: Output file name not specified\n");
						exit(1);
				} /* end if */
				/* allocate an array for the output suffix andread in output */
				/* suffix */
				if ((output_suffix = (char *)malloc((1+strlen(argv[i]))*sizeof(char))) == NULL)
				{
						fprintf(stderr, "FATAL ERROR: Cannot allocate output_suffix\n");
						exit(1);
				} /* end if */

				strcpy(output_suffix, argv[i]);
				suffix_flag = 1;
		} /* end if */

		if (strcmp(argv[i], "-pre") == 0 )
			{
			i += 1;
			if   (i == argc)  {
                           fprintf(stderr,
				"FATAL ERROR: Output prefix not specified\n");
                                    exit(1);
                            } /*end if*/

			
/*--------------------------------------------------------------*/
/*                      Allocate an array for the output prefix and */
/*                      Read in the output prefix   */
/*--------------------------------------------------------------*/
   if ((input_prefix =(char *)malloc((1+strlen(argv[i]))*sizeof(char))) ==	NULL) 
	{ 
	  	fprintf(stderr,	"FATAL ERROR: Cannot allocat output_prefix\n");
        exit(1);
              } /*end if*/

         strcpy(input_prefix,argv[i]);
         prefix_flag = 1;
			}

		if (strcmp(argv[i], "-r") == 0 )
			{
			r_flag = 1;
			}
		if (strcmp(argv[i], "-a") == 0 )
			{
			arc_flag = 1;
			}

		i += 1;

	}


	if (prefix_flag == 0) {
                fprintf(stderr,
			"\nFATAL ERROR: You must specify a prefix for image files\n");
                         exit(1);
         } /*end if*/
		
	if (suffix_flag == 0) {
			if ((output_suffix = (char *)malloc((1+strlen("_flow_table.dat"))*sizeof(char))) == NULL) {
					fprintf(stderr, "FATAL ERROR: Cannot allocate output_suffix\n");
					exit(1);
			}  /* end if */
			strcpy(output_suffix, "_flow_table.dat");
	} /* end if */

	if ((fl_flag) || (fh_flag))
		f_flag = 1;
	else
		f_flag = 0;

    printf("Create_flowpaths.C\n\n");

	
    input_prompt(&maxr, &maxc, input_prefix, fndem,fnslope,fnK,fnflna,
					fnpatch, fnzone,fnhill, fnstream, fnroads, fnsewers, fnmpar,fnpartition,
		 fntable,fnroot, fnehr, fnwhr, f_flag, sewer_flag, arc_flag);

	/* open some diagnostic output files */

  strcpy(name, input_prefix);
  strcat(name, ".build"); 
  if ( (out1 = fopen(name, "w")) == NULL)
        {
        printf("cannot open file \n");
        exit(1);
       	} 

  strcpy(name2, input_prefix);
  strcat(name2, ".pit"); 
  if ( (out2 = fopen(name2, "w")) == NULL)
        {
        printf("cannot open file \n");
        exit(1);
       	} 

	/* allocate and input map images */


	dem = (int *)malloc(maxr*maxc*sizeof(int));
	input_ascii_int(dem, fndem, maxr, maxc, arc_flag);

	patch   = (int *) calloc(maxr*maxc, sizeof(int));      
    	input_ascii_int(patch, fnpatch, maxr, maxc, arc_flag);	


	zone   = (int *) malloc(maxr*maxc*sizeof(int));

    input_ascii_int(zone, fnzone, maxr, maxc, arc_flag);	


    hill   = (int *) malloc(maxr*maxc*sizeof(int));
    input_ascii_int(hill, fnhill, maxr, maxc, arc_flag);	


    stream   = (int *) malloc(maxr*maxc*sizeof(int));
    input_ascii_int(stream, fnstream, maxr, maxc, arc_flag);	

   if ((sc_flag == 1) || (slp_flag > 0) ){ 

    	slope   = (float *) malloc(maxr*maxc*sizeof(int));
		scale_factor = 1;
    	input_ascii_float(slope, fnslope, maxr, maxc, arc_flag, scale_factor);	
	}


    roads = (int *) malloc(maxr*maxc*sizeof(int));
    input_ascii_int(roads, fnroads, maxr, maxc, arc_flag);	

    if (sewer_flag == 1) {
    	sewers = (int *) malloc(maxr*maxc*sizeof(int));
    	input_ascii_int(sewers, fnsewers, maxr, maxc, arc_flag);	
	}

	K = (float *) malloc(maxr*maxc*sizeof(float));
	scale_factor = (float)0.001;
    input_ascii_float(K, fnK, maxr, maxc, arc_flag, scale_factor);	


    m_par = (float *) malloc(maxr*maxc*sizeof(float));
	scale_factor = (float)0.001;
    input_ascii_float(m_par, fnmpar, maxr, maxc, arc_flag, scale_factor);	

	if (f_flag) {
		scale_factor = 1.0;
		flna = (float *)  malloc(maxr*maxc*sizeof(float));
		input_ascii_float(flna, fnflna, maxr, maxc, arc_flag, scale_factor);
		}
	else flna = NULL;


	/* create data structure and files for ew horizon if needed */
	if (ewhr_flag) {
		ehr = (float *)  malloc(maxr*maxc*sizeof(float));
		whr = (float *)  malloc(maxr*maxc*sizeof(float));
		}

	/* allocate flow table */
	flow_table = (struct flow_struct *)calloc((maxr*maxc),sizeof(struct flow_struct));
/*
	for (j=0; j< maxr*maxc; j++) {
		flow_table[j].adj_list = (struct adj_struct *)W_malloc(sizeof(struct adj_struct));
		flow_table[j].adj_ptr =  flow_table[j].adj_list;
		for (i=0; i< 100; i++) {
			flow_table[j].adj_ptr->next = (struct adj_struct *)W_malloc(sizeof(struct adj_struct));
			flow_table[j].adj_ptr = flow_table[j].adj_ptr->next;
			}
		}
*/

	if (ewhr_flag) {
		ew_horizon(dem, ehr, whr, maxr, maxc, cell, scale_dem);
		output_ascii_float(ehr, fnehr, maxc, maxr);
		output_ascii_float(whr, fnwhr, maxc, maxr);
		exit(0);
	}	

	printf("\n Building flow table");
	num_patches = build_flow_table(flow_table, dem, slope, hill, zone, patch, 
					stream, roads, sewers, K, m_par, flna, out1, maxr, 
					maxc,f_flag, sc_flag, sewer_flag, slp_flag, cell, 
                    scale_dem);

	fclose(out1);

	/* processes patches - computing means and neighbour slopes and gammas */
	printf("\n Computing gamma");
	num_stream = compute_gamma(flow_table, num_patches, out2,scale_trans,cell,sc_flag,
                        slp_flag);


	
	 /* remove pits and re-order patches appropriately */
	 
	printf("\n Removing pits");
	   remove_pits(flow_table, num_patches, sc_flag, slp_flag, cell, out2); 
	 
	/* add roads */
	printf("\n Adding roads");
	 add_roads(flow_table, num_patches, out2, cell);

	/* find_receiving patch for flna options */
	if (f_flag)
		route_roads_to_patches(flow_table, num_patches, fl_flag);
	

	printf("\n Computing upslope area");
 		tmp = compute_upslope_area(flow_table, num_patches, out2, r_flag,cell);

	if (s_flag == 1) {
		printf("\n Printing drainage stats");
		print_drain_stats(num_patches, flow_table);
		tmp = compute_dist_from_road(flow_table, num_patches, out2, cell);	
 		tmp = compute_drainage_density(flow_table, num_patches, cell);
		}
	

	printf("\n Printing flowtable");
	print_flow_table(num_patches,flow_table,sc_flag,slp_flag,cell,scale_trans,
                     input_prefix, output_suffix, width);

	if (pst_flag == 1) {
		printf("\n Printing  stream table");
		print_stream_table(num_patches,num_stream, flow_table,sc_flag,slp_flag,cell,scale_trans,
                     input_prefix, output_suffix, width, basinid);
		}

	
	fclose(out2);
	printf("\n Finished Createflowpaths \n\n");
    exit(0);


} /* end create_flowpaths.c */


