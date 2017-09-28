#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "params.h"

param * readParamFile(int *paramCnt, char *filename)
{

    /* Read all parameters from the specified parameter file into a character array.
       The parameters will be accessed by the calling program via the get* functions
       that follow.
       
       Parameters are of the format:

           <value> <name> <comment>

       for example:
       
           0.071   epc.alloc_livewoodc_woodc               # White p.28 - Mean value. Was 0.60

    */
   
    int paramInd = -1;
    int iParam;

    char line [1024];
    char strbuf1 [128];
    char strbuf2 [128];
    char strbuf3 [128];
    int argCnt;
    param *paramPtr = NULL;
    FILE *file;

    if ((file = fopen( filename, "r")) == NULL ) {
        fprintf(stderr,"ERROR: unable to open defaults file %s.\n", filename);
        /* exit(EXIT_FAILURE); */
        return NULL;
    }

    //FILE *file = fopen ( filename, "r" );
    if ( file != NULL ) {
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

            /* Parse the parameter value */
            strcpy(paramPtr[paramInd].strVal, strbuf1);

            /* Parse the parameter name */
            strcpy(paramPtr[paramInd].name, strbuf2);
            paramPtr[paramInd].accessed = 0;
            paramPtr[paramInd].defaultValUsed = 0;
            //printf("\n%d param name: %s value %s", *paramCnt, paramPtr[paramInd].name, paramPtr[paramInd].strVal);
        }

        fclose ( file );
    }
    else {
        perror ( filename );
    }

    return paramPtr;

}

char * getStrParam(int *paramCnt, param **paramPtr, char *paramName, char *readFormat, char *defaultVal, int useDefaultVal) {

    int iParam;
    int sLen;
    char *outStr;
    int found = 0;
    int paramInd;

    param *params;
    params = *paramPtr;

    /* Search for a parameter that matches the specified parameter name */
    for (iParam = 0; iParam < *paramCnt; iParam++) {
        if (strcmp(params[iParam].name, paramName) == 0) {
            found = 1;
            // Allocate an output string buffer that is the same size as the parameter value string
            sLen = string_length(params[iParam].strVal);
            outStr = (char *)malloc(sizeof(char) * sLen);
            // Transform the string according to the specified format
            sscanf(params[iParam].strVal, readFormat, outStr);
            params[iParam].accessed = 1;
            strcpy(params[iParam].format, readFormat);
            break;
        }
    }

    /* Return the requested parameter if found in the parameter list, otherwise return the default value. */
    if (found) {
        return outStr;
    } else if (useDefaultVal) {
        // Add this parameter to the list, as it wasn't found in the list
        if (*paramCnt == 0) {
            params = (param *) malloc(sizeof(param));
            *paramPtr = params;
        } else {
            params = (param *) realloc(params, (sizeof(param) * (*paramCnt + 1)));
            *paramPtr = params;
        }

        (*paramCnt)++;
        paramInd = *paramCnt - 1;

        /* Store the parameter name */
        strcpy(params[paramInd].name, paramName);

        /* Store the parameter value */
        strcpy(params[paramInd].strVal, defaultVal);
        params[paramInd].accessed = 1;
        params[paramInd].defaultValUsed = 1;
        strcpy(params[paramInd].format, readFormat);
        // Allocate an output string buffer that is the same size as the parameter value string
        sLen = string_length(defaultVal);
        outStr = (char *)malloc(sizeof(char) * sLen);
	sscanf(defaultVal,readFormat,outStr);
        
	return outStr;
    } else {
        printf("\nNo parameter value found for %s and 'useDefault' flag set to false\n", paramName);
    }
}

