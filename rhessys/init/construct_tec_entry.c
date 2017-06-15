/*--------------------------------------------------------------*/
/* 																*/
/*						construct_tec_file						*/
/*																*/
/*	construct_tec_entry.c - creates a tec file entry		 OK	*/
/*																*/
/*	NAME														*/
/*																*/
/*	SYNOPSIS													*/
/*	struct	tec_entry  *construct_tec_entry(					*/
/*					struct date, char *);						*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "rhessys.h"

struct tec_entry	*construct_tec_entry(
										 struct date cal_date,
										 char *tec_command)
{
	/*--------------------------------------------------------------*/
	/*	Local FUnction Declarations 				*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*	Local Variable Definition. 									*/
	/*--------------------------------------------------------------*/
	struct	tec_entry 	*event;
	/*--------------------------------------------------------------*/
	/*  Allocate a tec event entry  object							*/
	/*--------------------------------------------------------------*/
	event = (struct tec_entry *) alloc( 1 *
		sizeof(struct tec_entry),"entry","construct_tec_entry");
	/*--------------------------------------------------------------*/
	/*	Initialize time of  tec event								*/
	/*--------------------------------------------------------------*/
	event[0].cal_date.year = cal_date.year;
	event[0].cal_date.month = cal_date.month;
	event[0].cal_date.day = cal_date.day;
	event[0].cal_date.hour = cal_date.hour;
	/*--------------------------------------------------------------*/
	/* 	Initialze event command										*/
	/*--------------------------------------------------------------*/
	strcpy(event[0].command, tec_command);
	return(event);
} /*end construct_tec_entry.c*/
