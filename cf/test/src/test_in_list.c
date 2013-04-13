/** @file test_in_list.c
 *
 * 	@brief Test function in_list
 *
 */
#include <stdio.h>
#include <glib.h>

#include "in_list.h"

void test_in_list() {
	// Setup list
	int LIST_LEN = 4;
	int list[LIST_LEN];
	list[0] = -1; list[1] = 1; list[2] = 42; list[3] = 3;

	// Find our element
	int result = in_list(42, &list, LIST_LEN);
	g_assert(result == 2);
}

int main(int argc, char **argv) {
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/set1/test in_list", test_in_list);
  return g_test_run();
}
