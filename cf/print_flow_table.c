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

void	print_flow_table(num_patches, flow_table, sc_flag, slp_flag, cell,  scale_trans, input_prefix, output_suffix, width)
	struct flow_struct *flow_table;
	int num_patches;
	int sc_flag;
    int slp_flag;
	double cell;
	double width;
	double scale_trans;
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
	/*strcat(name,"_flow_table.dat");*/
	strcat(name,output_suffix);
	strcpy(name2,input_prefix);
	strcat(name2,".gamma");

	if ((outfile = fopen(name,"w"))==NULL)
		{
		printf("Error opening flow_table output file\n");
		exit(1);
		}

	if ((gammaout = fopen(name2,"w"))==NULL)
		{
		printf("Error opening gamma output file\n");
		exit(1);
		}

	fprintf(outfile,"%8d",num_patches); 

	for (i=1; i<= num_patches; i++) {

	
/* this is a temporary patch, so that streams immediately produces outflow */

		if (((flow_table[i].land == 1) || (flow_table[i].total_gamma < ZERO)) && (sc_flag > 0))  {

				mult = ( float )( flow_table[i].K * flow_table[i].m_par *
							sqrt(flow_table[i].area) ); 	
	
				if (sc_flag == 2)	
					tmp = ( float )( rand()/(pow(2.0,15.0)-1) );	
				else
				  	tmp = (flow_table[i].internal_slope );

				
				if (sc_flag == 3) {
				 	adj_ptr = flow_table[i].adj_list;
					flow_table[i].internal_slope = 0.0;
					cnt = 0;
					for (j=1; j<= flow_table[i].num_adjacent; j++) {
						if (adj_ptr->gamma <= 0)
							flow_table[i].internal_slope += adj_ptr->slope;
						cnt += 1;
						adj_ptr = adj_ptr->next;
					}
					flow_table[i].internal_slope = flow_table[i].internal_slope / cnt;
					tmp =( float )( -1.0*flow_table[i].internal_slope );
					
				}

				if (slp_flag == 1)
					tmp = flow_table[i].internal_slope * sqrt(flow_table[i].area) * cell;
				if (slp_flag == 2)
					tmp = flow_table[i].max_slope * sqrt(flow_table[i].area) * cell;

				flow_table[i].total_gamma = (float)( mult * tmp * scale_trans );
					
		
		}


		fprintf(gammaout,"\n %d:%d:%lf",
				flow_table[i].patchID,  
				flow_table[i].patchID,  
				flow_table[i].total_gamma);

	
		fprintf(outfile,"\n %6d %6d %6d %6.1f %6.1f %6.1f %10f %4d %f %4d",  
						flow_table[i].patchID,  
						flow_table[i].zoneID,  
						flow_table[i].hillID,  
						flow_table[i].x,
						flow_table[i].y,
						flow_table[i].z,
						flow_table[i].acc_area,
						flow_table[i].land,
						flow_table[i].total_gamma,
						flow_table[i].num_adjacent);
	
		adj_ptr = flow_table[i].adj_list;


				
		for (j=1; j<= flow_table[i].num_adjacent; j++) {
				fprintf(outfile, "\n%16d %6d %6d %8.8f  ", 
							adj_ptr->patchID,  
							adj_ptr->zoneID,  
							adj_ptr->hillID,  
							adj_ptr->gamma);
			adj_ptr = adj_ptr->next;
			}
		if (flow_table[i].land == 2) {

			
			fprintf(outfile, "\n%16d %6d %6d %lf",
				flow_table[i].stream_ID.patch, 
				flow_table[i].stream_ID.zone, 
				flow_table[i].stream_ID.hill,
				width);
				}

		}


	fclose(outfile);
	fprintf(gammaout,"\n end");
	fclose(gammaout);

	return;

    }