int getIntParam(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, int defaultVal, int useDefaultVal) {

    int iParam;
    int paramInd;
    int sLen;
    int intVal;
    int found = 0;
    param *params;
    params = *paramPtr;

    for (iParam = 0; iParam < *paramCnt; iParam++) {
        if (strcmp(params[iParam].name, paramName) == 0) {
            found = 1;
            // Transform the string according to the specified format
            sscanf(params[iParam].strVal, readFormat, &intVal);
            params[iParam].accessed = 1;
            strcpy(params[iParam].format, readFormat);
            break;
        }
    }

    if (found) {
        return intVal;
    } else if (useDefaultVal) {
        // Add this parameter to the list, as it wasn't found in the list
        if (*paramCnt == 0) {
            params = (param *) malloc(sizeof(param));
            *paramPtr = params;
        } else {
            params = (param *) realloc(params, (sizeof(param) * (*paramCnt + 1)));
            *paramPtr = params;
        }

        (*paramCnt)++;
        paramInd = *paramCnt - 1;

        /* Store the parameter name */
        strcpy(params[paramInd].name, paramName);

        /* Store the parameter value */
        sprintf(params[paramInd].strVal, "%d", defaultVal);
        params[paramInd].accessed = 1;
        params[paramInd].defaultValUsed = 1;
        strcpy(params[paramInd].format, readFormat);
        return defaultVal;
    } else {
        printf("\nNo parameter value found for %s and 'useDefault' flag set to false\n", paramName);
    }
}

float getFloatParam(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, float defaultVal, int useDefaultVal) {

    int iParam;
    int paramInd;
    int sLen;
    float floatVal;
    int found = 0;
    param *params;
    params = *paramPtr;

    for (iParam = 0; iParam < *paramCnt; iParam++) {
        if (strcmp(params[iParam].name, paramName) == 0) {
            found = 1;
            // Transform the string according to the specified format
            sscanf(params[iParam].strVal, readFormat, &floatVal);
            params[iParam].accessed = 1;
            strcpy(params[iParam].format, readFormat);
            break;
        }
    }

    if (found) {
        return floatVal;
    } else if (useDefaultVal) {
        // Add this parameter to the list, as it wasn't found in the list
        if (*paramCnt == 0) {
            params = (param *) malloc(sizeof(param));
            *paramPtr = params;
        } else {
            params = (param *) realloc(params, (sizeof(param) * (*paramCnt + 1)));
            *paramPtr = params;
        }

        (*paramCnt)++;
        paramInd = *paramCnt - 1;

        /* Store the parameter name */
        strcpy(params[paramInd].name, paramName);

        /* Store the parameter value */
        sprintf(params[paramInd].strVal, "%f", defaultVal);
        params[paramInd].accessed = 1;
        params[paramInd].defaultValUsed = 1;
        strcpy(params[paramInd].format, readFormat);
        return defaultVal;
    } else {
        printf("\nNo parameter value found for %s and 'useDefault' flag set to false\n", paramName);
    }
}

double getDoubleParam(int *paramCnt, param **paramPtr, char *paramName, char *readFormat, double defaultVal, int useDefaultVal) {

    int iParam;
    int paramInd;
    int sLen;
    double doubleVal;
    int found = 0;
    param *params;
    params = *paramPtr;

    for (iParam = 0; iParam < *paramCnt; iParam++) {
        if (strcmp(params[iParam].name, paramName) == 0) {
            found = 1;
            // Transform the string according to the specified format
            sscanf(params[iParam].strVal, readFormat, &doubleVal);
            params[iParam].accessed = 1;
            strcpy(params[iParam].format, readFormat);
            break;
        }
    }

    if (found) {
        return doubleVal;
    } else if (useDefaultVal) {
        // Add this parameter to the list, as it wasn't found in the list
        if (*paramCnt == 0) {
            params = (param *) malloc(sizeof(param));
            *paramPtr = params;
        } else {
            params = (param *) realloc(params, (sizeof(param) * (*paramCnt + 1)));
            *paramPtr = params;
        }

        (*paramCnt)++;
        paramInd = *paramCnt - 1;

        /* Store the parameter name */
        strcpy(params[paramInd].name, paramName);

        /* Store the parameter value */
        sprintf(params[paramInd].strVal, "%f", defaultVal);
        params[paramInd].accessed = 1;
        params[paramInd].defaultValUsed = 1;
        strcpy(params[paramInd].format, readFormat);
        return defaultVal;
    } else {
        printf("\nNo parameter value found for %s and 'useDefault' flag set to false\n", paramName);
    }
}

void printParams(int paramCnt, param *params, char *outFilename) {

    /* Output all parameters that were used by RHESSys. This includes all parameters that
       were retrieved from *params (i.e. by a call to one of the get*Param functions) or
       that had a default value set because no value could be found in a parameter file.

       Parameters that were read from a .def file but were not accessed are not printed out.
    */


    int iParam;
    char outFormat[64];

    FILE *outFile;

    if ((outFile = fopen(outFilename, "w")) == NULL ){
    	fprintf(stderr, "FATAL ERROR:Error opening output parameter filename %s\n", outFilename);
    	exit(EXIT_FAILURE);
    }
    
    for (iParam = 0; iParam < paramCnt; iParam++) {
        /* Print the parameter value with the format that was specified when the parameter was read, i.e. call to getIntParameter */
        if (params[iParam].accessed) {
            fprintf(outFile, "%s %s\n", params[iParam].strVal, params[iParam].name);
        }
    }

    fclose(outFile);
}

