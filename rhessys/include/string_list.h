/** @file string_list.h
 *  @brief Defines data and functions for a singly linked list of strings.
 *
 */
#ifndef _STRING_LIST_H_
#define _STRING_LIST_H_

#include <stdio.h>

typedef struct string_list_s {
	char *str;
	struct string_list_s *next;
} StringList_t;

StringList_t *newStringList(char *str);
void freeStringList(StringList_t *list);

StringList_t *stringListAppend(StringList_t *list, char *str);

void printStringList(StringList_t *list);

#endif
