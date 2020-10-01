
#include <stdio.h>

#include "rhessys.h"
#include "isodate.h"


char *get_iso_date(struct date const * const d) {
	char *iso_date = malloc(ISO_DATE_LEN);
	snprintf(iso_date, ISO_DATE_LEN, ISO_DATE_FMT,
			d->year, d->month, d->day, d->hour, 0);
	return iso_date;
}
