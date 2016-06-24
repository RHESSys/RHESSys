#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char name[32];    /* The name field of the parameter. */
    char strVal[1024]; /* The parameter as it was read in from the file. */
    char format[8]; /* The default format to read this parameter with. */
    int accessed; /* Has the program accessed this parameter? */
    int defaultValUsed; /* Was the passed in default value used for this parameter? */
} param;

/* Function prototypes */
param * readParamFile(int *paramCnt, char *filename);
char * getStrParam(int *paramCnt, param **paramPtr, char *paramName, char *readFormat, char *defaultVal, int useDefaultVal);
int    getIntParam(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, int defaultVal, int useDefaultVal);
float  getFloatParam(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, float defaultVal, int useDefaultVal);
double getDoubleParam(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, double defaultVal, int useDefaultVal);
void   printParams(int paramCnt, param *params, char *outFilename);
int string_length(char *s);


char * getStrWorldfile(int *paramCnt, param **paramPtr, char *paramName, char *readFormat, char *defaultVal, int useDefaultVal);
int    getIntWorldfile(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, int defaultVal, int useDefaultVal);
float  getFloatWorldfile(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, float defaultVal, int useDefaultVal);
double getDoubleWorldfile(int *paramCnt, param **paramPtr , char *paramName, char *readFormat, double defaultVal, int useDefaultVal);


#endif
