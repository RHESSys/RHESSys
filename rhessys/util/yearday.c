/*--------------------------------------------------------------*/
/*	yearday - computes yearday given date		*/
/*																*/
/*																*/
/*	PROGRAMMERS NOTES											*/
/*   assumes that leap years do not exist (so 1 day possible error)																*/
/*--------------------------------------------------------------*/
#include	<math.h>
#include	"rhessys.h"

long 	yearday(struct date calender_date)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	long day;
	long cummulative_day_by_month[12]
		= {0,31,59,90,120,151,181,212,243,273,304,334};
	day = cummulative_day_by_month[calender_date.month-1]
		+ calender_date.day;
	return(day);
}/*end yearday.c*/
