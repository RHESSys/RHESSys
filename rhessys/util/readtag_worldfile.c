/*
 * =====================================================================================
 *
 *       Filename:  read_basin.c
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

param *readtag_worldfile(int *paramCnt, FILE *file,char *next_key){
    int paramInd = -1;
    int iParam;

    char line [1024];
    char strbuf1 [128];
    char strbuf2 [128];
    char strbuf3 [128];
    int argCnt;
    param *paramPtr = NULL;
    int num_variables=0;


    if (strcmp(next_key,"hillslope_ID")==0){
      //it is reading the 7 state variables for basin, 
      num_variables = 7;
    }
    else if (strcmp(next_key,"zone")==0){
      //in this case, it is reading the 8 state variables for hillslope
      num_variables = 8;
    }
    else if(strcmp(next_key,"patch_ID")==0){
      //it is reading the 12 state variables for zones
      num_variables = 12;
    }
    else if(strcmp(next_key,"canopy_strata_ID")==0){
      //it is reading the 37 state variables for patch
      num_variables = 37;
    }
    else{
      // reading the 56 state variables for strata
      num_variables = 56;
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
	    if(strcmp(strbuf2,next_key)==0){
	      fprintf(stderr,"Num of hillslopes need to be specified in worldfile!\n");
	    }
	    if(num_variables < *paramCnt){
	      printf("num_variables=%d,paramCnt=%d\n",num_variables,*paramCnt);
	      fprintf(stderr,"added new parameter, adjust the num_variables in readtag_worldfile.c\n");
	    }

            //printf("\n%d param name: %s value %s", *paramCnt, paramPtr[paramInd].name, paramPtr[paramInd].strVal);
        }

        //fclose ( file );
    return paramPtr; 
}

