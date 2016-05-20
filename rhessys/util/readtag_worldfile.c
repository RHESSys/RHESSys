/*
 * =====================================================================================
 *
 *       Filename:  readtag_worldfile.c
 *
 *    Description:  Read the state variables for basins in the worldfile, return the 
 *		    parameter pointer which stores these state value and names, it will
 *		    stop reading when reach the variable "n_basestation"
 *
 *        Version:  1.0
 *        Created:  04/26/2015 17:03:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "params.h"
#include "phys_constants.h"

param *readtag_worldfile(int *paramCnt, FILE *file,char *key){
    int paramInd = -1;
    int iParam;

    char line [1024];
    char strbuf1 [128];
    char strbuf2 [128];
    char strbuf3 [128];
    int argCnt;
    param *paramPtr = NULL;
    int num_variables=0;

    
    /*-----------------------------------------------------------------------------
     *  number of state variables in each hierachic level is specified in phys_constants.h
     *-----------------------------------------------------------------------------*/
    if (strcmp(key,"Basin")==0){
      //it is reading the 7 state variables for basin, 
      num_variables = NUM_VAR_BASIN;
    }
    else if (strcmp(key,"Hillslope")==0){
      //in this case, it is reading the 8 state variables for hillslope
      num_variables = NUM_VAR_HILLSLOPE;
    }
    else if(strcmp(key,"Zone")==0){
      //it is reading the 12 state variables for zones
      num_variables = NUM_VAR_ZONE;
    }
    else if(strcmp(key,"Patch")==0){
      //it is reading the 37 state variables for patch
      num_variables = NUM_VAR_PATCH;
    }
    else if(strcmp(key,"Canopy_Strata")==0){
      // reading the 56 state variables for strata
      num_variables = NUM_VAR_STRATA;
    }
    
    
    
    paramPtr = (param *)malloc(sizeof(param) * (num_variables + 1));

    // Char array that will hold parameter names and values (as strings)
    //FILE *file;
        while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */ {
            // Char array that will hold parameter names and values (as strings)
            (*paramCnt)++;
            paramInd++;

            //printf("paramInd: %d\n", paramInd);

            /* Reset string buffers */
            strbuf1[0] = '\0';
            strbuf2[0] = '\0';
            strbuf3[0] = '\0';
            argCnt = sscanf (line, "%s %s %s", strbuf1, strbuf2, strbuf3);
	    //printf("argCnt=%d, strbuf1=%s, strbuf2=%s,strbuf3=%s\n",argCnt,strbuf1,strbuf2,strbuf3);
            /* Parse the parameter value */

            strcpy(paramPtr[paramInd].strVal, strbuf1);

            /* Parse the parameter name */
            strcpy(paramPtr[paramInd].name, strbuf2);
            paramPtr[paramInd].accessed = 0;
            paramPtr[paramInd].defaultValUsed = 0;
	    if(strcmp(strbuf2,"n_basestations")==0){
	      break;
	    }
	    if(num_variables < *paramCnt){
	      printf("num_variables=%d,paramCnt=%d\n",num_variables,*paramCnt);
	      fprintf(stderr,"added new parameter, adjust the num_variables in phys_constants.h\n");
	    }

            //printf("\n%d param name: %s value %s", *paramCnt, paramPtr[paramInd].name, paramPtr[paramInd].strVal);
        }

        //fclose ( file );
    
    return paramPtr; 
}

