/* derived from grass2world */
/* ************************************************************************************* */
/*--------------------------------------------------------------*/
/*                                                              */
/*		change_world										*/
/*                                                              */
/*  NAME                                                        */
/*		 change_world										*/
/*			FILE *,					*/
/*			FILE *,					*/
/*			flow_struct *				*/
/*			int					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 change_world( 								        */
/*                                                              */
/*  ARGS							*/
/*                                                              */
/*                                                              */
/*  DESCRIPTION                                                 */
/*	reads, alters stratum and writes out 			*/
/* 	worldfile  according to flowtable specs		)	*/
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blender.h"

/* function declarations */
	struct tlevelstruct *find_tlevel(struct tlevelstruct *, int *, int);
	struct tlevelstruct *readnextlevel(int, FILE *);
	void writenextlevel(struct tlevelstruct *, int, FILE *);

change_world(oldworld, redefine, flow_table, num_patches) 

	FILE *oldworld, *redefine;
	struct	flow_struct *flow_table;
	int 	num_patches;


/* Read a initialworld file which guides us as to what data
   to export 
*/
{
  struct tlevelstruct *tlevel;
 struct headerstruct *header;
  int level, ID[5], i, j, n, itmp;
  char ch;
  char name[MAXNAME], line[MAXTEMPLATELINE];


	header = (struct headerstruct *)malloc(sizeof(struct headerstruct));

	/* ******************************************************* */
	/* read and write the header information */
	/* ******************************************************* */
    readheader( oldworld, header);
    /* outputheader(redefine, header); */
	/* ******************************************************* */
	/*  read in worldfile into data structure				  */
	/* ******************************************************* */
    level=0;

    if(fgets(line,MAXTEMPLATELINE,oldworld)==NULL) error("Reading initialworld file.");
    sscanf(line,"%d %s", &itmp,name);

    if ( (tlevel = (struct tlevelstruct*)malloc(sizeof(struct tlevelstruct)) ) == NULL) {
			fprintf(stderr,"ERROR: Could not allocate level structure for level %d id %d \n",
			level, itmp );
			exit(1);
			}
    if(fgets(line,MAXTEMPLATELINE,oldworld)==NULL) error("Reading initialworld file.");
    sscanf(line,"%d %s", &tlevel[0].nchildren,name);
    tlevel[0].ID = itmp;
    tlevel[0].namelist = NULL;
    tlevel[0].valuelist = NULL;
    tlevel[0].level = level;
    if ( (tlevel[0].children = (struct tlevelstruct **)calloc(tlevel[0].nchildren , sizeof(struct tlevelstruct *)) ) == NULL) {
			fprintf(stderr,"ERROR: Could not allocate level structure for level %d id %d \n",
			level, itmp );
			exit(1);
			}

   	for(n=0; n< tlevel[0].nchildren; n++) {

		tlevel[0].children[n] = readnextlevel(level+1, oldworld);
	}

	printf("\n finished reading worldfile");

	/* ******************************************************* */
	/* ******************************************************* */

	for (i=0; i<num_patches; i++) {

		ID[1] = 1;
		ID[2] = flow_table[i].hillID;
		ID[3] = flow_table[i].zoneID;
		ID[4] = flow_table[i].patchID;
		ID[5] = flow_table[i].patchID;

		printf("\n processing patchID %d", flow_table[i].patchID);
		flow_table[i].worldlink = find_tlevel(tlevel[0].children[0], ID, 1);

		if ((flow_table[i].worldlink != NULL)  && (flow_table[i].veglink != NULL) ) {
			if (flow_table[i].veglink[0].sla > 0.0001) {


			if (flow_table[i].lai < 1.0) {
				flow_table[i].worldlink[0].valuelist[7] = 1.0/flow_table[i].veglink[0].sla;
				}

			else
				flow_table[i].worldlink[0].valuelist[7] = flow_table[i].lai/flow_table[i].veglink[0].sla;
					
					
			flow_table[i].worldlink[0].valuelist[4] = 0.0;
			flow_table[i].worldlink[0].valuelist[5] = 0.0;
			flow_table[i].worldlink[0].valuelist[6] = 0.0;
			flow_table[i].worldlink[0].valuelist[8] = 0.0;
			flow_table[i].worldlink[0].valuelist[9] = 0.0;
			flow_table[i].worldlink[0].valuelist[10] = 0.0;

			if (flow_table[i].veglink[0].ls > ZERO) {

				if (flow_table[i].lai > 1.0) 
					flow_table[i].worldlink[0].valuelist[11] =  pow(flow_table[i].worldlink[0].valuelist[7],4/3)*
								flow_table[i].veglink[0].ls *
								(flow_table[i].veglink[0].lived/(1+flow_table[i].veglink[0].lived));
				else
					flow_table[i].worldlink[0].valuelist[11] = flow_table[i].worldlink[0].valuelist[7];
			}

			else
			flow_table[i].worldlink[0].valuelist[11] = 0.0;
			
			flow_table[i].worldlink[0].valuelist[12] = 0.0;
			flow_table[i].worldlink[0].valuelist[13] = 0.0;

			if (flow_table[i].veglink[0].ls > ZERO) 
			flow_table[i].worldlink[0].valuelist[14] =  flow_table[i].worldlink[0].valuelist[11]/flow_table[i].veglink[0].lived;
			else
			flow_table[i].worldlink[0].valuelist[14] = 0.0;

			flow_table[i].worldlink[0].valuelist[15] = 0.0;
			flow_table[i].worldlink[0].valuelist[16] = 0.0;
			if (flow_table[i].veglink[0].ls > ZERO) 
			flow_table[i].worldlink[0].valuelist[17] =  flow_table[i].worldlink[0].valuelist[11]/
								flow_table[i].veglink[0].sc *
								(flow_table[i].veglink[0].lived/(1+flow_table[i].veglink[0].lived));
			else
			flow_table[i].worldlink[0].valuelist[17] = 0.0;

			flow_table[i].worldlink[0].valuelist[18] = 0.0;
			flow_table[i].worldlink[0].valuelist[19] = 0.0;
			if (flow_table[i].veglink[0].ls > ZERO) 
			flow_table[i].worldlink[0].valuelist[20] =  flow_table[i].worldlink[0].valuelist[17]/flow_table[i].veglink[0].lived;
			else
			flow_table[i].worldlink[0].valuelist[20] = 0.0;

			flow_table[i].worldlink[0].valuelist[21] = 0.0;
			flow_table[i].worldlink[0].valuelist[22] = 0.0;
			flow_table[i].worldlink[0].valuelist[23] =  flow_table[i].worldlink[0].valuelist[7]/flow_table[i].veglink[0].lr;
			flow_table[i].worldlink[0].valuelist[24] = 0.0;
			flow_table[i].worldlink[0].valuelist[25] = 0.0;
			
			flow_table[i].worldlink[0].valuelist[27] = 0.05* flow_table[i].worldlink[0].valuelist[7];


			flow_table[i].worldlink[0].valuelist[28] = 0.0;
			flow_table[i].worldlink[0].valuelist[29] =  flow_table[i].worldlink[0].valuelist[7]/flow_table[i].veglink[0].cnl;
			flow_table[i].worldlink[0].valuelist[30] = 0.0;
			flow_table[i].worldlink[0].valuelist[31] = 0.0;
			flow_table[i].worldlink[0].valuelist[32] = 0.0;

			if (flow_table[i].veglink[0].ls > ZERO) 
			flow_table[i].worldlink[0].valuelist[33] =  flow_table[i].worldlink[0].valuelist[11]/flow_table[i].veglink[0].cnlw;
			else
			flow_table[i].worldlink[0].valuelist[33] = 0.0;

			flow_table[i].worldlink[0].valuelist[34] = 0.0;
			flow_table[i].worldlink[0].valuelist[35] = 0.0;

			if (flow_table[i].veglink[0].ls > ZERO) 
			flow_table[i].worldlink[0].valuelist[36] =  flow_table[i].worldlink[0].valuelist[14]/flow_table[i].veglink[0].cndw;
			else flow_table[i].worldlink[0].valuelist[36] = 0.0;

			flow_table[i].worldlink[0].valuelist[37] = 0.0;
			flow_table[i].worldlink[0].valuelist[38] = 0.0;

			if (flow_table[i].veglink[0].ls > ZERO) 
			flow_table[i].worldlink[0].valuelist[39] =  flow_table[i].worldlink[0].valuelist[17]/flow_table[i].veglink[0].cnlw;
			else flow_table[i].worldlink[0].valuelist[39] = 0.0;

			flow_table[i].worldlink[0].valuelist[40] = 0.0;
			flow_table[i].worldlink[0].valuelist[41] = 0.0;

			if (flow_table[i].veglink[0].ls > ZERO) 
			flow_table[i].worldlink[0].valuelist[42] =  flow_table[i].worldlink[0].valuelist[20]/flow_table[i].veglink[0].cndw;
			else 	flow_table[i].worldlink[0].valuelist[42] = 0.0;

			flow_table[i].worldlink[0].valuelist[43] = 0.0;
			flow_table[i].worldlink[0].valuelist[44] = 0.0;

			flow_table[i].worldlink[0].valuelist[45] =  flow_table[i].worldlink[0].valuelist[23]/flow_table[i].veglink[0].cnr;
			flow_table[i].worldlink[0].valuelist[46] = 0.0;
			flow_table[i].worldlink[0].valuelist[47] = 0.0;
			flow_table[i].worldlink[0].valuelist[48] = 0.0;
			} 
		else  {
				printf("\n Inappropriate SLA values for stratum ID %d of vegtype %d setting carbon stores to 0",
				flow_table[i].patchID, flow_table[i].vegid);
				for (j=4; j < 49; j++)
					flow_table[i].worldlink[0].valuelist[j] = 0.0;
			}
		}

		else  { 
			if (flow_table[i].worldlink == NULL)
				printf("\n Could not find hill %d  zone %d  patch %d in worldfile", 
				flow_table[i].hillID, flow_table[i].zoneID, flow_table[i].patchID);
			else
				printf("\n For patch %d Could not find vegtype of %d",
				flow_table[i].patchID, flow_table[i].vegid);

			}



	}


	/* ******************************************************* */
	/*  write out the new worldfile				  */
	/* ******************************************************* */

	printf("\n Finished making changes to worldfile  now write");

	level=0;
    	fprintf(redefine,"%d %s\n", tlevel[0].ID, LEVELNAME[level]);
    	fprintf(redefine,"%d num_%s", tlevel[0].nchildren, LEVELNAME[level+1]);
   	for(n=0;n<tlevel[0].nchildren; n++) {
		writenextlevel(tlevel[0].children[n], level+1, redefine);
	}

	fclose(redefine);
	fclose(oldworld);

}

