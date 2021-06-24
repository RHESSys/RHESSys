//
// Created by Brian C Miles on 3/7/21.
//

#ifndef RHESSYS_STRINGS_H
#define RHESSYS_STRINGS_H

#include <strings.h>

/**
 * Strip the first and last characters from a string and return a copy of the string.
 * If str is NULL, return NULL. If len(str) < 3, return a copy of str unmodified.
 * @param str
 * @return A copy of the string with first and last characters missing. If str is NULL, return NULL. If len(str) < 3,
 * return a copy of str unmodified.
 */
char *strip(const char * const str);

#endif //RHESSYS_STRINGS_H
