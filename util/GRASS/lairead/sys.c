/*
  Macaque V2.5
 
  Copyright (C) Fred Watson, February, 1996
 
*/

#include <stdio.h>
#include "blender.h"
#include "sub.h"    /* chu */



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


void 	readheader(templatefile, header, legacy_world)
  int     legacy_world;
	FILE	*templatefile;
	struct 	headerstruct	*header;
	
{


	struct date tm;
	int num_def;
	int i,j;
	char line[MAXTEMPLATELINE];
	char name[MAXTEMPLATELINE];


	// If we're reading a legacy worldfile we need to read the date
	if ( legacy_world ) {
	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   error("Reading template file. 1");
	sscanf(line,"%d %s",&tm.year,name);
	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   error("Reading template file. 2");
	sscanf(line,"%d %s",&tm.month,name);
	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   error("Reading template file. 3");
	sscanf(line,"%d %s",&tm.day,name);
	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   error("Reading template file. 4");
	sscanf(line,"%d %s",&tm.hour,name);
	header->start_date = tm;
	

	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   error("Reading template file. 5");
	sscanf(line,"%d %s",&tm.year,name);
	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   error("Reading template file. 6");
	sscanf(line,"%d %s",&tm.month,name);
	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   error("Reading template file. 7");
	sscanf(line,"%d %s",&tm.day,name);
	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   error("Reading template file. 8");
	sscanf(line,"%d %s",&tm.hour,name);
	header->end_date = tm;

	}

	for (i=0; i<NUMDEFLEVELS; ++i) {
		if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   		error("Reading template file. 9");
		sscanf(line,"%d %s", &num_def,name);
		header->defaults[i].num_defaults = num_def;
		header->defaults[i].filenames = (char **)calloc(num_def, sizeof(char *));
		for (j=0; j<num_def; ++j)  {
			header->defaults[i].filenames[j] = (char *)malloc(MAXFILENAME*sizeof(char));
			if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   			error("Reading template file. 10");
			sscanf(line,"%s %s",header->defaults[i].filenames[j],name);
			}
		}

	if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
		error("Reading template file. 11");
	sscanf(line,"%d %s", &num_def,name);
	header->num_base_stations= num_def;
	header->base_station_filenames = (char **)calloc(num_def, sizeof(char *));
	for (j=0; j<num_def; ++j) {
		header->base_station_filenames[j] = (char *)malloc(MAXFILENAME*sizeof(char));
		if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
	   			error("Reading template file. 12");
		sscanf(line,"%s %s",header->base_station_filenames[j],name);
		}


}


void 	outputheader(outfile, header)
	FILE	*outfile;
	struct 	headerstruct	*header;

{

  int i,j;

	fprintf(outfile,"\n%-20d start_year\n%-20d month\n%-20d day \n%-20d hour",
		header->start_date.year,
		header->start_date.month,
		header->start_date.day,
		header->start_date.hour);

	fprintf(outfile,"\n%-20d end_year\n%-20d month\n%-20d day \n%-20d hour",
		header->end_date.year,
		header->end_date.month,
		header->end_date.day,
		header->end_date.hour);

	for (i=0; i<NUMDEFLEVELS; ++i) {
		fprintf(outfile,"\n%-20d num_%s_files",	header->defaults[i].num_defaults,
						DEFLEVELNAME[i]);
		for (j=0; j< header->defaults[i].num_defaults; ++j)  
			fprintf(outfile,"\n%-20s %s_default_filename", header->defaults[i].filenames[j],
					DEFLEVELNAME[i]);
		}

	fprintf(outfile,"\n%-20d num_base_stations",	header->num_base_stations);
	for (j=0; j< header->num_base_stations; ++j)  
		fprintf(outfile,"\n%-20s base_station_filename", header->base_station_filenames[j]);

	fprintf(outfile,"\n");
	return;
	
}


