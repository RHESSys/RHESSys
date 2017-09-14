#include	<math.h>
#include	"rhessys.h"

#define IGREG 2299161
/*--------------------------------------------------------------*/
/*	caldat - computes calendar date given julian day			*/
/*																*/
/*	Taken from Numerical Recepies in C p. 13.					*/
/*																*/
/*	PROGRAMMERS NOTES											*/
/*																*/
/*	Converted function from void to return a date type.			*/
/*	Adjusted algorithm to report an hour=1 for the first		*/
/*	 	hour of that calendar day.								*/
/*--------------------------------------------------------------*/

struct	date	caldat(long julian)
{
	/* inverse of the function julday.  Here julian is input as Julian
	Day Number and the routine outputs the hour, day, month and year on
	which the specified Julian Day started at.  */
	long	ja, jalpha, jb, jc, jd, je;
	struct	date	calendar_date;
	/*	Cross-over to Gregorian calendar produces this correction*/
	if (julian >= IGREG) {
		jalpha = (long)(((double)(julian - 1867216) - 0.25) / 36524.25);
		ja = julian + 1 + jalpha - (long)(0.25 * jalpha);
	}
	else
		ja = julian;
	jb = ja + 1524;
	jc = (long)(6680.0 + ((double)(jb - 2439870) - 122.1) / 365.25);
	jd = (long)(365 * jc + (0.25 * jc));
	je = (long)((jb - jd) / 30.6001);
	calendar_date.day = jb - jd - (int)(30.6001 * je);
	calendar_date.month = je - 1;
	if ( calendar_date.month > 12 ) calendar_date.month -= 12;
	calendar_date.year = jc - 4715;
	if (calendar_date.month>2) --(calendar_date.year);
	if (calendar_date.year <= 0) --(calendar_date.year);
	calendar_date.hour = 1;
	return(calendar_date);
}/*end caldat.c*/
