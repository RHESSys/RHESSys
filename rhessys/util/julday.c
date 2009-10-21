#include <stdio.h>
#include <math.h>
#include	"rhessys.h"

/*------------------------------------------------------------------*/
/*	julday - converts calendar date to julian day					*/
/*																	*/
/*	From numerical recepies in C p. 12.								*/
/*																	*/
/*	PROGRAMMERS NOTES												*/
/*																	*/
/*	Converted arguements to accepte date structure rather than		*/
/*	separate month, day and year.  Modified original comments		*/
/*	to reflect this.												*/
/*																	*/
/*------------------------------------------------------------------*/

/* Gregorian calendar was adopted on Oct. 15, 1582 */
#define IGREG (15+31L*(10+12L*1582)) 

long	julday( struct date calendar_date	)
/*--------------------------------------------------------------------
In this routine the julday returns the Julian Day Number which begins at
noon of the calendar date specified by month,  day , and year ,
all long  integer variables.  Positive year signifies A.D.; negative B.C..
Remember that the year after 1 B.C. was 1 A.D.
--------------------------------------------------------------------*/
{
	/*------------------------------------------------------*/
	/*	Local Function Definition. 							*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	long	jul;
	int	ja, jy, jm;
	
	if (calendar_date.year == 0) {
		/*	fprintf(stderr,"JULDAY: there is no year zero");*/
		return(0);
	}
	if (calendar_date.year < 0) ++(calendar_date.year);
	if (calendar_date.month > 2) {
		jy = calendar_date.year;
		jm = calendar_date.month+1;
	}
	else {
		jy = calendar_date.year - 1;
		jm = calendar_date.month + 13;
	}
	jul= (long)(floor(365.25 * jy) + floor(30.6001 * jm)
		+ calendar_date.day + 1720995);
	if (calendar_date.day + 31L * (calendar_date.month + 12L *calendar_date.year)
		>= IGREG) {
		ja = (int)(0.01 * jy);
		jul += (2 - ja + (int)(0.25 * ja));
	}
	return jul;
}/*end julday.c*/
