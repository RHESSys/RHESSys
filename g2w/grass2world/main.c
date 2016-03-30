/* grass2world


   derived from grass2macaque (C), October 1996, by Fred Watson.
   Modifications from January 1997 by Christina Tague
*/
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <errno.h>

#include "world.h"
#include "macaque.h"
#include "fun.h" 



 int main(int argc, char *argv[])
/* Read a template file which guides us as to what data
   to export from GRASS. File is of the form:

   (example)

   _world worldmap
   p_blah0 blah0map
   _biome biomemap
     _catchment catchmentmap
       _hillslope hillslopemap
         p_blah1 blah1map
         _interval intervalmap
         p_blah2 blah2map
         s_blah3 blah3map
         a_area _area

   ...where the 'p_...' and 's_...' bits are naming parameters
   to be put in the output at a certain point in the
   hierarchy using data from map '.....map'; and the
   '_unitname' bits are naming maps which define the spatial
   arrangement at various levels of the various spatial
   units. Also, map names that start with '_' are treated
   specially: _area spits out the area of the spatial unit
   instead of some average parameter value.

   Confused? I am.
*/
{
  struct tunitstruct *tunit;
  struct tlevelstruct tlevel[NUMLEVELS];
  struct tvarliststruct *thetvarlist;
  struct tvarliststruct *prev;
  struct headerstruct	*header;	
  
  char item[MAXVARNAME];
  char function[MAXFUNCNAME];
  char map[MAXMAPNAME];
  int  f,lev,lev2,ext;
  int  nh_flag,i;
  int world_flag, template_flag;
  int 	label_cos, label_sin;
  float	mult,add,value;
  float value_sin, value_cos;
  char line[MAXTEMPLATELINE];
  FILE *templatefile, *outfile, *tmpfile, *tmpfile2, *hdr_outfile;
  char command[MAXCOMMAND], command2[MAXCOMMAND], filecommand[MAXCOMMAND];
  char command3[MAXCOMMAND];
  char tmpname[MAXFILENAME];
  char tmpname2[MAXFILENAME];
  char template_fname[MAXFILENAME];
  char world_fname[MAXFILENAME];
  char header_fname[MAXFILENAME];
  char **VARNAME;
  char tmpstr[MAXFILENAME];
  char v;
  char c, ch;


	/* initial allocation */
	VARNAME = (char **)calloc(MAXNUMVARS, sizeof(char *));
	for (i=0; i< MAXNUMVARS; i++)
		VARNAME[i] = (char *)malloc(400*sizeof(char));
	/* process command line */

	nh_flag = 0;
	template_flag = 0;
	world_flag = 0;
	i = 0;
	while (i < argc)
        {
                if (strcmp(argv[i],"-nh") == 0 )
                        {
                        nh_flag = 1;
                        }

			if (!strcmp(argv[i],"-t"))
			{
				strncpy(template_fname, argv[i+1], MAXFILENAME);
				template_flag = 1;
			}

			if (!strcmp(argv[i],"-w"))
			{
				strncpy(world_fname, argv[i+1], MAXFILENAME);
				world_flag = 1;
			}
		i++;
	}
 
  /* Start reading the template file to see which maps
     define the spatial structure of which levels, and
     what parameters are defined at what level, and
     which maps define those parameters...
  */
 
	if (template_flag == 0)
		strcpy(template_fname, TEMPLATEFILENAME);
	if (world_flag == 0)
		strcpy(world_fname, OUTFILENAME);

	if((templatefile=fopen(template_fname,"r"))==NULL) {
		printf("Couldn't open template file.\n");
		exit(EXIT_FAILURE);
	}


	header = (struct headerstruct *)malloc(sizeof(struct headerstruct));

/* read the header information */
   if (nh_flag == 0)
  	readheader( templatefile, header);

  for(lev=0;lev<NUMLEVELS;lev++) {

    tlevel[lev].thetvarlist = NULL;
    strcpy(tlevel[lev].map,"");
    tlevel[lev].table = NULL;
	


    /* Try and read which map defines the spatial structure
       of this level...
    */


    if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
      error("Reading template file.");
    sscanf(line,"%s %s %d",item,map,&ext);
    if (*item!='_') 
      error("Expecting \"_\".");
    if(strcmp(&(item[1]),LEVELNAME[lev])!=0)
      error("Expecting _levelname.");
    strcpy(tlevel[lev].map,map);
	tlevel[lev].extent = ext;

    /* Search the template file for parameters which are defined
       at this level until lower level information is found
       (introduced by a '_'). Work out which maps are associated
       with which parameters and store this information in
       a linked list ('thetvarlist')...
    */

    thetvarlist = NULL;
    fscanf(templatefile," ");
    while(ungetc(c=getc(templatefile),templatefile)!='_') {
	/*
      if(fgets(line,MAXTEMPLATELINE,templatefile)==NULL)
        break;
      sscanf(line,"%s %s",item, function);
	*/
		if (feof(templatefile)) break;

	   fscanf(templatefile,"%s %s", item, function) ;

      if((f=funcnumber(function))==ERRORFUNCNUMBER) {
        msgnoloop(function);
        error("Couldn't find a function name that was given in the template file.");
      }
      if((v=varnumber(item, VARNAME))==ERRORVARNUMBER) {
        msgnoloop(item);
        error("Couldn't find a varname that was given in the template file.");
      }
 
      prev=thetvarlist;
      if ( (thetvarlist = (struct tvarliststruct*)malloc(sizeof(struct tvarliststruct)) ) ==
	  		NULL) {
			fprintf(stderr,"ERROR: Could not allocate the varlist \n");
			exit(EXIT_FAILURE);
			}
      thetvarlist->next = NULL;
      thetvarlist->varnum = v;
	  thetvarlist->funcnum = f;

	  for (ext =0; ext < tlevel[lev].extent; ext++) { 
	  /* read in functions parameters */
	  switch(f) {
		case 0:  
			fscanf(templatefile,"%s",map);
      		strcpy(thetvarlist->map[ext],map);
			break;
		case 1:  
			fscanf(templatefile,"%s",map);
      		strcpy(thetvarlist->map[ext],map);
			break;
		case 2:
			break;
		case 3:					
			fscanf(templatefile,"%s",map);
      		strcpy(thetvarlist->map[ext],map);
			break;
		case 4:
			fscanf(templatefile,"%f %f %s",&mult,&add,map);
  			thetvarlist->mult[ext] = mult;
			thetvarlist->add[ext] = add;
			strcpy(thetvarlist->map[ext], map);
			break;
		case 5:
			fscanf(templatefile,"%f %f %s",&mult,&add,map);
  			thetvarlist->mult[ext] = mult;
			thetvarlist->add[ext] = add;
			strcpy(thetvarlist->map[ext], map);
			break;
		case 6:
			fscanf(templatefile,"%f",&value);
			thetvarlist->value[ext] = value;
			break;
		case 7:
			fscanf(templatefile,"%f",&value);
			thetvarlist->value[ext] = value;
			break;
		case 8:
			fscanf(templatefile,"%s",&map);
			strcpy(thetvarlist->map[ext], map);
			fscanf(templatefile,"%s",&map);
			strcpy(thetvarlist->map2[ext], map);
			break;
		case 9:  
			fscanf(templatefile,"%s",map);
      		strcpy(thetvarlist->map[ext],map);
			break;
	  } /* end switch */

	 } /* end for (extent) */

      if(prev!=NULL)
        prev->next = thetvarlist;
      if(tlevel[lev].thetvarlist == NULL)
        tlevel[lev].thetvarlist = thetvarlist;
      fscanf(templatefile," ");
    } /*while*/
  } /*for*/

  fclose(templatefile);

  /* OK, we know the deal now and can move on from reading
     the template file.

     The next step is to create a tree structure storing the labels
     and counts of subtunits of each spatial unit in the world.

     We start this by spitting out a bunch of files from
     rat, one for each level. We then open the files
     and let a recursive routine go through them constructing
     the tree as it goes...
  */

  for(lev=0;lev<NUMLEVELS;lev++) {

	/* form rat command to find output the number of sub-levels for each level */
	/*	ie. the number of patches in each zone 												*/

    sprintf(tmpname,"%s%d",TEMPFILENAME,lev);
    sprintf(command,"rat -z table=");

	/* add all basemaps to r.average.table command */
    if(lev>0) {
      for(lev2=0;lev2<lev;lev2++) {
        sprintf(command2,"b%s,",tlevel[lev2].map);
        strcat(command,command2);
      }
    }

    if(lev<BOTTOMLEVEL)
      sprintf(command2,"B%s,X%s file=%s",
        tlevel[lev].map,
        tlevel[lev+1].map,
        tmpname);
    else
      sprintf(command2,"B%s,T0 file=%s",
        tlevel[lev].map,
        tmpname);
    
	strcat(command,command2);
    printf("\n Executing command %s", command);
    int ret = system(command);
	if( ret != 0 )
	{
		printf("\n Command %s failed", command);
		return(ret);
	}


	/*
	if (( outfile = fopen("tmp.txt","w")) == NULL) 
	error("Opening output file");
	
	fprintf(outfile, "%s \n", command);

	fclose(outfile);
	*/
		
	/* debugging 
	if (lev == NUMLEVELS-1) {
	fprintf(stderr,"\n processing command %s",command);
    } */

/* read in ouput from rat and store the table */
    tmpfile = fopen(tmpname,"r");

	if ( (tlevel[lev].table = (struct tableliststruct *)malloc(
					sizeof(struct tableliststruct))) == NULL ) {
			fprintf(stderr,"ERROR: Could not allocate tlevel.table \n");
			exit(EXIT_FAILURE);
			}

	tlevel[lev].table_ptr = tlevel[lev].table;
	while (!feof(tmpfile)) {
		fscanf(tmpfile,"%d %f",&(tlevel[lev].table_ptr->label), &(tlevel[lev].table_ptr->value));

		if ( (tlevel[lev].table_ptr->next = (struct tableliststruct *)malloc(
								sizeof(struct tableliststruct))) == NULL ) {
			fprintf(stderr,"ERROR: Could not allocate tlevel.table \n");
			exit(EXIT_FAILURE);
			}

		tlevel[lev].table_ptr = tlevel[lev].table_ptr->next;
		}
	tlevel[lev].table_ptr = tlevel[lev].table;
	fclose(tmpfile);
	sprintf(filecommand,"rm %s",tmpname);
	system(filecommand);  
  }

  /* ...all the rat are created and open. Now let's
     initialise the world (world 0) and recursively sift through 
     the files...
  */

  if ( (tunit = (struct tunitstruct*)malloc( sizeof(struct tunitstruct))) == NULL) { 
			fprintf(stderr,"ERROR: Could not allocate tunitstruct \n");
			exit(EXIT_FAILURE);
			}

  tunit->lev = 0;          /* Real value  */
  tunit->numsubtunits = 0; /* Dummy value */ 
  tunit->subtunit = NULL;  /* Dummy value */
  tunit->label = 0;        /* Dummy value */
  tallyunit(tunit, tlevel);


  /* Now we finally get ready to start spitting out the
     hierarchical world data structure.
 
     First up, we use rat again to produce files
     containing average parameter values for each spatial
     unit as dictated by the template file. We then open
     those files ready for a recursive function to start spitting
     shit out...
  */

  for(lev=0;lev<NUMLEVELS;lev++) {

   for (ext=0; ext < tlevel[lev].extent; ext++) {

    thetvarlist = tlevel[lev].thetvarlist;
    while(thetvarlist != NULL) {

	/* initialize rat command for the general case */
      sprintf(tmpname,"%s%d%s",TEMPFILENAME,lev,VARNAME[thetvarlist->varnum]);
      sprintf(tmpname2,"%s%d%s.2",TEMPFILENAME,lev,VARNAME[thetvarlist->varnum]);
      sprintf(command,"rat  -z -k table=");
      if(lev>0) {
        for(lev2=0;lev2<lev;lev2++) {
          	sprintf(command2,"b%s,",tlevel[lev2].map);
          	strcat(command,command2);
        } 
      }

	/* now figure out which option to use */
	/* and construct the appropriate grass command */

	  switch(thetvarlist->funcnum) {
      case 0:  /* parameter average value in spatial unit  */  
      	sprintf(command2,"B%s,V%s file=%s",
          tlevel[lev].map,
          thetvarlist->map[ext],
          tmpname);
		 break;
      case 1:  /* parameter average value in spatial unit  */  
      	sprintf(command2,"B%s,V%s file=%s",
          tlevel[lev].map,
          thetvarlist->map[ext],
          tmpname);
		 break;
	  case 2: /* area of spatial unit */
    	sprintf(command2,"B%s,A file=%s", tlevel[lev].map, tmpname);
	 	break;
	  case 3: /* number in parameter  */
        sprintf(command2,"B%s,X%s file=%s",
          	tlevel[lev].map,
       		thetvarlist->map[ext],
       		tmpname);
		break;
	  case 4: /* equation from base map */
      	sprintf(command2,"B%s,V%s file=%s",
          tlevel[lev].map,
          thetvarlist->map[ext],
          tmpname);
		 break;
	  case 5: /* equation from base map */
      	sprintf(command2,"B%s,V%s file=%s",
          tlevel[lev].map,
          thetvarlist->map[ext],
          tmpname);
		 break;
	  case 6: break;	/* fixed (non-spatial) float  value */	
	  case 7: break;	/* fixed (non-spatial) integer value */	
	  case 8: break;	/* spatial average */
	  case 9: /* modal value in spatial unit */
		  sprintf(command2,"B%s,M%s file=%s",
			tlevel[lev].map,
			thetvarlist->map[ext],
			tmpname);
		
		break;
	  default:
	  	error("Unknown function number ");
	  } /* end switch statement */

      if ((thetvarlist->funcnum < 6) || (thetvarlist->funcnum == 9)) {
		strcat(command,command2);
		/* debugging  */
		printf("\n\nProcessing command %s",command); 
      		system(command);


		/* read in ouput from rat and store the table */
      		if((tmpfile = fopen(tmpname,"r"))==NULL)
       			 error("Opening intermediate file.");
		if ( (thetvarlist->table[ext] = (struct tableliststruct *)malloc(
						sizeof(struct tableliststruct)) ) == NULL) {
			fprintf(stderr,"ERROR: Could not allocate tableliststruct \n");
			exit(EXIT_FAILURE);
			}

		thetvarlist->table_ptr[ext] = thetvarlist->table[ext];
		thetvarlist->table_ptr[ext]->value = 2;
		while (!feof(tmpfile)) {
			fscanf(tmpfile,"%d %s",&(thetvarlist->table_ptr[ext]->label), &tmpstr);
			if (strcmp(tmpstr,"*") == 0)
					thetvarlist->table_ptr[ext]->value = 0;
			else
					 thetvarlist->table_ptr[ext]->value = atof(tmpstr);
			if ( (thetvarlist->table_ptr[ext]->next = (struct tableliststruct *)malloc(
								sizeof(struct tableliststruct)) ) == NULL) {
				fprintf(stderr,"ERROR: Could not allocate tableliststruct \n");
				exit(EXIT_FAILURE);
				}
			thetvarlist->table_ptr[ext] = thetvarlist->table_ptr[ext]->next;
				thetvarlist->table_ptr[ext]->value = 2;
			}
		thetvarlist->table_ptr[ext] = thetvarlist->table[ext];
		fclose(tmpfile);
		sprintf(filecommand,"rm %s",tmpname);
		system(filecommand); 
	}

	/* spherical average processing */
	if (thetvarlist->funcnum == 8) {

		printf("\n spherical average processing ");
		/* calculate average sin and average cos of map */
		/* system("gremove sin"); */
		system("g.remove cos"); 
		system("g.remove sin"); 
		sprintf(command3,"r.mapcalc \"cos=100*cos(%s)*sin(%s)\"",
			thetvarlist->map[ext], thetvarlist->map2[ext]);
      		system(command3);
	
		sprintf(command3,"r.mapcalc \"sin=100*sin(%s)*sin(%s)\"",
			thetvarlist->map[ext], thetvarlist->map2[ext]);
      		system(command3);

      	sprintf(command2,"B%s,Vsin file=%s",
			tlevel[lev].map,
			tmpname);
		sprintf(command3,"%s%s",command,command2);
      	system(command3);

      	sprintf(command2,"B%s,Vcos file=%s",
			tlevel[lev].map,
			tmpname2);
		sprintf(command3,"%s%s",command,command2);
      		system(command3);

      		if((tmpfile = fopen(tmpname,"r"))==NULL)
       			 error("Opening intermediate file.");
      		if((tmpfile2 = fopen(tmpname2,"r"))==NULL)
       			 error("Opening intermediate file.");
		if ( (thetvarlist->table[ext] = (struct tableliststruct *)malloc(
						sizeof(struct tableliststruct)) ) == NULL) {
			fprintf(stderr,"ERROR: Could not allocate tableliststruct \n");
			exit(EXIT_FAILURE);
			}

		thetvarlist->table_ptr[ext] = thetvarlist->table[ext];

		/* read temporary file and add value to variable list */
		while (!feof(tmpfile)) {
			fscanf(tmpfile,"%d %f",&label_sin, &value_sin);	
			fscanf(tmpfile2,"%d %f",&label_cos, &value_cos);	
			if (label_cos != label_sin) {
				fprintf(stderr,"ERROR: sin/cos file labels differ \n");
				exit(EXIT_FAILURE);
				}
			thetvarlist->table_ptr[ext]->label = label_cos;
			value = (float)atan(value_sin/value_cos) * RtoD;
	
			if (value_cos < 0.0)
				thetvarlist->table_ptr[ext]->value =  value + 180 ;
			else if (value_sin < 0.0)
				 thetvarlist->table_ptr[ext]->value = value + 360;
			     else
				 thetvarlist->table_ptr[ext]->value = value;

			thetvarlist->table_ptr[ext]->value = 360 - 
						thetvarlist->table_ptr[ext]->value;

			if (thetvarlist->table_ptr[ext]->value <= 270)
				thetvarlist->table_ptr[ext]->value += 90;
			else 
				thetvarlist->table_ptr[ext]->value -= 270;
				
				
		
					
			if ( (thetvarlist->table_ptr[ext]->next = (struct tableliststruct *)malloc(
								sizeof(struct tableliststruct)) ) == NULL) {
				fprintf(stderr,"ERROR: Could not allocate tableliststruct \n");
				exit(EXIT_FAILURE);
				}
			thetvarlist->table_ptr[ext] = thetvarlist->table_ptr[ext]->next;
			}
		thetvarlist->table_ptr[ext] = thetvarlist->table[ext];
		fclose(tmpfile);
		fclose(tmpfile2);
		sprintf(filecommand,"rm %s",tmpname);
		system(filecommand); 
		sprintf(filecommand,"rm %s",tmpname2);
		system(filecommand); 
		}


      thetvarlist = thetvarlist->next;
    } /*while*/
   } /* end for (extent) */
  } /* end for (levels) */

  /* OK, the average parameter files are all created and openned.
     Lets open the output file and call the recursive function to
     spit everything out...
  */

	printf("\n Opening worldfile %s", world_fname);
 	 if (( outfile = fopen(world_fname,"w")) == NULL) {
		printf("Couldn't open output worldfile %s", world_fname);
		exit(EXIT_FAILURE);
  	}

	/* output header information */
	if ( nh_flag == 0 ) {
		if ( snprintf(header_fname, MAXFILENAME, "%s.hdr", world_fname) >= MAXFILENAME ) {
			fprintf(stderr,
					"Couldn't write world file header as filename would have been longer than the limit of %d\n", MAXFILENAME);
			exit(EXIT_FAILURE);
		}
		hdr_outfile = fopen(header_fname, "w");
		if ( hdr_outfile == NULL ) {
			fprintf(stderr,
					"Error writing header file: %s", strerror(errno) );
			exit(EXIT_FAILURE);
		}
		outputheader(hdr_outfile,header);
	}

  exportunit(tunit,outfile,tlevel,VARNAME);
  fclose(outfile);

  return(EXIT_SUCCESS);
}

