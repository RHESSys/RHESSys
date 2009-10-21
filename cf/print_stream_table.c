/*--------------------------------------------------------------*/
/*                                                              */
/*		print_stream_table									    */
/*                                                              */
/*  NAME                                                        */
/*		 print_stream_table										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 print_stream_table( 								        */
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

void	print_stream_table(num_patches, num_stream, flow_table, sc_flag, slp_flag, cell,  scale_trans, input_prefix, output_suffix, width, basinid)
	struct flow_struct *flow_table;
	int num_patches;
	int sc_flag;
    	int slp_flag;
	int  basinid;
	double cell;
	double width;
	double scale_trans;
	char *input_prefix;
	char *output_suffix;

    {
	int i,j, cnt;
	struct	adj_struct *adj_str_ptr;
	FILE *streamout, *fopen();
	float mult, tmp;

	char name[256];
	char name2[256];


	strcpy(name2,input_prefix);
	strcat(name2,".stream_table");

	if ((streamout = fopen(name2,"w"))==NULL)
		{
		printf("Error opening gamma output file\n");
		exit(1);
		}

	fprintf(streamout,"%d",num_stream); 
	fprintf(streamout,"%d",basinid); 

	for (i=1; i<= num_patches; i++) {

	
		if (flow_table[i].land == 1) {

		fprintf(streamout,"\n %d %d %d %f %f %f %d %d %d",  
						flow_table[i].patchID,  
						flow_table[i].zoneID,  
						flow_table[i].hillID,  
						flow_table[i].x,
						flow_table[i].y,
						flow_table[i].z,
						flow_table[i].area,
						flow_table[i].land,
						flow_table[i].num_str);
	
		adj_str_ptr = flow_table[i].adj_str_list;

		for (j=1; j<= flow_table[i].num_dsa; j++) {
			if (adj_str_ptr->gamma > 0.0)
				fprintf(streamout, "\n%16d %6d %6d %8.8f  ", 
							adj_str_ptr->patchID,  
							adj_str_ptr->zoneID,  
							adj_str_ptr->hillID,  
							adj_str_ptr->gamma/flow_table[i].total_str_gamma);
			adj_str_ptr = adj_str_ptr->next;
			}

		}
	}


	fclose(streamout);
	fprintf(streamout,"\n end");
	fclose(streamout);

	return;

    }



