/*--------------------------------------------------------------*/
/*	wateryearday - computes wateryearday based on a		*/
/*	calendaryear start day					*/
/*								*/
/*								*/
/*	PROGRAMMERS NOTES					*/
/*   								*/
/*--------------------------------------------------------------*/
#include	<math.h>
#include	"rhessys.h"

long 	wateryearday(struct date calendar_date,
		     int    wyday_start)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	long	compute_year_day(struct date calendar_date);
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int yday;
	int wyday;

	yday= compute_year_day(calendar_date);

	/* Compute wateryear day for leap year when wateryear start is after Feb 29 */
	if (calendar_date.year % 4 == 0 && wyday_start >= 60) {
		if (yday >= (wyday_start + 1)){
			wyday = yday - wyday_start;
		} else {
			wyday = yday + (365 - wyday_start + 1);
		}

	/* Compute wateryear day for leap year when wateryear start is before Feb 29 */
	} else if (calendar_date.year % 4 == 0 && wyday_start < 60) {
		if (yday < wyday_start){
			wyday = yday + (365 - wyday_start + 1);
		}
		if (yday >= wyday_start && yday < 60){
			wyday = yday - wyday_start + 1;
		}
		if (yday >= 60){
			wyday = yday - (wyday_start-1);
		}

	/* Compute wateryear day during the year following a leap year when start is before Feb 29 */
	} else if (calendar_date.year % 4 == 1 && wyday_start < 60 && yday < wyday_start) {
		wyday = yday + (365 - wyday_start + 2);

	/* Compute wateryear day for a normal year */
	} else {
		if (yday >= wyday_start){
			wyday = yday - (wyday_start - 1);
		} else {
			wyday = yday + (365 - wyday_start + 1);
		}
	}

	return(wyday);
}/*end wateryearday.c*/