void tallyunit(tunit, tlevel)
	struct	tunitstruct *tunit;
	struct	tlevelstruct	*tlevel;
/* Count subtunits and creat a subtree for them using output
   from the (already open) file produced by rat
   that might look something like this...

   ...
   612 5
   613 7
   614 5
   ...

   ...where the 1st column is the label of the current spatial unit
   and the 2nd is the number of subtunits in that unit.
*/
{
  int u;

  /* I start knowing my level and thats it.

     First up, I should find out my label and how many
     subtunits I have...
  */

  tunit->label = tlevel[tunit->lev].table_ptr->label;
  tunit->numsubtunits = tlevel[tunit->lev].table_ptr->value;
  tlevel[tunit->lev].table_ptr =  tlevel[tunit->lev].table_ptr->next;


  if(tunit->numsubtunits>0) {

    /* Now allocate space for my subtunits...
    */

    if (( tunit->subtunit = (struct tunitstruct*)malloc(
      		tunit->numsubtunits * sizeof(struct tunitstruct))) == NULL) {
				fprintf(stderr,"ERROR: Could not allocate tunit->subunit \n");
				exit(EXIT_FAILURE);
				}

    /* ...and loop through them, telling them their level and recurring...
    */

    for(u=0;u<tunit->numsubtunits;u++) {
      tunit->subtunit[u].lev = tunit->lev+1;      /* Real value */
      tunit->subtunit[u].subtunit = NULL;  /* Just in case of errors later */
      tunit->subtunit[u].numsubtunits = 0; /* Real value in case this is the bottom level */
      tunit->subtunit[u].label = 0;        /* Dummy value */
      tallyunit(&(tunit->subtunit[u]),tlevel);
    }
  }
}

