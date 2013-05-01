#include <stdio.h>
#include <glib.h>

#include "patch_hash_table.h"

void test_patch_hash_table() {
	PatchTable_t *table = allocatePatchHashTable(7);

	PatchKey_t k1 = { 1, 2, 3 };
	PatchTableValue_t v1 = 86;
	PatchTableValue_t v1a = 87;
	PatchKey_t k2 = { 2, 3, 4 };
	PatchTableValue_t v2 = 43;
	PatchKey_t k3 = { 3, 4, 5 };
	PatchTableValue_t v3 = 25;
	PatchKey_t k4 = { 3, 2, 1 };
	PatchTableValue_t v4 = 66;
	PatchKey_t k5 = { 4, 3, 2 };
	PatchTableValue_t v5 = 52;

	patchHashTableInsert(table, k1, v1);
	//printPatchHashTable(table);
	patchHashTableInsert(table, k2, v2);
	//printPatchHashTable(table);
	patchHashTableInsert(table, k3, v3);
	//printPatchHashTable(table);
	patchHashTableInsert(table, k4, v4);
	//printPatchHashTable(table);
	patchHashTableInsert(table, k5, v5);
	//printPatchHashTable(table);

	g_assert( patchHashTableGet(table, k1) == v1 );
	g_assert( patchHashTableGet(table, k2) == v2 );
	g_assert( patchHashTableGet(table, k3) == v3 );
	g_assert( patchHashTableGet(table, k4) == v4 );
	g_assert( patchHashTableGet(table, k5) == v5 );

	patchHashTableInsert(table, k1, v1a);
	//printPatchHashTable(table);
	g_assert( patchHashTableGet(table, k1) == v1a );

	freePatchHashTable(table);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL );
	g_test_add_func("/set1/test patch_hash_table", test_patch_hash_table);
	return g_test_run();
}
