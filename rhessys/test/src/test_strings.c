#include <glib.h>
#include <string.h>

#include "rhessys.h"
#include "strings.h"

void test_strip() {
    char *null_str = NULL;
    char *strip1 = strip(null_str);
    g_assert(strip1 == NULL);

    char *two_char_str = "tu";
    char *strip2 = strip(two_char_str);
    g_assert(strcmp(strip2, two_char_str) == 0);

    char *short_str = "foo";
    char *strip3 = strip(short_str);
    g_assert(strcmp(strip3, "o") == 0);

    char *quoted_str = "\"foo-bar\"";
    g_assert(strcmp(quoted_str, "\"foo-bar\"") == 0);
    char *strip4 = strip(quoted_str);
    g_assert(strcmp(strip4, "foo-bar") == 0);
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL );
    g_test_add_func("/set1/test strings", test_strip);
    return g_test_run();
}