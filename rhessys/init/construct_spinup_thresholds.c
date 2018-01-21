/*--------------------------------------------------------------*/
/* 													                              			*/
/*					construct_spinup_thresholds               					*/
/*														                               		*/
/*	construct_spinup_thresholds.c - creates a patch object	   	*/
/*													                              			*/
/*	NAME										                             				*/
/*	construct_spinup_thresholds.c                           		*/
/*														                              		*/
/*	SYNOPSIS								                           					*/
/*	struct spinup_threshold_list_object construct_spinup_thresholds(    	*/
/*							struct basin_object *world)	                 		*/
/*														                              		*/
/* 														                              		*/
/*									                              							*/
/*	OPTIONS								                          						*/
/*												                              				*/
/*													                              			*/
/*	DESCRIPTION						                        							*/
/*									                              							*/
/*  reads spinup thresholds from input file				           		*/
/*	creates neighbourhood structure for each patch in the basin */
/*	returns a list giving order for patch-level assignment	  	*/
/*														                              		*/
/*												                               				*/
/*												                              				*/
/*	PROGRAMMER NOTES				                       							*/
/*												                              				*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rhessys.h"

void *construct_spinup_thresholds(char *spinup_thresholds_filename,
		  struct world_object *world)
													  
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.					                				*/
	/*--------------------------------------------------------------*/
	struct canopy_strata_object *find_stratum(int, int, int, int, int, struct world_object *); 	
	
	void *alloc(size_t, char *, char *);
  struct target_read {
    char name[120];
    double value;
  };	
// need to add read record function - no the function is defined in rhessys.h??

	/*--------------------------------------------------------------*/
	/*	Local variable definition.							                		*/
	/*--------------------------------------------------------------*/
	int	   	i,d,j;
	int		  num_stratum, num_targets;
	int		  stratum_ID, patch_ID, zone_ID, hill_ID, basin_ID;
	FILE	  *spinup_thresholds_file;
	struct  spinup_thresholds_list_object	*stlist;
	struct	canopy_strata_object	*strata;
  double  target_lai;
  double  target_total_stemc;
  double  target_height;
  double  target_age;
  struct  target_read *target_array;	
	char	  record[MAXSTR];
	
	/*--------------------------------------------------------------*/
	/*  Try to open the spinup_thresholds file in read mode.        */
	/*--------------------------------------------------------------*/
	if ( (spinup_thresholds_file = fopen(spinup_thresholds_filename,"r")) == NULL ){
		  fprintf(stderr,"FATAL ERROR:  Cannot open spinup_thresholds file %s\n",
		  spinup_thresholds_filename);
		  exit(EXIT_FAILURE);
	} /*end if*/

//create target array, times number of targets (readin first)
  fscanf(spinup_thresholds_file,"%d",&num_stratum);
  read_record(spinup_thresholds_file, record);  
  fscanf(spinup_thresholds_file,"%d",&num_targets);
  read_record(spinup_thresholds_file, record);

  target_array = (struct target_read *)alloc( sizeof(struct target_read)*num_targets, "target_array", "construct_spinup_thresholds");
  
  for (j=0; j< num_targets; j++){
    fscanf(spinup_thresholds_file, "%s", target_array[j].name 	);
    read_record(spinup_thresholds_file, record);
  }
	/*--------------------------------------------------------------*/
	/*	Read in  each stratum record and find it			          		*/
	/*	add it to the world level spinup_thresholds list	          */
	/*--------------------------------------------------------------*/
  read_record(spinup_thresholds_file, record);

  for (i=0; i< num_stratum; ++i) {  
		fscanf(spinup_thresholds_file,"%d %d %d %d %d",
			&basin_ID,
			&hill_ID,
			&zone_ID,
			&patch_ID,
			&stratum_ID);

    strata = find_stratum(stratum_ID, patch_ID, zone_ID, hill_ID, basin_ID, world);

	  if (strata == NULL) {
		  fprintf(stderr,	"FATAL ERROR: Could not find strata %d \n", stratum_ID);
			exit(EXIT_FAILURE);
    }

    strata->target.met = 0;
     
      for (j=0; j< num_targets; ++j) {
		      fscanf(spinup_thresholds_file,"%lf", &target_array[j].value);
            
        if( strcmp (target_array[j].name, "LAI") == 0 || strcmp (target_array[j].name, "lai") == 0){ 
          strata->target.lai = target_array[j].value;
        } 
        if( strcmp (target_array[j].name,"total_stemc") == 0 ) {
            strata->target.total_stemc = target_array[j].value;
        }
        if( strcmp (target_array[j].name, "height") == 0 ) {
            strata->target.height = target_array[j].value;
        }
        if( strcmp (target_array[j].name,"age") == 0 ) {
            strata->target.age = target_array[j].value;
        }
     }
  }

 
} /*end construct_spinup_thresholds.c*/

