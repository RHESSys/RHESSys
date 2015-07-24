/*--------------------------------------------------------------*/
/* 																*/
/*							cal_date_lt							*/
/*																*/
/*	cal_date_lt - compares two calendar dates and returns 1	*/
/*					if the first is earlier than the second.	*/
/*																*/
/*	NAME														*/
/*	cal_date_lt - compares two calendar dates and returns 1	*/
/*					if the first is earlier than the second.	*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	The routine performs as follows:							*/
/*																*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

int	 cal_date_lt( struct date date1,
				 struct date date2) 
{
	/*--------------------------------------------------------------*/
	/*	Local function declarations.								*/
	/*--------------------------------------------------------------*/
	long	julday( struct date );
	/*--------------------------------------------------------------*/
	/*	Local Variable Definition. 									*/
	/*--------------------------------------------------------------*/
	long	julday1;
	long	julday2;
	/*--------------------------------------------------------------*/
	/*	Check if the julian days are different.						*/
	/*--------------------------------------------------------------*/
	julday1 = 0.0;
	julday2 = 0.0;
	julday1 = julday(date1);
	julday2 = julday(date2);
	if ( julday1 == julday2 ){
		if ( date1.hour < date2.hour ){
			return(1);
		}
		else{
			return(0);
		}
	}
	else if ( julday1 < julday2 ){
		return(1);
	}
	else{
		return(0);
	}
} /*end cal_date_lt.c*/
