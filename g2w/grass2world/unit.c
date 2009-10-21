/*
  Macaque V2.6

  Copyright (C) Fred Watson, October, 1996

	Adapted C. Tague
*/

#include "macaque.h"
#include "fun.h"   /* chu */

int funcnumber(funcname)
	char	*funcname;
{
  int v;

  for(v=0;v<NUMFUNCS;v++)
    if(strcmp(funcname,FUNCNAME[v])==0)
      return(v);
  return(ERRORFUNCNUMBER);
}

int varnumber(vname, VARNAME)
	char *vname;
	char **VARNAME;
{
  char v;

  for(v=0;v<NUMVARS;v++) {
    if(strcmp(vname,VARNAME[v])==0)
      return(v);
    }

/*
    if  ( (VARNAME[v] = ( char *)malloc(200 * sizeof(char))) == NULL) {
        fprintf(stderr,"ERROR: Could not allocate VARIABLE NAME \n");
        exit(1);
	}
*/	
     	strcpy(VARNAME[v], vname);
	NUMVARS += 1;
	return(v);

}

