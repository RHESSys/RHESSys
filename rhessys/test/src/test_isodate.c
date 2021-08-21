#include <stdio.h>
#include <glib.h>

#include "rhessys.h"
#include "isodate.h"


void test_isodate() {
	struct date d1 = {2020, 10, 10, 10};
	char *iso1 = get_iso_date(&d1);
	g_assert(strcmp("2020-10-10T10:00", iso1) == 0);

	struct date d2 = {2020, 1, 1, 1};
	char *iso2 = get_iso_date(&d2);
	g_assert(strcmp("2020-01-01T01:00", iso2) == 0);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test isodate", test_isodate);
	return g_test_run();
}