void exportunit(tunit, outfile, tlevel,VARNAME)
	struct	tunitstruct	*tunit;
	FILE	*outfile;
	struct	tlevelstruct	tlevel[NUMLEVELS];
	char	**VARNAME;

/* Export a unit using parameter information from
   the (already open) parameter files which might
   look something like this...
   (produced by rat)...

   (example)

   ...
   612 4.3
   613 4.4
   614 4.7
   ...

   ...where the 1st col. is the label of the current spatial unit
   and the 2ns is the parameter value to be spat out.
*/
{
  int u, ext;
  char lev2;
  double val;
  long label;
  struct tvarliststruct *thetvarlist;
  static char buf[100];

  /* OK, we've come into life as a spatial unit. We can start
     by spitting out our own parameter values (if any) and then
     move onto our children...
  */


  for (ext =0; ext<tlevel[tunit->lev].extent; ext++) {


/* output id */
   if(tunit->lev>0) {
       for(lev2=0;lev2<tunit->lev;lev2++)
       fprintf(outfile,"    ");
      }
   fprintf(outfile,"%-15d %s_ID \n",
        (tunit->label * (ext+1)),
        LEVELNAME[tunit->lev]);

   if ((tunit->label * (ext+1)) == 0) {
	printf("\n\n ERROR spatial unit ID within %s has a value of 0; this is should be fixed as it may invalidate worldfile results", 
		LEVELNAME[tunit->lev]);
		exit(EXIT_FAILURE);
		}

	
  thetvarlist = tlevel[tunit->lev].thetvarlist;
  while(thetvarlist!=NULL) {

	switch(thetvarlist->funcnum) {

	case 0:;
	case 1:;
	case 2:;
	case 3:;
	case 4:;
	case 5: {
		label = thetvarlist->table_ptr[ext]->label;
		val = thetvarlist->table_ptr[ext]->value;
		thetvarlist->table_ptr[ext] = thetvarlist->table_ptr[ext]->next;
		if ( ( thetvarlist->funcnum == 4) || (thetvarlist->funcnum == 5) )  
				val = val*thetvarlist->mult[ext]+ thetvarlist->add[ext];
		break;
		}

	case 6: {
		val = thetvarlist->value[ext];
		break;
		}
	case 7: { 
		val = thetvarlist->value[ext];
		break;
		}
	case 8:;
	case 9: { 
		label = thetvarlist->table_ptr[ext]->label;
		val = thetvarlist->table_ptr[ext]->value;
		thetvarlist->table_ptr[ext] = thetvarlist->table_ptr[ext]->next;
		break;
		}
	} /* end switch */

    if(tunit->lev>0) {
      for(lev2=0;lev2<tunit->lev;lev2++)
        fprintf(outfile,"    ");
    }

	/* distinguish between integer and real values in output format */

	if ( (  thetvarlist->funcnum == 1 ) || (thetvarlist->funcnum == 3) ||
		(  thetvarlist->funcnum == 5 ) || (thetvarlist->funcnum == 7) ||
			(thetvarlist->funcnum == 9) ) 
    	fprintf(outfile,"%-15.0f %s\n",
	  	val,
      	VARNAME[thetvarlist->varnum]);
	else 
    	fprintf(outfile,"%-15lf %s\n",
	  	val,
      	VARNAME[thetvarlist->varnum]);

    thetvarlist = thetvarlist->next;
   } /* end while */
  } /* end for (extent) */

  if(tunit->lev < BOTTOMLEVEL) {

    /* Now we spit out how many children we have...
    */
    if(tunit->lev>0) {
      for(lev2=0;lev2<tunit->lev;lev2++)
        fprintf(outfile,"    ");
    }

    fprintf(outfile,"%-15d num_%ss \n",
      tunit->numsubtunits*tlevel[tunit->lev+1].extent,
      LEVELNAME[tunit->lev+1]);

    /* ...introduce them in turn and recur to get them to spit
       out their own stuff...
    */

    for(u=0;u<tunit->numsubtunits;u++) {
      exportunit(&(tunit->subtunit[u]),outfile,tlevel,VARNAME);
    } /*for*/
  } /*if*/
}
