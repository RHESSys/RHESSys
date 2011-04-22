/* raveragetables
 
	Based on r.average and altered extensively by Fred Watson:

   Description:

	Outputs a table of areas, cell counts, area averages of cover maps,
	crossreferenced category counts, and text with respect to the
	categories of a number of defined basemaps.

   Usage:

	raveragetables table=[b,B,V,S,L,A,C,X,T][filename],[etc,...,...]:

   ...where one of b,B,A,C,V,S,L,X, or T must prefix each filename and any
   number (max=30) of filenames can be listed.

   Example:

	raveragetables table=Bmap1,C,Vmap2

	Outputs, for each category in map1, the number of cells of that
	category and the average value of map2 within that category.

	Ordering of output columns is as in the command line.

   Options:

	bthe_map  = defines a silent basemap (its category values are not output)
	Bthe_map  = defines a basemap
	A         = area of each category in the basemap(s)
	C         = cell count of each category in the basemap(s)
	Vthe_map  = average value of the_map within each category of the basemap(s)
        Sthe_map  = total value of the_map within each category of the basemap(s)
	Mthe_map = mode of the_map within each category of the basemap(s)
	Lthe_map  = as above but uses category labels in the_map
	Xthe_map  = number of categories in the_map within each category of the basemap(s)
	Tthe_text = outputs the_text 

   Flags:

	The -z flag causes rows with basemap categories of zero (printed
	or not) to be excluded from output.

	The -n flag causes the number of rows to be printed before the table
	is printed.

	The -k flag causes category values in V, S or L files whose name contains
	the characters "1k" to be divided by 1000 in all calculations.
	Such files are multiplied by 1000 (1k) in the first place to overcome
	GRASS's displike of floating point raster maps.

   Notes:

	This program was written to provide topographic export data for input
	to the RHESSys hydro-ecological modelling system. Of course, its
	functions are applicable to a wider range of tasks.

   Copyright (C) by Fred Watson, May, 1994.
   (original code may be copyright elsewhere)

	modified for PC Backland GRASS C.Tague June, 2000
	(now includes mode function)
	modified for erroneous hillslope Lynn Ford, 2003
 
	Changed long to int for compatibility issues, AD, April 2011
*/


#include	<grass/gis.h>
#include	<grass/gisdefs.h>
#include	<stdio.h>        
#include	<stdlib.h>       
#include	<string.h>       

#define	STATS		"r.stats -i"
#define RECLASS		"r.reclass"
#define MAXCOLS		500

int iscovermap(int, char[MAXCOLS]);             
int isrealcovermap(int, char[MAXCOLS]);         
int isbasemap(int , char[MAXCOLS]);             
int isothertype(int , char[MAXCOLS]);           

