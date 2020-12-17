#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "string_list.h"

void test_string_list() {
	StringList_t *l = stringListAppend(NULL, "one");
	g_assert(strcmp(l->str, "one") == 0);
	g_assert(l->next == NULL);

	stringListAppend(l, "two");
	g_assert(strcmp(l->next->str, "two") == 0);
	g_assert(l->next->next == NULL);

	stringListAppend(l, "three");
	g_assert(strcmp(l->next->next->str, "three") == 0);
	g_assert(l->next->next->next == NULL);

	stringListAppend(l, "four");
	g_assert(strcmp(l->next->next->next->str, "four") == 0);
	g_assert(l->next->next->next->next == NULL);

	stringListAppend(l, "five");
	g_assert(strcmp(l->next->next->next->next->str, "five") == 0);
	g_assert(l->next->next->next->next->next == NULL);

	printStringList(l);
	freeStringList(l);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test string_list", test_string_list);
	return g_test_run();
}
