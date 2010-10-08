/*
  Macaque V2.5
 
  Copyright (C) Fred Watson, February, 1996
 
*/

#include <stdio.h>
#include "macaque.h"
#include "fun.h"    /* chu */

void fprintloop(fptr,loop)
	FILE *fptr;
	struct loopstruct *loop;
{
  char t;

  fprintf(fptr,"y%d ",loop->year);
  fprintf(fptr,"d%d ",loop->day);
  for(t=0;t<NUMLEVELS;t++)
    fprintf(fptr,"%c%d ",LEVELNAME[t][0],loop->level[t]);
  fprintf(fptr,"\n");
}

void msgnoloop(msg)
	char *msg;
{
  fprintf(stderr,"message: %s\n",msg);
}

void msg(loop,msg)
	struct loopstruct *loop;
	char *msg;
{   
  fprintloop(stderr,loop);
  msgnoloop(msg);
}

void warningnoloop(msg)
	char *msg;
{
  fprintf(stderr,"warning: %s\n",msg);
}

void warning(loop,msg)
	struct loopstruct *loop;
	char *msg;
{
  fprintloop(stderr,loop);
  warningnoloop(msg);
}

void error(msg)
	char *msg;
{
  fprintf(stderr,"error: %s\n",msg);
  exit(1);
}

void d(a)
	int a;
{
  fprintf(stderr,"debug mark: %d\n",a);
}

int yes(answer)
	char *answer;
{
  return((answer[0]=='y')||((answer[0]=='Y')||(answer[0]=='\0')));
}

int leapyear(year)
	int year;
{
  if(year%4==0)
    return(1);
  else
    return(0);
}

/* Header Routines */

#define MAXTEMPLATELINE 200

void 	readheader(templatefile, header)
	FILE	*templatefile;
	struct 	headerstruct	*header;
	
{
	struct date tm;
	int num_def;
	int i,j;
	char line[MAXTEMPLATELINE];


	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
		error("Reading template file.");
	sscanf(line,"%d %d %d %d",&tm.year,&tm.month,&tm.day,&tm.hour);
	header->start_date = tm;

	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
		error("Reading template file.");
	sscanf(line,"%d %d %d %d",&tm.year,&tm.month,&tm.day,&tm.hour);
	header->end_date = tm;

	for (i=0; i<NUMLEVELS; ++i) {
		fscanf(templatefile,"%d", &num_def);
		header->defaults[i].num_defaults = num_def;
		header->defaults[i].filenames = (char **)calloc(num_def, sizeof(char *));
		for (j=0; j<num_def; ++j)  {
			header->defaults[i].filenames[j] = (char *)malloc(MAXFILENAME*sizeof(char));
			fscanf(templatefile,"%s", header->defaults[i].filenames[j]);
		}
	}

	fscanf(templatefile,"%d", &num_def);
	header->num_base_stations= num_def;
	header->base_station_filenames = (char **)calloc(num_def, sizeof(char *));
	for (j=0; j<num_def; ++j) {
		header->base_station_filenames[j] = (char *)malloc(MAXFILENAME*sizeof(char));
		fscanf(templatefile,"%s\n", header->base_station_filenames[j]);
	}
}


void 	outputheader(outfile, header)
	FILE	*outfile;
	struct 	headerstruct	*header;

{
  	int i,j;

	fprintf(outfile,"%-20d start_year\n%-20d month\n%-20d day \n%-20d hour",
		header->start_date.year,
		header->start_date.month,
		header->start_date.day,
		header->start_date.hour);

	fprintf(outfile,"\n%-20d end_year\n%-20d month\n%-20d day \n%-20d hour",
		header->end_date.year,
		header->end_date.month,
		header->end_date.day,
		header->end_date.hour);

	for (i=0; i<NUMLEVELS; ++i) {
		
		/*	Handle landuse. Somehow landuse got inserted into the template file
			between patch and stratum, this results in the tags in the worldfile
			being incorrect, where land_use is labled as stratum, and stratum as 
			(null). Using the special case i=5 to deal with this works without
			breaking existing template files, but makes the format more brittle.
			The next time the template is updated, this should be re-ordered and
			moved to handling land use defs down below along with climate base
			stations
		*/
		if (i==4) { // landuse special case
			fprintf(outfile, "\n%-20d num_landuse_files", header->defaults[i].num_defaults);
			for (j=0; j < header->defaults[i].num_defaults; ++j)
				fprintf(outfile, "\n%-20s landuse_default_filename", header->defaults[i].filenames[j]);
		} else if (i==5) {
			fprintf(outfile,"\n%-20d num_%s_files",	header->defaults[i].num_defaults,
							LEVELNAME[i]);
			for (j=0; j< header->defaults[i].num_defaults; ++j)  
				fprintf(outfile,"\n%-20s %s_default_filename", header->defaults[i].filenames[j],
						LEVELNAME[i]);
		} else { // all others act like normal
			fprintf(outfile,"\n%-20d num_%s_files",	header->defaults[i].num_defaults,
							LEVELNAME[i+1]);
			for (j=0; j< header->defaults[i].num_defaults; ++j)  
				fprintf(outfile,"\n%-20s %s_default_filename", header->defaults[i].filenames[j],
						LEVELNAME[i+1]);
		}
	}

	// Landuse should be done here
	fprintf(outfile,"\n%-20d num_base_stations",	header->num_base_stations);
	for (j=0; j< header->num_base_stations; ++j)  
		fprintf(outfile,"\n%-20s base_station_filename", header->base_station_filenames[j]);

	fprintf(outfile,"\n");
}


