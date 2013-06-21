/*
  Macaque V2.6
 
  Copyright (C) Fred Watson, February, 1996 
 
*/

#ifndef HDR
#define HDR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PROGNAME "Macaque V2.6"
#define MSGINTRO "Macaque V2.6\n\nCopyright (C) by Fred Waston, September, 1996\n\n"
#define MAXFILENAME 1024
#define DOUBLEABS(a) ((a)>0?(a):-(a))
#define SPLUSFIFOOUT "splusfifoout"
#define SPLUSFIFOIN  "splusfifoin"
#define DtoR 0.017453292 /* = pi/180 */
#define RtoD 57.29577951 /* = 180/pi */
#define FULL_DIAGNOSTIC writereportfulldiagnostic(master,in,nextin,loop,thevar)
#define ZERO_PLUS_NUMERICAL_DRFIT 1E-10
#define MAXCOMMAND ( (2 * MAXFILENAME) + 256 )

#include "leveldef.h"
#include "vardef.h"

struct masterstruct {
  char unitname[MAXFILENAME];
  char inname[MAXFILENAME];
  char outprefix[MAXFILENAME];
  FILE *outfile[NUMLEVELS];
  char reportname[MAXFILENAME];
  FILE *reportfile;
  char splusfifooutname[MAXFILENAME];
  char splusfifoinname[MAXFILENAME];
  double prefluxwaterbalancethreshold;
  double postfluxwaterbalancethreshold;
  long firstyear;
  long lastyear;
  char outputlevel;
  char outputunit[NUMLEVELS];
  char outputformat;
  char outputcycle;
  char outputprofilevar;
};

struct varlist {
  char index;
  double value;
  struct varlist *next;
};

struct unitstruct {
  int numsubunits;
  struct varlist *varlist;
  struct unitstruct *subunit;
};

struct instruct {
  double precip;
  double yesterdays_base_max_temp;
  double todays_base_min_temp;
  double obsflow;
};

struct loopstruct {
  int year;
  int day;
  int level[NUMLEVELS];
};

char *readcommandline();
FILE *startinfile();
double sigmoid();
double global_solar_energy_density();


#endif
