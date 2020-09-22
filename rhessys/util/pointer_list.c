#include <stdio.h>
#include <assert.h>

#include "pointer_list.h"


PointerList_t *newPointerList(void *ptr) {
	PointerList_t *l = (PointerList_t *) malloc(sizeof(PointerList_t));
	assert(l);
	l->next = NULL;
	l->ptr = ptr;

	return l;
}

void freePointerList(PointerList_t *list) {
	if (list == NULL) return;
	PointerList_t *next = list->next;
	// Free storage for current list entry
	free(list);
	// Free remaining elements in the list
	freePointerList(next);
}

PointerList_t *pointerListAppend(PointerList_t *list, void *ptr) {
	if (list == NULL) {
		// Create new list
		return newPointerList(ptr);
	} else {
		// Append onto existing list
		// Find the end of the list
		PointerList_t *l = list;
		while (l->next != NULL) {
			l = l->next;
		}
		l->next = newPointerList(ptr);
		return l->next;
	}
}

void printPointerList(PointerList_t *list) {
	fprintf(stderr, "PointerList@%p [\n", (void *)list);
	PointerList_t *l = list;
	while (l != NULL) {
		fprintf(stderr, "%p,\n", l->ptr);
		l = l->next;
	}
	fprintf(stderr, "]\n");
}
