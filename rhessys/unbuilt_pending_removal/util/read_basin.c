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

param *read_basin(int *paramCnt, FILE *file){
    int paramInd = -1;
    int iParam;

    char line [1024];
    char strbuf1 [128];
    char strbuf2 [128];
    char strbuf3 [128];
    int argCnt;
    param *paramPtr = NULL;
    //FILE *file;
        while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */ {
            // Char array that will hold parameter names and values (as strings)
            if (*paramCnt == 0) {
                paramPtr = (param *) malloc(sizeof(param));
            } else {
                paramPtr = (param *) realloc(paramPtr, (sizeof(param) * (*paramCnt + 1)));
            }
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
	    if(strcmp(strbuf2,"hillslope_ID")==0){
	      fprintf(stderr,"Num of hillslopes need to be specified in worldfile!\n");
	    }

            //printf("\n%d param name: %s value %s", *paramCnt, paramPtr[paramInd].name, paramPtr[paramInd].strVal);
        }

        //fclose ( file );
    return paramPtr; 
}

