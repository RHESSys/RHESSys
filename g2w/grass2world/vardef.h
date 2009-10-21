/*
  Macaque V2.6
 
  Copyright (C) Fred Watson, September, 1996 
  Modifications	Christina Tague	January, 1997
 
*/

/*---------------------------------------------------------------------------
 THE FOLLOWING LINES DEFINE THE NAMING OF PARAMETERS.
 THIS IS THE ONLY PLACE WHERE THEY ARE EXPLICITLY MENTIONED.
 THIS SECTION OF CODE ***MUST*** BE CONSISTENT WITH ITSELF.
---------------------------------------------------------------------------*/
#define MAXVARNAME 200 
#define MAXNUMVARS 400
#define MAXFUNCNAME  50
#define NUMPARAMS	33
#define NUMWORKINGS 3 
#define NUMFLUXES	0 
#define NUMSTATES	38
#define NUMAREAS 1
/*
#define NUMVARS (NUMPARAMS + NUMWORKINGS + NUMFLUXES + NUMSTATES + NUMAREAS)
*/
#define NUMFUNCS 10
#define ERRORVARNUMBER -1
#define ERRORFUNCNUMBER -1

/* Not too sure why these had to be static but i got a 'multiply defined'
   error when they weren't.
*/

static int	NUMVARS = 0;

static char *FUNCNAME[NUMFUNCS] = {
	"aver",
	"daver",
	"area",
	"count",
	"eqn",
	"deqn",
	"value",
	"dvalue",
	"spavg",
	"mode",
	};

