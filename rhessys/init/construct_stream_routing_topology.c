/*--------------------------------------------------------------*/
/* 																*/
/*					construct_stream_routing_topology					*/
/*																*/
/*	construct_stream_routing_topology.c - creates a patch object		*/
/*																*/
/*	NAME														*/
/*	construct_stream_routing_topology.c - creates a patch object		*/
/*																*/
/*	SYNOPSIS													*/
/*	struct stream_routing_list_object construct_stream_routing_topology( 		*/
/*							struct basin_object *basin)			*/
/*																*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*  reads stream_routing topology from input file						*/
/*	creates neighbourhood structure for each patch in the basin */
/*	returns a list giving order for patch-level stream_routing			*/
/*																*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"

struct stream_list_object construct_stream_routing_topology(
					  		  char *stream_filename,
							  struct basin_object *basin,
							  struct command_line_object *command_line)
													  
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct patch_object *find_patch(int, int, int, struct basin_object *);
	struct hillslope_object *find_hillslope_in_basin(int, struct basin_object *);
	int assign_neighbours (struct neighbour_object *,
		int,
		struct basin_object *,
		FILE *);
	
	void *alloc(size_t, char *, char *);

	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct patch_object *find_patch(int, int, int, struct basin_object *);
	struct hillslope_object *find_hillslope_in_basin(int, struct basin_object *);
	int assign_neighbours (struct neighbour_object *,
						   int,
						   struct basin_object *,
						   FILE *);
	
	void *alloc(size_t, char *, char *);
	
	
	/*--------------------------------------------------------------*/
	/*      Local variable definition.                                                                      */
	/*--------------------------------------------------------------*/
	int i, j, num_reaches,m,k,num_reservoir,reach_ID,reservoir_ID,flag_min_flow_storage;
	int hillID, patchID, zoneID;
	int neighbour_hill_num,neighbour_hill_count_0,neighbour_hill_count_1;    
	double month_max_storage[12],min_storage, min_outflow;
	FILE *stream_file;	
	FILE *reservoir_file;
	
	struct stream_list_object stream_list;
	struct stream_network_object *stream_network;
    struct stream_network_object *stream_network_ini;
	struct hillslope_object *hillslope;
	
	
	

	
	/*--------------------------------------------------------------*/
	/*  Try to open the stream file in read mode.                    */
	/*--------------------------------------------------------------*/
	if ( (stream_file = fopen(stream_filename,"r")) == NULL ){
		fprintf(stderr,"FATAL ERROR:  Cannot open stream file %s\n",
				stream_filename);
		exit(0);
	} /*end if*/
	
	
	fscanf(stream_file,"%d",&num_reaches);
	stream_list.num_reaches = num_reaches;
	
	/*--------------------------------------------------------------*/
	/* allocate stream network					*/
	/*--------------------------------------------------------------*/
	
	stream_network_ini = (struct stream_network_object *)alloc(
															   num_reaches * sizeof(struct stream_network_object), " streamlist",
															   "construct_stream_routing_topography");
	stream_network = (struct stream_network_object *)alloc(
														   num_reaches * sizeof(struct stream_network_object), " streamlist",
														   "construct_stream_routing_topography");
	
	
	/*--------------------------------------------------------------*/
	/*      Read  each reach record.                                */
	/*--------------------------------------------------------------*/
	for (i=0; i< num_reaches; ++i) {
		neighbour_hill_num=0;
		neighbour_hill_count_0=0;
		neighbour_hill_count_1=0;
		stream_network_ini[i].reservoir_ID=0;
		fscanf(stream_file,"%d %lf %lf %lf %lf %lf %lf %d",
			   &(stream_network_ini[i].reach_ID),
			   &(stream_network_ini[i].top_width),
			   &(stream_network_ini[i].bottom_width),
			   &(stream_network_ini[i].max_height),
			   &(stream_network_ini[i].stream_slope),
			   &(stream_network_ini[i].manning),
			   &(stream_network_ini[i].length),
			   &(stream_network_ini[i].num_lateral_inputs));
		
	
		/* initializations */
		stream_network_ini[i].initial_flow=0.0;
		stream_network_ini[i].previous_lateral_input=0.0;
		stream_network_ini[i].previous_Qin=0.0;
		stream_network_ini[i].Qin=0.0;
		stream_network_ini[i].Qout=0.0;
	
		/*find neighbouring hillslopes by reach_ID, one hill ID is reach_ID-1, another is reach_ID*/
		
		
		if(stream_network_ini[i].num_lateral_inputs != 0){
			stream_network_ini[i].lateral_inputs = (struct patch_object **)alloc(
				stream_network_ini[i].num_lateral_inputs * sizeof(struct patch_object *), "patch list",
			    "construct_stream_routing_topography");
			
			
			for (j=0; j< stream_network_ini[i].num_lateral_inputs; ++j) {
				fscanf(stream_file,"%d %d %d", &patchID, &zoneID, &hillID);
				stream_network_ini[i].lateral_inputs[j] = find_patch(patchID, zoneID, hillID, basin);
				if(hillID==stream_network_ini[i].reach_ID-1) neighbour_hill_count_0=1;
				if(hillID==stream_network_ini[i].reach_ID) neighbour_hill_count_1=1;
				
			}
		}
		neighbour_hill_num=neighbour_hill_count_0+neighbour_hill_count_1;
		
		stream_network_ini[i].neighbour_hill = (struct hillslope_object **)alloc(
																			 neighbour_hill_num * sizeof(struct hillslope_object *), "hillslope list",
																			 "construct_stream_routing_topography");
		if(neighbour_hill_num==2){
			stream_network_ini[i].neighbour_hill[0] = find_hillslope_in_basin(stream_network_ini[i].reach_ID-1,basin);
			stream_network_ini[i].neighbour_hill[1] = find_hillslope_in_basin(stream_network_ini[i].reach_ID,basin);
		}
		else if(neighbour_hill_count_0==1)stream_network_ini[i].neighbour_hill[0] = find_hillslope_in_basin(stream_network_ini[i].reach_ID-1,basin);
		else stream_network_ini[i].neighbour_hill[0] = find_hillslope_in_basin(stream_network_ini[i].reach_ID,basin);
        hillslope=stream_network_ini[i].neighbour_hill[0];
        stream_network_ini[i].num_neighbour_hills=neighbour_hill_num;
		
		fscanf(stream_file,"%d",&(stream_network_ini[i].num_upstream_neighbours));
		
		if(stream_network_ini[i].num_upstream_neighbours != 0){
			stream_network_ini[i].upstream_neighbours = (int *)alloc(
																	 stream_network_ini[i].num_upstream_neighbours * sizeof(int *),"upstream_neighbours","construct_stream_routing_topography");
			
			for (j=0; j< stream_network_ini[i].num_upstream_neighbours; ++j) {
				fscanf(stream_file,"%d",&(stream_network_ini[i].upstream_neighbours[j]));
								
			}
		}
		fscanf(stream_file,"%d",&(stream_network_ini[i].num_downstream_neighbours));
			
		if(stream_network_ini[i].num_downstream_neighbours != 0){
			stream_network_ini[i].downstream_neighbours = (int *)alloc(
																	   stream_network_ini[i].num_downstream_neighbours * sizeof(int *),"upstream_neighbours","construct_stream_routing_topography");
			
			
			for (j=0; j< stream_network_ini[i].num_downstream_neighbours; ++j) {
				fscanf(stream_file,"%d",&(stream_network_ini[i].downstream_neighbours[j]));
							
			}
		}
	}
	
	/*--------------------------------------------------------------*/
	/*   code to search the outlet reach*/
	/*--------------------------------------------------------------*/
	for (i=0; i< num_reaches; ++i) {
		if(stream_network_ini[i].num_downstream_neighbours==0){
			stream_network[num_reaches-1]=stream_network_ini[i];

			break;
		}
	}
	
	/*--------------------------------------------------------------*/
	/*   code to sort the stream_network from upstream to downstream */
	/*--------------------------------------------------------------*/
	
	m=num_reaches-2;
	for(i=num_reaches-1;i>=0;--i){
		for (j=0; j< stream_network[i].num_upstream_neighbours; ++j) {
			for(k=0; k< num_reaches; ++k){
				
				if(stream_network[i].upstream_neighbours[j]==stream_network_ini[k].reach_ID){
					stream_network[m]=stream_network_ini[k];
					m=m-1;
					break;
				}
			}
		} 
	}
		
        /*--------------------------------------------------------------*/
        /*   code to construct reservoir                                */
        /*--------------------------------------------------------------*/
        if ( command_line[0].reservoir_operation_flag == 1 ){
          if ( (reservoir_file = fopen(command_line[0].reservoir_operation_filename,"r")) == NULL ){
		        fprintf(stderr,"FATAL ERROR:  Cannot open reservoir file %s\n",
				command_line[0].reservoir_operation_filename);
		   exit(0);
	      } /*end if*/
	      fscanf(reservoir_file,"%d",&num_reservoir);
			
	  for(i=0;i<num_reservoir;i++){
	      	fscanf(reservoir_file,"%d %d %d %lf %lf",
			&reach_ID,&reservoir_ID,&flag_min_flow_storage, &min_storage, &min_outflow);
	 	for(j=0;j<12;j++){
			fscanf(reservoir_file,"%lf",&month_max_storage[j]);
							  }
	      	for(k=0;k<num_reaches;k++)
		   if(stream_network[k].reach_ID==reach_ID){
			stream_network[k].reservoir_ID=reservoir_ID;
                  	stream_network[k].reservoir.reservoir_ID=reservoir_ID;
                  	stream_network[k].reservoir.flag_min_flow_storage=flag_min_flow_storage;
                  	stream_network[k].reservoir.min_storage=min_storage*10000.0;
                  	stream_network[k].reservoir.min_outflow=min_outflow;
		  	stream_network[k].reservoir.initial_storage=min_storage*10000.0;
		 printf("%d %d %lf %lf\n",
			stream_network[k].reach_ID,stream_network[k].reservoir.reservoir_ID,
			stream_network[k].reservoir.min_storage,stream_network[k].reservoir.min_outflow);
		  	for(j=0;j<12;j++){
		      		stream_network[k].reservoir.month_max_storage[j]=month_max_storage[j]*10000.0;
		  	} /* end monthly storage assignment */
		}/*end reach search*/
	} /* end reservoir */
	} /* end reservoir flag */

        /*--------------------------------------------------------------*/
        /*   code to construct stream_list                              */
        /*--------------------------------------------------------------*/
		
        stream_list.stream_network = stream_network;
        return(stream_list);
		
	} /*end construct_stream_routing_topology.c*/	
	
	