/* **************************** ******************************************************* */
/* 	find t_levelstruct for a particular patch					*/
/* **************************** ******************************************************* */

struct tlevelstruct *find_tlevel(struct tlevelstruct *tlevel, int *ID, int level ) 

{


	int i, n, fndid, vid;
	struct tlevelstruct  *link;

	link = NULL;
	fndid = 0;

	if (tlevel[0].ID == ID[level]) {
		fndid = 1;
		}
			
	if ((tlevel[0].level < BOTTOMLEVEL)  && (fndid == 1)) {	
	n = 0;
	while ((link==NULL) && (n < tlevel[0].nchildren)) { 
			link = find_tlevel(tlevel[0].children[n], ID, level+1);
			n += 1;
			}
	}

	if ((tlevel[0].level == BOTTOMLEVEL)  && (fndid == 1))  link = tlevel;	


	return(link);

}


/* **************************** ******************************************************* */
/*	readnextlevel function definition						*/
/* **************************** ******************************************************* */

struct tlevelstruct  *readnextlevel(level, oldworld) 
		int level;
		FILE *oldworld;

{

		struct tlevelstruct *tlevel;
		int ID, i, n;
		char name[MAXNAME], line[MAXTEMPLATELINE];

    		if(fgets(line,MAXTEMPLATELINE,oldworld)==NULL) error("Reading initialworld file.");
    		sscanf(line,"%d %s", &ID,name);
    
		if ( (tlevel = (struct tlevelstruct*)malloc(sizeof(struct tlevelstruct)) ) == NULL) {
			fprintf(stderr,"ERROR: Could not allocate level structure for level %d id %d \n",
			level, ID );
			exit(1);
			}
    		tlevel[0].ID = ID;
		tlevel[0].level = level;
    		if ( (tlevel[0].namelist = (char **)calloc(NUMVARS[level],sizeof(char *)) ) == NULL) {
			fprintf(stderr,"ERROR: Could not allocate var name structure for level %d id %d \n",
			level, tlevel[0].ID );
			exit(1); 
			}
		for (i=0; i<NUMVARS[level]; i++)
    			if ( (tlevel[0].namelist[i] = (char *)malloc(MAXNAME*sizeof(char)) ) == NULL) {
			fprintf(stderr,"ERROR: Could not allocate var name structure for level %d id %d \n",
			level, tlevel[0].ID );
			exit(1);
			}
			
    		if ( (tlevel[0].valuelist = (float *)calloc(NUMVARS[level],sizeof(float)) ) == NULL) {
			fprintf(stderr,"ERROR: Could not allocate variable  structure for level %d id %d \n",
			level, tlevel[0].ID );
			exit(1);
			}
   	
		for (i=0; i<NUMVARS[level]; i++) {	
			if(fgets(line,MAXTEMPLATELINE,oldworld)==NULL) error("Reading initialworld file.");
    			sscanf(line,"%f %s", &tlevel[0].valuelist[i], tlevel[0].namelist[i]);
				/* replace with -9999 */
			tlevel[0].valuelist[i] = -9999;
		}

		if (level < BOTTOMLEVEL) {
			if(fgets(line,MAXTEMPLATELINE,oldworld)==NULL) error("Reading initialworld file.");
    			sscanf(line,"%d %s", &tlevel[0].nchildren,name);
    			if ( (tlevel[0].children = (struct tlevelstruct **)calloc(tlevel[0].nchildren , sizeof(struct tlevelstruct)) ) == NULL) {
				fprintf(stderr,"ERROR: Could not allocate level structure for level %d id %d \n",
				level, tlevel[0].ID );
				exit(1);
				}
		printf("\n Level %d number children %d", level, tlevel[0].nchildren);
		
	   		for(n=0;n<tlevel[0].nchildren; n++) {
				tlevel[0].children[n] = readnextlevel(level+1, oldworld);
				}
		}

	return(tlevel);

	}

/* **************************** ******************************************************* */
/*	writenextlevel function definition						*/
/* **************************** ******************************************************* */

void writenextlevel(tlevel, level, outfile) 
		struct tlevelstruct *tlevel;
		int level;
		FILE *outfile;

{

		int ID, i, n;
		char name[MAXNAME], line[MAXTEMPLATELINE];

    		fprintf(outfile,"\n%d %s", tlevel[0].ID, LEVELNAME[level]);
   	
		for (i=0; i<NUMVARS[level]; i++) {	
    			fprintf(outfile,"\n%f %s", tlevel[0].valuelist[i], tlevel[0].namelist[i]);
		}

		if (level < BOTTOMLEVEL) {
    		fprintf(outfile,"\n%d num_%s", tlevel[0].nchildren,LEVELNAME[level+1]);
	   	for(n=0;n<tlevel[0].nchildren; n++) {
			writenextlevel(tlevel[0].children[n], level+1, outfile);
			}
		}

	return;

	}
	
