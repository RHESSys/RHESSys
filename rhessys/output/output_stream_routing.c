/*--------------------------------------------------------------*/
/* 																*/
/*					output_stream_routing						*/
/*																*/
/*	output_stream_routing - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_stream_routing - outputs current contents of streamflow routing.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_stream_routing( int routing_flag,										*/	
/*					struct	basin_object	*basin,				*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs streamflow routing results according to commandline			*/
/*	specifications to specific files							*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_stream_routing(			
					 struct	stream_network_object  *stream_network,
					 struct	date	date,
					 FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	/*--------------------------------------------------------------*/
	/*      Output streamflow routing results.								*/
	/*--------------------------------------------------------------*/
	

	fprintf(outfile, "%d %d %d %d %lf %lf %lf %lf %lf\n", 
                date.day,
		date.month,
		date.year,
                stream_network[0].reach_ID,
                stream_network[0].Qout * 86400,
		stream_network[0].previous_lateral_input * stream_network[0].length * 86400,
                stream_network[0].previous_Qin * 86400,
		stream_network[0].water_depth,
		stream_network[0].reservoir.initial_storage/10000);


	return;
} /*end output_stream_routing*/
