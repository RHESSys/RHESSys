#ifndef INCLUDE_POINTER_SET_H_
#define INCLUDE_POINTER_SET_H_

typedef struct pointer_set_s {
	void *ptr;
	struct pointer_set_s *next;
} PointerSet;

PointerSet *newPointerSet(void *ptr);
void freePointerSet(PointerSet *set);

PointerSet *pointerSetAppend(PointerSet *set, void *ptr);

void printPointerSet(PointerSet *set);

#endif /* INCLUDE_POINTER_SET_H_ */
