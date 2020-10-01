/*
 * Convert RHESSys date struct into an ISO-8601-like datetime string, e.g.
 * 2020-10-10T10:00. Note, this code does not check for valid ranges of date
 * fields.
 */

#ifndef INCLUDE_ISODATE_H_
#define INCLUDE_ISODATE_H_

#define ISO_DATE_FMT "%ld-%02ld-%02ldT%02ld:%02d"
#define ISO_DATE_LEN 17

char *get_iso_date(struct date const * const date);

#endif /* INCLUDE_ISODATE_H_ */
