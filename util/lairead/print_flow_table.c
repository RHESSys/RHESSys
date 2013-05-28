/*--------------------------------------------------------------*/
/*                                                              */
/*		print_flow_table									    */
/*                                                              */
/*  NAME                                                        */
/*		 print_flow_table										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 print_flow_table( 								        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*		- locates a patch based on ID value						*/
/*                                                              */
/*  revision:  6.0  29 April, 2005                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "blender.h"
#define DtoR 0.01745329 

void	print_flow_table(num_patches, flow_table,  input_prefix, output_suffix)
	struct flow_struct *flow_table;
	int num_patches;
	char *input_prefix;
	char *output_suffix;

    {
	int i,j, cnt;
	struct	adj_struct *adj_ptr;
	FILE *outfile, *gammaout, *fopen();
	float mult, tmp;

	char name[256];
	char name2[256];


	strcpy(name,input_prefix);
	/*strcat(name,"_table.dat");*/
	strcat(name,output_suffix);

	printf("\n Trying to openfile %s", name);
	if ((outfile = fopen(name,"w"))==NULL)
		{
		printf("Error opening flow_table output file\n");
		exit(1);
		}


	fprintf(outfile,"%d",num_patches); 

	for (i=1; i<= num_patches; i++) {

		fprintf(outfile,"\n %d %d %d   %d %f",  
						flow_table[i].patchID,  
						flow_table[i].zoneID,  
						flow_table[i].hillID,  
						flow_table[i].vegid,
						flow_table[i].lai);

		if (flow_table[i].veglink != NULL) {
			fprintf(outfile, "\n allometric  %f %f %f %f",
					flow_table[i].veglink[0].sla,
					flow_table[i].veglink[0].lived,
					flow_table[i].veglink[0].sc,
					flow_table[i].veglink[0].lr);

			fprintf(outfile, "\n cn %f %f %f %f \n",
					flow_table[i].veglink[0].cnl,
					flow_table[i].veglink[0].cnlw,
					flow_table[i].veglink[0].cndw,
					flow_table[i].veglink[0].cnr);
		}

		else fprintf(outfile, "\n No veg link for vegid %d \n", flow_table[i].vegid);

		}


	fclose(outfile);

	return;

    }