main(argc, argv)
int		argc;
char *argv[];
{
    char	*me = argv[0];   
    char	command[1024];
    char	newcommand[1024];
    char	*mapset;
    struct	Option		*tableopt;
	struct	Option		*fileopt;
	struct	Option		*logopt;	
    struct	Flag		*zeroflag;	
    struct	Flag		*kflag;     
    struct	Flag		*fflag;     
    struct	Categories	cats;
    int		basecat[MAXCOLS];		
    int		oldbasecat[MAXCOLS];	
    int		covercat;				
    int		sum3;				   
    double	x;
    double	area;
    double	sum1;
    double	sum2;
    double	covervalue[MAXCOLS];
    double	max;
    double	kfactor;
	double	amax;
	int		stat;
	int		i;
	int		j;
	int		numcols;
	int		firstcovermap;
	int		gotazero;
	int		formattedoutput;
	int		covermap;				
	int		basemap;				
	int		numrows;				
	int		finalnumrows;			
	int		changed_bases;			
	int		first_time;				
	int		hangin_in_there;		
	int		status;					
	int		log_rat;				
	char	*tempfile1;
	char	*tempfile2[MAXCOLS];
	char	*tempfile3;
	char	*name;
	char	buf[80];
	FILE	*fd1;
	FILE	*fd2;
	FILE	*fd3;
	FILE	*the_file[MAXCOLS];
	FILE	*logfile;
	char	type[MAXCOLS];
	char	*table[MAXCOLS];
	char	temp_table[MAXCOLS];
	char	*ch;
	char	temp_str1[2500];

	/* INITIALIZATION AND PARSER SETUP ************************************/

    G_gisinit(argv[0]);
	//G_begin_cell_area_calculations();

	tableopt = G_define_option();
	tableopt->key       = "table";
	tableopt->type      = TYPE_STRING;
	tableopt->required  = YES ;
	tableopt->gisprompt  = "old,cell,raster" ;
	tableopt->description = "table column(s)";
	tableopt->multiple  = YES;

	fileopt = G_define_option();
	fileopt->key = "file";
	fileopt->type = TYPE_STRING;
	fileopt->required = NO;
	fileopt->multiple = NO;
	fileopt->description = "Output file name";

	logopt = G_define_option();
	logopt->key = "log";
	logopt->type = TYPE_STRING;
	logopt->required = NO;
	logopt->multiple = NO;
	logopt->description = "Log file name";

	zeroflag = G_define_flag();
	zeroflag->key = 'z';
	zeroflag->description = "exclude rows with any zero basemap categories";


	kflag = G_define_flag();
	kflag->key = 'k';
	kflag->description = "divide cover files whose name contains the chars '1k' by 1000";

	fflag = G_define_flag();
	fflag->key = 'f';
	fflag->description = "format output in nice columns";

	if (G_parser(argc,argv))
		exit(1);  

	// open log
	log_rat = 0;

	/* if(strcmp(logopt->answer, ""))
	{
		status = open_log(&logfile, logopt->answer);
		if(status == 0)
		{
			sprintf (temp_str1, "executing ravgtables\n");
			log_output(logfile, temp_str1);
			log_rat = 1;
		}
		else
			printf("could not open logfile %s\n", logopt->answer);
	}
	*/
	/* ERROR CHECKING ***********************************************/

	basemap = 0;
	covermap = 0;
	for (i = 0; name = tableopt->answers[i]; i++)
    {
		/*debugging*/

		if (i==MAXCOLS)
		{
			if(log_rat)
			{
				sprintf(temp_str1,"%s: Only works with up to %d columns\n", me, MAXCOLS);
				log_output(logfile, temp_str1);
			}
			exit(1);
		}
		type[i] =* name;
		table[i] = name + 1;

		/*debugging
		fprintf(stderr,"table %d is: %s of type  %c %d\n",i,table[i], type[i],isrealcovermap(i,type)); 
		ch=NULL;
		ch=getchar();*/
	
		if ((isrealcovermap(i,type) == 1)||(isbasemap(i,type) == 1))
		{
			
			if(isrealcovermap(i,type) == 1)
			{
				covermap=1;
				if(log_rat)
				{
					sprintf (temp_str1, "real cover map found\n");
					log_output(logfile, temp_str1);
				}
			}
			if(isbasemap(i,type) == 1)
			{
				basemap=1;
				if(log_rat)
				{
					sprintf (temp_str1, "base map found\n");
					log_output(logfile, temp_str1);
				}
			}
			// have to use temp_table because G_find_cell alters it (p.100 of manual)
			strcpy(temp_table, table[i]);
			if (!(mapset = G_find_cell(temp_table, "")))
			{
				printf("Raster map %s not found\n", table[i]);	
				/* Logging is currently broken, but want to get this error message out
				if(log_rat)
				{
					sprintf (temp_str1, "%s: %s - raster map not found\n", me, table[i]);
					log_output(logfile, temp_str1);
				}
				*/
				exit(1);
			}
			else
			{
				if(log_rat)
				{
					sprintf (temp_str1, "found cell\n");
					log_output(logfile, temp_str1);
				}
			}
		}
		else
		{
			if(iscovermap(i,type) == 1)
			{
				covermap=1;
				if(log_rat)
				{
					sprintf (temp_str1, "real cover map found\n");
					log_output(logfile, temp_str1);
				}
			}
			else
			{
				if(isothertype(i,type) == 0)
				{
					if(log_rat)
					{
						sprintf(temp_str1,"%s: Undefined column type: %c\n", me, type[i]);
						log_output(logfile, temp_str1);
					}
					exit(1);
				}
			}
		}
	}	/* end error checking */

	if(basemap == 0)
	{
		if(log_rat)
		{
			sprintf(temp_str1,"%s: Must have at least one basemap (type b or B)\n", me);
			log_output(logfile, temp_str1);
		}
		exit(1);
    }

	/* Pretty sure we dont need this
	if(covermap==0)
	{
		fprintf(stderr,"%s: Must have at least one non-basemap (type A,C,V,S,L,X, or T)\n",me);
		exit(1);
	}
	*/

    numcols = i;
	if(log_rat)
	{
		sprintf (temp_str1, "number of columns set to %d\n", numcols);
		log_output(logfile, temp_str1);
	}

/* AND... WE'RE AWAY - USING r.stats TO FILL UP THE RAW DATA FILES****************/

	firstcovermap = -1;
	printf("\n Determining r.stats command in rat");
	for (covermap = 0; covermap < numcols; covermap++)
	{
		if((iscovermap(covermap,type) == 0) && (isothertype(covermap,type) == 0))
			continue;
		if(firstcovermap < 0)
			firstcovermap = covermap;

		tempfile1 = G_tempfile();
		tempfile2[covermap] = G_tempfile();
		sprintf(command, "%s ", STATS);
		if(type[covermap]=='M')
			strcat(command, "-i ");
		if(type[covermap]=='C')
			strcat(command, "-c input=");
		else if(isothertype(covermap, type) == 1)
			strcat(command, "input=");
		else
			strcat(command, "-a input=");
		for (basemap = 0; basemap < numcols; basemap++)
		{
			if (isbasemap(basemap, type) == 0)
				continue;
			strcat(command, table[basemap]);
			strcat(command, ",");
		}
		if(isrealcovermap(covermap,type) == 1)
			strcat(command, table[covermap]);
		strcat(command, " fs=space output=");
		strcat(command, tempfile1);
	
		/*debugging*/

		if(log_rat)
		{
			sprintf (temp_str1, "executing command - %s\n", command);
			log_output(logfile, temp_str1);
		}

		printf ("\nexecuting command - %s\n", command);
		system(command);
		sprintf(newcommand, "%s ", "sed 's/*/0/g' < ");
		strcat(newcommand, tempfile1);
		strcat(newcommand, " > tmp ");
		printf ( "\nexecuting command - %s\n", newcommand);
		system(newcommand);
		sprintf(newcommand, "\\mv tmp ");
		strcat(newcommand, tempfile1);
		system(newcommand);


		fd1 = fopen (tempfile1, "r");
		fd2 = fopen (tempfile2[covermap], "w");
		if (fd1 == NULL || fd2 == NULL)
		{
			if(log_rat)
			{
				sprintf (temp_str1, "file open failure for %s(read) or %s(write), unlinking files\n", tempfile1, tempfile2[covermap]);
				log_output(logfile, temp_str1);
			}

			unlink(tempfile1);
			for(i = 0; i <= covermap; i++)
            if(iscovermap(i, type) == 1)        
                unlink(tempfile2[i]);
			exit(1);
		}
		else
		{
			if(log_rat)
			{
				sprintf (temp_str1, "opened %s(read) and %s(write)\n", tempfile1, tempfile2[covermap]);
				log_output(logfile, temp_str1);
			}
		}
    
		if (kflag->answer)
			kfactor = strstr(table[covermap], "1d") ? 10.0:
				strstr(table[covermap], "1c") ? 100.0:
				strstr(table[covermap],"1k") ? 1000.0: 1.0 ;
		else
			kfactor=1.0;

		if (fflag->answer)
			formattedoutput=1;
		else
			formattedoutput=0;
		sum1 = 0.0;
		sum2 = 0.0;
		amax = 0.0;
		max = 0;
		sum3 = 0;
		numrows = 0;
		hangin_in_there = 1;
		first_time = 1;
		while (hangin_in_there)
		{
			if(log_rat)
			{
				sprintf (temp_str1, "iteration count for covermap type X = %d\n", sum3);
				log_output(logfile, temp_str1);
			}
			changed_bases = 0;
			for (basemap = 0; basemap < numcols; basemap++)
			{
				if (isbasemap(basemap, type) == 0)
					continue;

				if (fscanf(fd1, "%ld", &(basecat[basemap])) != 1)
				{
					hangin_in_there = 0;
					if(log_rat)
					{
						sprintf (temp_str1, "no longer hanging in there I\n");
						log_output(logfile, temp_str1);
					}
					break;
				}
				else
				{
					if(log_rat)
					{
						sprintf (temp_str1, "read basecat[%d] = %ld\n", basemap, basecat[basemap]);
						log_output(logfile, temp_str1);
					}
				}

				if (first_time == 1)
					oldbasecat[basemap] = basecat[basemap];
			    else
				{
					if (oldbasecat[basemap] != basecat[basemap])
					{
						changed_bases = 1;
						if(log_rat)
						{
							sprintf (temp_str1, "changed bases\n");
							log_output(logfile, temp_str1);
						}
					}
				}
			}
			if(isrealcovermap(covermap, type) == 1)
			{
				if (fscanf(fd1, "%d %lf", &covercat, &area) != 2)
				{
					hangin_in_there = 0;
					if(log_rat)
					{
						sprintf (temp_str1, "no longer hanging in there II\n");
						log_output(logfile, temp_str1);
					}
				}
				else
				{
					if(log_rat)
					{
						sprintf (temp_str1, "read covercat = %ld area = %lf\n", covercat, area);
						log_output(logfile, temp_str1);
					}
				}
			}
			else if(isothertype(covermap, type) == 1)
			{
				/* I guess I do nuthin here? */
			}
			else
			{
				if (fscanf(fd1, "%lf", &area) != 1)
				{
					hangin_in_there = 0;
					if(log_rat)
					{
						sprintf (temp_str1, "no longer hanging in there III\n");
						log_output(logfile, temp_str1);
					}
				}
				else
				{
					if(log_rat)
					{
						sprintf (temp_str1, "read area = %lf\n", area);
						log_output(logfile, temp_str1);
					}
				}
				covercat = 1;
				if(log_rat)
				{
					sprintf (temp_str1, "read area = %lf\n", area);
					log_output(logfile, temp_str1);
				}
			}
			first_time = 0;
	
			if ((changed_bases == 1) || (hangin_in_there == 0))
			{
				/* OUTPUT TO TEMPFILE ...********************************/

				if (isrealcovermap(covermap, type) == 0)
					sum2 = 1.0;
				sum1 = sum1 / kfactor;
				if(log_rat)
				{
					sprintf (temp_str1, "switching on covermap type %c\n", type[covermap]);
					log_output(logfile, temp_str1);
				}
				switch(type[covermap])
				{
					case 'M':
						sprintf (buf, "%.10lf", max);
						break;
					case 'X':
						sprintf (buf, "%ld", sum3);
						break;
					case 'S':
						sprintf (buf, "%.10lf", (sum1 / sum2) * (sum2 / G_area_of_cell_at_row(0)));
						break;  
					case 'T':
						sprintf (buf, "0");
						break;
					default:
						sprintf (buf, "%.10lf", sum1 / sum2);
				}
				G_trim_decimal (buf);  
				if (covermap == firstcovermap)
				{
					for(basemap = 0; basemap < numcols; basemap++)
						if (isbasemap(basemap,type) == 1)
						{
							fprintf(fd2, "%ld ", oldbasecat[basemap]);
							if(log_rat)
							{
								sprintf (temp_str1, "outputing for first cover map %ld\n", oldbasecat[basemap]);
								log_output(logfile, temp_str1);
							}
						}
				}  
				fprintf (fd2, "%s\n", buf);
				if(log_rat)
				{
					sprintf (temp_str1, "outputing buffer %s\n", buf);
					log_output(logfile, temp_str1);
				}


				/********************************************************/

				numrows++;
				if (hangin_in_there == 0)
					break;
				sum1 = 0.0;
				sum2 = 0.0;
				sum3 = 0;
				amax = 0.0;
				for(basemap = 0;basemap < numcols; basemap++)
				{
					if (isbasemap(basemap, type) == 0)
						continue;
					oldbasecat[basemap] = basecat[basemap];
				}
			}
			if (type[covermap] == 'L')
				sscanf (G_get_cat((CELL)covercat, &cats), "%lf", &x);
			else
				x = (double)covercat;
			sum1 += x * area;
			sum2 += area;
			sum3 += 1;
			if (amax < area) {
				max = x;
				amax = area;
				}
		}
		fclose (fd1);
		fclose (fd2);
		unlink (tempfile1);
	}

	/* NOW OUTPUT THE TABLE USING THE RAW DATA FILES CONSTRUCTED ABOVE **************/

	if ((fd3 = fopen (fileopt->answers[0], "w")) == NULL)
	{
		if(log_rat)
		{
			sprintf(temp_str1, "\n Cannot open output file %s", fileopt->answers[0]);
			log_output(logfile, temp_str1);
		}
		exit(1);
	}
	else
	{
		if(log_rat)
		{
			sprintf(temp_str1, "opened output file %s\n", fileopt->answers[0]);
			log_output(logfile, temp_str1);
		}
	}
	/*
	if ((fd3 = fopen (tempfile3, "w")) == NULL) {
		fprintf(stderr,"\n Cannot open file %s", fileopt->answers[0]);
		exit(1);
	}
  
	fprintf(stderr,"Output File is %s\n", fileopt->answers[0]);
	ch=NULL;
	ch=getchar();
	*/
	for (covermap = 0; covermap < numcols; covermap++)
	{
		if((iscovermap(covermap, type) == 0) && (isothertype(covermap, type) == 0))
			continue;

		if ((the_file[covermap] = fopen(tempfile2[covermap], "r")) == NULL)
		{
			if(log_rat)
			{
				sprintf(temp_str1, "\n Cannot open input file %s", tempfile2[covermap]);
				log_output(logfile, temp_str1);
			}
			exit(1);
		}
		else
		{
			if(log_rat)
			{
				sprintf(temp_str1, "opened input file %s\n", tempfile2[covermap]);
				log_output(logfile, temp_str1);
			}
		}
	}

	finalnumrows=0;
	for (i = 0; i < numrows; i++)
	{
		gotazero=0;
		for(basemap = 0; basemap < numcols; basemap++)
		{
			if (isbasemap(basemap, type) == 0)
				continue;

			fscanf(the_file[firstcovermap], "%ld", &basecat[basemap]);
			if(log_rat)
			{
				sprintf(temp_str1, "read basecat[%d] = %ld\n", basemap, basecat[basemap]);
				log_output(logfile, temp_str1);
			}

			if(basecat[basemap]==0)
				gotazero=1;
		}

		for (covermap = 0; covermap < numcols; covermap++)
		{
			if((iscovermap(covermap, type) == 0) && (isothertype(covermap, type) == 0))
				continue;

			fscanf(the_file[covermap], "%lf", &covervalue[covermap]);
			if(log_rat)
			{
				sprintf(temp_str1, "read covervalue[%d] = %lf\n", covermap, covervalue[covermap]);
				log_output(logfile, temp_str1);
			}
		}
		if ((zeroflag->answer) && (gotazero == 1))
			continue;
		finalnumrows++;
		j = 0;
		/*		for(j = 0; j < numcols; j++) */
		while(j < numcols)
		{
			switch(type[j])
			{
				case 'A':
				case 'M':
				case 'C':
				case 'V':
				case 'S':
				case 'L':
					if(formattedoutput == 1)
					{
						fprintf(fd3, "%12.2lf", covervalue[j]);        
						if(log_rat)
						{
							sprintf(temp_str1, "writing covervalue[%d] = %12.2lf\n", j, covervalue[j]);
							log_output(logfile, temp_str1);
						}
					}
					else
					{
						fprintf(fd3, " %lf", covervalue[j]);
						if(log_rat)
						{
							sprintf(temp_str1, "writing covervalue[%d] = %lf\n", j, covervalue[j]);
							log_output(logfile, temp_str1);
						}
					}
					break;
				case 'B':
					if(formattedoutput == 1)
					{
						fprintf(fd3, "%6ld", basecat[j]);        
						if(log_rat)
						{
							sprintf(temp_str1, "writing basecat[%d] = %6ld\n", j, basecat[j]);
							log_output(logfile, temp_str1);
						}
					}
					else
					{
						fprintf(fd3, " %ld", basecat[j]);
						if(log_rat)
						{
							sprintf(temp_str1, "writing basecat[%d] = %ld\n", j, basecat[j]);
							log_output(logfile, temp_str1);
						}
					}
					break;
				case 'X':
					if(formattedoutput == 1)
					{
						fprintf(fd3, "%6ld", (int)(covervalue[j]));
						if(log_rat)
						{
							sprintf(temp_str1, "writing covervalue[%d] = %6ld\n", j, (int)covervalue[j]);
							log_output(logfile, temp_str1);
						}
					}
					else
					{
						fprintf(fd3, " %ld", (int)(covervalue[j]));
						if(log_rat)
						{
							sprintf(temp_str1, "writing covervalue[%d] = %ld\n", j, (int)covervalue[j]);
							log_output(logfile, temp_str1);
						}
					}
					break;
				case 'T':
					fprintf(fd3, "  %s", table[j]);
					if(log_rat)
					{
						sprintf(temp_str1, "writing table[%d] = %s\n", j, table[j]);
						log_output(logfile, temp_str1);
					}
					break;
			}
			j++;
		}
		if(j == numcols)
		{
			if(log_rat)
			{
				sprintf(temp_str1, "skipping newline\n");
				log_output(logfile, temp_str1);
			}
		}
		else
		{
			fprintf(fd3, "\n");
			if(log_rat)
			{
				sprintf(temp_str1, "writing newline\n");
				log_output(logfile, temp_str1);
			}
		}

	}

	for (covermap = 0; covermap < numcols; covermap++)
	{
		if(iscovermap(covermap, type) == 0)
			continue;
		fclose(the_file[covermap]);
		unlink(tempfile2[covermap]);
	}
	fclose(fd3);

	//if (nflag->answer)
	//printf("%ld\n",finalnumrows);  
	fflush(stdout);
	sprintf(command, "cat %s", fileopt->answers[0]);
	if(log_rat)
	{
		sprintf(temp_str1, "executing command - %s\n", command);
		log_output(logfile, temp_str1);
	}
	/*system(command); AD COMMENTED THIS OUT TO GET RID OF SCREEN NUMBER DUMP*/
	
	/*  unlink(tempfile3); */

	return(0);
}

int iscovermap(map, type)
    int map;
    char type[MAXCOLS];
{
    if ((type[map]=='A')||(type[map]=='C')||(isrealcovermap(map,type)==1)) return(1);
    return(0);
}

int isrealcovermap(map, type)
    int map;
    char type[MAXCOLS];
{
    if ((type[map]=='V')||(type[map]=='S')||(type[map]=='L')||(type[map]=='X')||(type[map]=='M')) return(1);
    return(0);
}

int isbasemap(map, type)
    int map;
    char type[MAXCOLS];
{
    if ((type[map]=='b')||(type[map]=='B')) return(1);
    return(0);
}

int isothertype(map, type)
    int map;
    char type[MAXCOLS];
{
    if (type[map]=='T') return(1);
    return(0);
}