int string_length(char *s)
{
   int c = 0;
 
   while(*(s+c))
      c++;
 
   return c;
}


/*-----------------------------------------------------------------------------
 *  function used to read variables in worldfile
 *-----------------------------------------------------------------------------*/

char * getStrWorldfile(int *paramCnt, param **paramPtr, char *paramName, char *readFormat, char *defaultVal, int useDefaultVal) {

    int iParam;
    int sLen;
    char *outStr;
    int found = 0;
    int paramInd;

    param *params;
    params = *paramPtr;

    /* Search for a parameter that matches the specified parameter name */
    for (iParam = 0; iParam < *paramCnt; iParam++) {
        if (strcmp(params[iParam].name, paramName) == 0) {
            found = 1;
            // Allocate an output string buffer that is the same size as the parameter value string
            sLen = string_length(params[iParam].strVal);
            outStr = (char *)malloc(sizeof(char) * sLen);
            // Transform the string according to the specified format
            sscanf(params[iParam].strVal, readFormat, outStr);
            params[iParam].accessed = 1;
            strcpy(params[iParam].format, readFormat);
            break;
        }
    }

    /* Return the requested parameter if found in the parameter list, otherwise return the default value. */
    if (found) {
        return outStr;
    } else if (useDefaultVal) {
        return defaultVal;
    } else {
        printf("\nNo parameter value found for %s and 'useDefault' flag set to false\n", paramName);
    }
}

int getIntWorldfile(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, int defaultVal, int useDefaultVal) {

    int iParam;
    int paramInd;
    int sLen;
    int intVal;
    int found = 0;
    param *params;
    params = *paramPtr;

    for (iParam = 0; iParam < *paramCnt; iParam++) {
        if (strcmp(params[iParam].name, paramName) == 0) {
            found = 1;
            // Transform the string according to the specified format
            sscanf(params[iParam].strVal, readFormat, &intVal);
            params[iParam].accessed = 1;
            strcpy(params[iParam].format, readFormat);
            break;
        }
    }

    if (found) {
        return intVal;
    } else if (useDefaultVal) {
        return defaultVal;
    } else {
        printf("\nNo parameter value found for %s and 'useDefault' flag set to false\n", paramName);
    }
}

float getFloatWorldfile(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, float defaultVal, int useDefaultVal) {

    int iParam;
    int paramInd;
    int sLen;
    float floatVal;
    int found = 0;
    param *params;
    params = *paramPtr;

    for (iParam = 0; iParam < *paramCnt; iParam++) {
        if (strcmp(params[iParam].name, paramName) == 0) {
            found = 1;
            // Transform the string according to the specified format
            sscanf(params[iParam].strVal, readFormat, &floatVal);
            params[iParam].accessed = 1;
            strcpy(params[iParam].format, readFormat);
            break;
        }
    }

    if (found) {
        return floatVal;
    } else if (useDefaultVal) {
        return defaultVal;
    } else {
        printf("\nNo parameter value found for %s and 'useDefault' flag set to false\n", paramName);
    }
}

double getDoubleWorldfile(int *paramCnt, param **paramPtr, char *paramName, char *readFormat, double defaultVal, int useDefaultVal) {

    int iParam;
    int paramInd;
    int sLen;
    double doubleVal;
    int found = 0;
    param *params;
    params = *paramPtr;

    for (iParam = 0; iParam < *paramCnt; iParam++) {
        if (strcmp(params[iParam].name, paramName) == 0) {
            found = 1;
            // Transform the string according to the specified format
            sscanf(params[iParam].strVal, readFormat, &doubleVal);
            params[iParam].accessed = 1;
            strcpy(params[iParam].format, readFormat);
            break;
        }
    }

    if (found) {
        return doubleVal;
    } else if (useDefaultVal) {
        return defaultVal;
    } else {
        printf("\nNo parameter value found for %s and 'useDefault' flag set to false\n", paramName);
    }
}
