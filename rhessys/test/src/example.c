/** @file example.c
 *
 * 	@brief Example unit test
 * 	@note To compile: gcc example.c `pkg-config --cflags --libs glib-2.0` -o example
 */
#include <stdio.h>
#include <glib.h>

int maxi(int i1, int i2)
{
  return (i1 > i2) ? i1 : i2;
}

void test_maxi() {
  g_assert(2 == maxi(0,2));
}

int main(int argc, char **argv) {
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/set1/test maxi", test_maxi);
  return g_test_run();
}


