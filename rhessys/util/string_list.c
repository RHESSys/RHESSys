/** @file string_list.c
 *  @brief Implements functions for a singly linked list of strings. Makes a deep copy of
 *  strings on insert.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "output_filter.h"
#include "string_list.h"

StringList_t *newStringList(char *str) {
	StringList_t *l = (StringList_t *) malloc(sizeof(StringList_t));
	assert(l);
	l->next = NULL;
	// Allocate memory for string
	size_t strSize = strlen(str) + 1;
	l->str = (char *) malloc(strSize);
	strncpy(l->str, str, strSize);

	return l;
}

void freeStringList(StringList_t *list) {
	if (list == NULL) return;
	StringList_t *next = list->next;
	// Free storage for current string data
	free(list->str);
	// Free storage for current list entry
	free(list);
	// Free remaining elements in the list
	freeStringList(next);
}

StringList_t *stringListAppend(StringList_t *list, char *str) {
	if (list == NULL) {
		// Create new list
		return newStringList(str);
	} else {
		// Append onto existing list
		// Find the end of the list
		StringList_t *l = list;
		while (l->next != NULL) {
			l = l->next;
		}
		l->next = newStringList(str);
		return l->next;
	}
}

void printStringList(StringList_t *list) {
	fprintf(stderr, "StringList@%p [\n", (void *)list);
	StringList_t *l = list;
	while (l != NULL) {
		fprintf(stderr, "%s,\n", l->str);
		l = l->next;
	}
	fprintf(stderr, "]\n");
}

