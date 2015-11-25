/*--------------------------------------------------------------*/
/*	wateryearday - computes wateryearday given date (starting in October)		*/
/*																*/
/*																*/
/*	PROGRAMMERS NOTES											*/
/*   assumes that leap years do not exist (so 1 day possible error)																*/
/*--------------------------------------------------------------*/
#include	<math.h>
#include	"rhessys.h"

long 	wateryearday(struct date calender_date)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	long wyday;
	long cummulative_day_by_month[12]
		= {92,123,151,182,212,243,273,304,335,0,31,61};
	wyday = cummulative_day_by_month[calender_date.month-1]
		+ calender_date.day;
	return(wyday);
}/*end wateryearday.c*/
