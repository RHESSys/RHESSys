#include <stdio.h>
#include <glib.h>

#include "dictionary.h"
#include "string_list.h"

void test_empty_dictionary() {
	Dictionary_t *d = newDictionary(7);
	printDictionary(d);

	// Test iteration
	StringList_t *key = d->keys;
	size_t i = 0;
	while (key != NULL) {
		key = key->next;
		i += 1;
	}
	g_assert(i == 0);

	freeDictionary(d);
}

void test_dictionary() {
	Dictionary_t *d = newDictionary(7);
	char *k1 = "patch_object.evaporation";
	DictionaryValue_t v1 = 42;
	char *k2 = "patch_object.Qout_total";
	DictionaryValue_t v2 = 23;
	char *k3 = "patch_object.rootzone.depth";
	DictionaryValue_t v3 = 73;
	char *k4 = "patch_object.Kdown_diffuse_subcanopy";
	DictionaryValue_t v4 = 123;
	char *k5 = "patch_object.exfiltration_unsat_zone";
	DictionaryValue_t v5 = 840;
	char *k6 = "patch_object.Kdown_direct_subcanopy";
	DictionaryValue_t v6 = 1024;
	char *k7 = "patch_object.transpiration_sat_zone";
	DictionaryValue_t v7 = 2043;
	char *k8 = "patch_object.transpiration_sat_zone";
	DictionaryValue_t v8 = 4043;

	dictionaryInsert(d, k1, v1);
	dictionaryInsert(d, k2, v2);
	dictionaryInsert(d, k3, v3);
	dictionaryInsert(d, k4, v4);
	dictionaryInsert(d, k5, v5);
	dictionaryInsert(d, k6, v6);
	dictionaryInsert(d, k7, v7);

	g_assert(dictionaryGet(d, k1) == v1);
	g_assert(dictionaryGet(d, k2) == v2);
	g_assert(dictionaryGet(d, k3) == v3);
	g_assert(dictionaryGet(d, k4) == v4);
	g_assert(dictionaryGet(d, k5) == v5);
	g_assert(dictionaryGet(d, k6) == v6);
	g_assert(dictionaryGet(d, k7) == v7);

	printDictionary(d);

	dictionaryInsert(d, k8, v8);
	g_assert(dictionaryGet(d, k8) == v8);

	printDictionary(d);

	// Test iteration over keys
	DictionaryValue_t values[] = {v1, v2, v3, v4, v5, v6, v8};
	StringList_t *key = d->keys;
	size_t i = 0;
	while (key != NULL) {
		g_assert(dictionaryGet(d, key->str) == values[i]);
		key = key->next;
		i += 1;
	}

	freeDictionary(d);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test empty dictionary", test_empty_dictionary);
	g_test_add_func("/set1/test dictionary", test_dictionary);
	return g_test_run();
}
