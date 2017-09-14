#include <stdio.h>
#include <math.h>
#include	"rhessys.h"

/*------------------------------------------------------------------*/
/*	julday - computes elapsed days stince start of year				*/
/*																	*/
/*	PROGRAMMERS NOTES												*/
/*																	*/
/*------------------------------------------------------------------*/
int	compute_year_day( struct date calendar_date	)
{

	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	long julday(struct date);	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int	year_day;
	struct	date 	jan_1_date;
	/*------------------------------------------------------------------*/
	/*	Define the first day of the year.								*/
	/*------------------------------------------------------------------*/
	jan_1_date.year = calendar_date.year;
	jan_1_date.month = 1;
	jan_1_date.day = 1;
	jan_1_date.hour = 1;
	/*------------------------------------------------------------------*/
	/*	Compute the elapsed days from start of year.					*/
	/*------------------------------------------------------------------*/
	year_day = julday(calendar_date) - julday(jan_1_date) + 1;
	return(year_day);
}/*end compute_year_day*/
