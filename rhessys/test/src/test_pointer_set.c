#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "pointer_set.h"

void test_pointer_list() {
	int dummy1 = 23;
	PointerSet *r = pointerSetAppend(NULL, &dummy1);
	g_assert(r->ptr  == &dummy1);
	g_assert(r->next == NULL);

	int dummy2 = 42;
	pointerSetAppend(r, &dummy2);
	g_assert(r->next->ptr == &dummy2);
	g_assert(r->next->next == NULL);

	int dummy3 = 65;
	pointerSetAppend(r, &dummy3);
	g_assert(r->next->next->ptr == &dummy3);
	g_assert(r->next->next->next == NULL);

	pointerSetAppend(r, &dummy3);
	g_assert(r->next->next->ptr == &dummy3);
	g_assert(r->next->next->next == NULL);

	int dummy4 = 107;
	pointerSetAppend(r, &dummy4);
	g_assert(r->next->next->next->ptr == &dummy4);
	g_assert(r->next->next->next->next == NULL);

	pointerSetAppend(r, &dummy1);
	g_assert(r->ptr  == &dummy1);
	g_assert(r->next != NULL);

	printPointerSet(r);
	freePointerSet(r);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test pointer_list", test_pointer_list);
	return g_test_run();
}
