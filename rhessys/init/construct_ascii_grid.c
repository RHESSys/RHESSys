/*--------------------------------------------------------------*/
/* 																*/
/*					construct_ascii_grid	 					*/
/*																*/
/*	construct_ascii_grid.c - makes a base station object 		*/
/*																*/
/*	NAME														*/
/*	construct_ascii_grid.c - makes base station object 		*/
/*																*/
/*	SYNOPSIS													*/
/*	construct_ascii_grid( 									*/
/*							 base_station_file_name,			*/
/*							 start_date,						*/
/*							 duration);							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine makes one base station object.					*/
/*	A base station object holds temporal sequences of driving	*/
/*	variables that different simulation units require.			*/
/*	Base stations are technically located at a point in space	*/
/*	however it is possible for more than on simulation unit		*/
/*	to link to the base station.								*/
/*																*/
/*	The base station object holds header data and (by default)	*/
/*	yearly, monthly, daily, and hourly list of clim objects		*/
/*	which begin empty (NULL pointers).							*/
/*																*/
/*	Each time step has a predetermined set of critical and		*/
/*	non-critical sequences.  The critical sequences are			*/
/*	driving variables that MUST be present for the model to 	*/
/*	execute.  References to these sequences are typically 		*/
/*	hard coded into process algorithms.  Non-critical 			*/
/*	sequences are driving variables which are used if 			*/
/*	provided but are not essential to the simulation.  			*/
/*																*/
/*	File prefixes provided in the base station file identify	*/
/* 	critical sequences.  Hard coded routines will attempt to	*/
/*	open the file given by the file prefix followed by a "."	*/
/*	followed by a suffix identifying the sequence name.  		*/
/*	By convention, the parameter name in the "rhessys.h" file	*/
/*	corresponding to the sequence is used as the suffix.		*/
/*																*/
/*	The number of non-critical sequences for a specific time	*/
/*	step is read in.  Then each of the suffices for the 		*/
/*	non-critical sequences provided in the base station file	*/
/*	are read in one at a time.  The suffix is compared to a		*/
/*	list of valid non-critical sequences for the time step.		*/
/*	If it exists the non-critical sequence object will be		*/
/*	constructed ; if it does not exists a warning will be		*/
/*	reported.													*/
/*																*/
/*	The procedure for constructing critical and non-critical	*/
/*	sequences is repeated for all of the time steps.			*/
/*									*/
/*	Patch type base stations however are special and do not 	*/
/*	need to contain critical sequences but rather contain 		*/
/*	only dated sequences that are associated with 			*/
/*	temporal inputs such as irrigation and fertilizers		*/
/* 	Patch type base stations are noted by having NULL climate inputs */
/*																*/
/*	NOTE:  MISSING DATA IN ANY CLIM SEQUENCE SHOULD BE FLAGGED	*/
/*			AS -999.0 FOR FLOAT SEQUENCES AND -999 FOR INTEGER	*/
/*			SEQUENCES.  THE CODE WILL HANDLE MISSING DATA IN 	*/
/*			A DOCUMENTED FASHION AFTER ALL OF THE BASE STATIONS	*/
/*			HAVE BEEN CONSTRUCTED.  THIS WILL ALLOW FOR 		*/
/*			REPLACEMENT DATA FROM NEIGHBOURING BASE STATIONS.	*/
/*			IN ADDITION, SOME MISSING DATA MAY BE SYNTHESIZED	*/
/*			USING A CLIMATE POSTPROCESSOR (E.G. MTCLIM).		*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	This code only sets up the creation of the base station.	*/
/*	Construting the climate sequence objects is done by			*/
/*	calls to other code.										*/
/*																*/
/*	A base station file is laid out as follows:					*/
/*																*/
/*	<start of file>												*/
/*	ID															*/
/*	x location										meters		*/
/*	y location										meters		*/
/*	z location										meters		*/
/*	isohyet											mm			*/
/*	base_station_effective_lai						m^2/m^2		*/
/*	critical_annual_file_prefix									*/
/*	num_annual_file_non_critical_sequences (n)					*/
/*	name of annual non critical sequnce file 1					*/
/*	name of annual non critical sequnce file 2					*/
/*	name of annual non critical sequnce file 3					*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	name of annual non critical sequnce file n					*/
/*	critical_monthly_file_prefix								*/
/*	num_monthly_file_non_critical_sequences (n)					*/
/*	name of monthly non critical sequnce file 1					*/
/*	name of monthly non critical sequnce file 2					*/
/*	name of monthly non critical sequnce file 3					*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	name of mnothly non critical sequnce file n					*/
/*	critical_daily_sequence_file_prefix							*/
/*	num_daily_file_non_critical_sequences (n)					*/
/*	name of daily non critical sequnce file 1					*/
/*	name of daily non critical sequnce file 2					*/
/*	name of daily non critical sequnce file 3					*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	name of daily non critical sequnce file n					*/
/*	critical_hourly_file_prefix									*/
/*	num_hourly_file_non_critical_sequences (n)					*/
/*	name of hourly non critical sequnce file 1					*/
/*	name of hourly non critical sequnce file 2					*/
/*	name of hourly non critical sequnce file 3					*/
/*		.														*/
/*		.														*/
/*		.														*/
/*	name of hourly non critical sequnce file n					*/
/*																*/	
/*	At the moment the code requires all of these entries for	*/
/*	all time steps.  The data file may have to have dummy		*/
/*	files for time steps for which there is no data.			*/
/*																*/
/*	The code which constructs clim objects ensures that they	*/
/*	extend from the start of the world to the end of the world	*/
/*	so there is no explicit date index for climate data in the	*/
/*	objects.  Each clim object consists of a set of pointers	*/
/*	to critical and non-critical climate sequences.  The 		*/
/*	critical sequences are always allocated while the non- 		*/
/*	critical sequences are only allocated if present.  In this	*/
/*	manner it is not neccessary to pre-allocate RAM for			*/
/*	parameters which may not be provided.   					*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <rhessys.h>

struct base_station_object* construct_ascii_grid
