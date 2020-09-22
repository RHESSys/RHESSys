/*
 * pointer_list.h
 *
 *  Created on: Sep 21, 2020
 *      Author: miles
 */

#ifndef INCLUDE_POINTER_LIST_H_
#define INCLUDE_POINTER_LIST_H_

typedef struct pointer_list_s {
	void *ptr;
	struct pointer_list_s *next;
} PointerList_t;

PointerList_t *newPointerList(void *ptr);
void freePointerList(PointerList_t *list);

PointerList_t *pointerListAppend(PointerList_t *list, void *ptr);

void printPointerList(PointerList_t *list);

#endif /* INCLUDE_POINTER_LIST_H_ */
