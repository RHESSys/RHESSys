#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "pointer_set.h"


PointerSet *newPointerSet(void *ptr) {
	PointerSet *s = (PointerSet *) malloc(sizeof(PointerSet));
	assert(s);
	s->next = NULL;
	s->ptr = ptr;

	return s;
}

void freePointerSet(PointerSet *set) {
	if (set == NULL) return;
	PointerSet *next = set->next;
	// Free storage for current set entry
	free(set);
	// Free remaining elements in the set
	freePointerSet(next);
}

PointerSet *pointerSetAppend(PointerSet *set, void *ptr) {
	if (set == NULL) {
		// Create new set
		return newPointerSet(ptr);
	} else {
		// Append onto existing set
		// Find the end of the set
		// Head of set is the same as the value to be appended, don't add it to the set, return it.
		if (set->ptr == ptr) return set;
		PointerSet *s = set;
		while (s->next != NULL) {
			s = s->next;
			// Current entry in set is the same as the value to be appended, don't add it to the set, return it.
			if (s->ptr == ptr) return s;
		}
		s->next = newPointerSet(ptr);
		return s->next;
	}
}

void printPointerSet(PointerSet *set) {
	fprintf(stderr, "PointerSet@%p {\n", (void *)set);
	PointerSet *s = set;
	while (s != NULL) {
		fprintf(stderr, "%p,\n", s->ptr);
		s = s->next;
	}
	fprintf(stderr, "}\n");
}
