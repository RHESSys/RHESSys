/*--------------------------------------------------------------*/
/*                                                              */
/*		print_drain_stats									    */
/*                                                              */
/*  NAME                                                        */
/*		 print_drain_stats										*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 print_drain_stats( 								    */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v 	Verbose Option										*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*		- prints drainage info for each paths					*/
/*                                                              */
/*  revision:  6.0  29 April, 2005                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

 
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
 

void	print_drain_stats(num_patches, flow_table, input_prefix)
	struct flow_struct *flow_table;
	int num_patches;
	char *input_prefix;

    {
	int i;
	FILE *outfile;
	char name[256];
	
	printf("\n Printing Drainage Statistics ");

	strcpy(name,input_prefix);
	strcat(name,"_stats.dat");

	if ((outfile = fopen("name.dat","w"))==NULL)
		{
		printf("Error opening flow_table output file\n");
		exit(1);
		}


	fprintf(outfile,"%8d",num_patches); 
	for (i=1; i<= num_patches; i++) {


		fprintf(outfile,"\n %6d %6d %6d %6.1f %6.1f %6.1f %10f %4d %15.8f %10.6f %9d %4d",  
						flow_table[i].patchID,  
						flow_table[i].zoneID,  
						flow_table[i].hillID,  
						flow_table[i].x,
						flow_table[i].y,
						flow_table[i].z,
						flow_table[i].acc_area,
						flow_table[i].land,
						flow_table[i].total_gamma,
						flow_table[i].slope,
						flow_table[i].area,
						flow_table[i].num_adjacent);


		}



	fclose(outfile);
	return;

    }



