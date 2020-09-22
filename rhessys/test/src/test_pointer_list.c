#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "pointer_list.h"

void test_pointer_list() {
	int dummy1 = 23;
	PointerList_t *l = pointerListAppend(NULL, &dummy1);
	g_assert(l->ptr  == &dummy1);
	g_assert(l->next == NULL);

	int dummy2 = 42;
	pointerListAppend(l, &dummy2);
	g_assert(l->next->ptr == &dummy2);
	g_assert(l->next->next == NULL);

	int dummy3 = 65;
	pointerListAppend(l, &dummy3);
	g_assert(l->next->next->ptr == &dummy3);
	g_assert(l->next->next->next == NULL);

	int dummy4 = 107;
	pointerListAppend(l, &dummy4);
	g_assert(l->next->next->next->ptr == &dummy4);
	g_assert(l->next->next->next->next == NULL);

	printPointerList(l);
	freePointerList(l);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test pointer_list", test_pointer_list);
	return g_test_run();
}
