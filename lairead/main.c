/*--------------------------------------------------------------*/
/*                                                              */
/*		lairead										*/
/*                                                              */
/*  NAME                                                        */
/*		 lairead										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 lairead( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-pre	input image file name prefix					*/
/*		-a arcview ascii data files (default is GRASS ascii)	*/
/*		-allom name of allometric ratios file		*/
/*		-old	old worldfile name			*/
/*		-redef 	new redefine worldfile name			*/
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

// Prints out program arguments documentation
void usage(void);


main(int argc, char *argv[]) 
{
	/* local variable declarations */
    int		i, nvegtype, num_patches;   
	FILE	 *out1, *out2, *fac;    
	FILE	*oldworld, *redefine;
	int	tmp, maxr, maxc;
	double	cell, width;
	int	prefix_flag, oldworld_flag, redefine_flag;
	int	arc_flag, suffix_flag, allom_flag;
	char    *input_prefix;
	char    *output_suffix;
	char	*allocate_fname, *oldworld_fname, *redefine_fname;
	float	scale_factor;



	/* filenames for each image and file */

	char    fnlai[MAXS], fnvegid[MAXS],  fntable[MAXS], fnroot[MAXS];
	char    fnpatch[MAXS], fnhill[MAXS], fnzone[MAXS];
	char	name[MAXS], name2[MAXS];


	/* set pointers for images */

    int		*vegid;
    int     *patch;	
    int     *hill;	
    int     *zone;	
    float	*lai;

    struct	allom_struct	*allometric_table;
	struct	flow_struct	*flow_table;

   	arc_flag = 0;		/* arcview input data flag		 */
	prefix_flag = 0;	/* input prefix flag             */
	suffix_flag = 0;	/* output suffix flag            */
	cell = 30.0;		/* default resolution            */

	i = 1;
	while (i < argc) {
		if (strcmp(argv[i], "-allom") == 0 ) {
			i += 1;
			if (i == argc) {
				fprintf(stderr, "FATAL ERROR: Output file name not specified\n");
				usage();
				exit(1);
			}
			/* allocate an array and read in allocation file name*/
			if ((allocate_fname = (char *)malloc((1+strlen(argv[i]))*sizeof(char))) == NULL) {
				fprintf(stderr, "FATAL ERROR: Cannot allocate output_suffix\n");
				usage();
				exit(1);
			} 

				strcpy(allocate_fname, argv[i]);
				allom_flag = 1;
		}

		/* **************** */
		/* worldfile flag */
		/* **************** */
		if (strcmp(argv[i], "-old") == 0 )
		{
			i += 1;
			if (i == argc) {
				fprintf(stderr, "FATAL ERROR: worldfile file name not specified\n");
				usage();
				exit(1);
			} 
	
			/* allocate an array and read in allocation file name*/
			if ((oldworld_fname = (char *)malloc((1+strlen(argv[i]))*sizeof(char))) == NULL) {
				fprintf(stderr, "FATAL ERROR: Cannot allocate output_suffix\n");
				usage();
				exit(1);
			}
		
		strcpy(oldworld_fname, argv[i]);
			oldworld_flag = 1;
		}

		/* **************** */
		/* redefine output worldfile flag */
		/* **************** */
		if (strcmp(argv[i], "-redef") == 0 )
		{
			i += 1;
			if (i == argc) {
				fprintf(stderr, "FATAL ERROR: redefine file name not specified\n");
				usage();
				exit(1);
			}

		/* allocate an array and read in allocation file name*/
			if ((redefine_fname = (char *)malloc((1+strlen(argv[i]))*sizeof(char))) == NULL) {
				fprintf(stderr, "FATAL ERROR: Cannot allocate output_suffix\n");
				usage();
				exit(1);
			} 

			strcpy(redefine_fname, argv[i]);
			redefine_flag = 1;
		}

		/* **************** */
		/* prefix flag */
		/* **************** */
		if (strcmp(argv[i], "-pre") == 0 ) {
			i += 1;
			if (i == argc) {
            	fprintf(stderr, "FATAL ERROR: Output prefix not specified\n");
				usage();
                exit(1);
            }

		/*--------------------------------------------------------------*/
		/*           Allocate an array for the input prefix and 		*/
		/*             Read in the input  prefix  					 	*/
		/*--------------------------------------------------------------*/
		    if ((input_prefix =(char *)malloc((1+strlen(argv[i]))*sizeof(char))) ==	NULL) { 
				fprintf(stderr,	"FATAL ERROR: Cannot allocat output_prefix\n");
				exit(1);
		  	}

	 		strcpy(input_prefix,argv[i]);
	 		prefix_flag = 1;
		}

		if (strcmp(argv[i], "-a") == 0 ) {
			arc_flag = 1;
		}

		i += 1;
	} /* end while */

	/* **************** */
	/*	deal with any missing filename inputs  */
	/*	and open world, redefine and allometric files */
	/* **************** */

	if (prefix_flag == 0) {
	    fprintf(stderr, "\nFATAL ERROR: You must specify a prefix for image files\n");	
		usage();
        exit(1);
    }
		
	if (suffix_flag == 0) {
		if ((output_suffix = (char *)malloc((1+strlen("_flow_table.dat"))*sizeof(char))) == NULL) {
			fprintf(stderr, "FATAL ERROR: Cannot allocate output_suffix\n");
			usage();
			exit(1);
		}
		strcpy(output_suffix, "_flow_table.dat");
	} 

	/* allometric file */
	if (allom_flag == 0) {
                fprintf(stderr,
			"\nFATAL ERROR: You must specify an allocation  file\n");
                         exit(1);
    }

  	if ( (fac = fopen(allocate_fname, "r")) == NULL) {
        	printf("cannot open allometric ratio file %s\n", allocate_fname);
        	exit(1);
	} 

	/* old worldfile */
	if (oldworld_flag == 0) {
		if ((oldworld_fname = (char *)malloc((1+strlen("worldfile"))*sizeof(char))) == NULL) {
			fprintf(stderr, "FATAL ERROR: Cannot allocate oldworld\n");
			usage();
			exit(1);
		} 
		strcpy(oldworld_fname, "worldfile");
	} 
  	
	if ( (oldworld = fopen(oldworld_fname, "r")) == NULL) {
       	printf("cannot open old world file %s\n", oldworld_fname);
		usage();
       	exit(1);
	} 

	/* redefine world */
	if (redefine_flag == 0) {
		if ((redefine_fname = (char *)malloc((1+strlen("world.Y1990M1D1H1"))*sizeof(char))) == NULL) {
			fprintf(stderr, "FATAL ERROR: Cannot allocate redefine\n");
			exit(1);
		} 
		strcpy(redefine_fname, "world.Y1990M1D1H1");
	} 
  	
	if ( (redefine = fopen(redefine_fname, "w")) == NULL) {
       	printf("cannot open new redefine world %s\n", redefine_fname);
		usage();
       	exit(1);
	} 
	
	/* suffix flag */	
	if (suffix_flag == 0) {
		if ((output_suffix = (char *)malloc((1+strlen("_flow_table.dat"))*sizeof(char))) == NULL) {
			fprintf(stderr, "FATAL ERROR: Cannot allocate output_suffix\n");
			usage();
			exit(1);
		}
		strcpy(output_suffix, "_flow_table.dat");
	} 

	/*--------------------------------------------------------------*/
	/*	created input files 										*/
	/*--------------------------------------------------------------*/
	
    input_prompt(&maxr, &maxc, input_prefix, fnlai, fnvegid, fnpatch, fnzone,fnhill, fntable,fnroot, arc_flag);

	/* open some diagnostic output files */

	strcpy(name, input_prefix);
	strcat(name, ".log"); 
	if ( (out1 = fopen(name, "w")) == NULL) {
        printf("cannot open file %s \n", name);
		usage();
        exit(1);
   	} 

    input_prompt(&maxr, &maxc, input_prefix, fnlai, fnvegid, fnpatch, fnzone,fnhill, fntable,fnroot, arc_flag);

	/* open some diagnostic output files */
	strcpy(name, input_prefix);
	strcat(name, ".build"); 
	if ( (out1 = fopen(name, "w")) == NULL) {
        printf("cannot open file %s \n", name);
		usage();
        exit(1);
   	} 

	/* allocate and input map images */

	printf("\nReading %s", fnvegid);
	vegid = (int *)malloc(maxr*maxc*sizeof(int));
	input_ascii_int(vegid, fnvegid, maxr, maxc, arc_flag);

	printf("\nReading %s", fnpatch);
	patch = (int *) calloc(maxr*maxc, sizeof(int));      
   	input_ascii_int(patch, fnpatch, maxr, maxc, arc_flag);	

	printf("\nReading %s", fnzone);
	zone = (int *) malloc(maxr*maxc*sizeof(int));
   	input_ascii_int(zone, fnzone, maxr, maxc, arc_flag);	

	printf("\nReading %s", fnhill);
    hill = (int *) malloc(maxr*maxc*sizeof(int));
    input_ascii_int(hill, fnhill, maxr, maxc, arc_flag);	

	printf("\nReading %s", fnlai);
	lai = (float *) malloc(maxr*maxc*sizeof(float));
	scale_factor = 1.0;
   	input_ascii_float(lai, fnlai, maxr, maxc, arc_flag, scale_factor);	

	/* allocate flow table */
	flow_table = (struct flow_struct *)calloc((maxr*maxc),sizeof(struct flow_struct));

	/* build representation of patches */
	num_patches = build_flow_table(out1, flow_table, vegid, lai, hill, zone, patch, maxr, maxc);

	/* read in allometric ratios table */
	fscanf(fac,"%d", &nvegtype);
	allometric_table = (struct allom_struct *)calloc(nvegtype, sizeof(struct allom_struct));	
	read_allom_table(fac, nvegtype, allometric_table);

	/* link patches with allometry */
	printf("\n Linking vegetation and allometry");
	link_patch_veg(flow_table, allometric_table, num_patches, nvegtype);
	
	/* now read in and change the worldfile */
	change_world(oldworld, redefine, flow_table, num_patches);

//	printf("\n Ready to print flowtable");
//	print_flow_table(num_patches, flow_table, input_prefix, output_suffix);

	printf("\n Finished Lairead \n\n");
    exit(0);
} /* end lairead.c */


void usage(void) {
	printf("\nOPTIONS\n\n");
	printf("\t-pre\tinput image file name prefix\n");					
	printf("\t-a\tarcview ascii data files (default is GRASS ascii)\n");	
	printf("\t-allom\tname of allometric ratios file\n");
	printf("\t-old\told worldfile name\n");
	printf("\t-redef\tnew redefine worldfile name\n");

	return;
}
