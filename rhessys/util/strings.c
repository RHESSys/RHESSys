#include <string.h>

#include "strings.h"

char *strip(const char * const str) {
    char *stripped = NULL;
    if (str != NULL) {
        size_t len = strlen(str);
        if (len < 3) {
            stripped = strdup(str);
        } else {
            stripped = strndup(str + 1, len - 2);
        }
    }
    return stripped;
}
