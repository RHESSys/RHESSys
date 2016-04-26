/** @file example.c
 *
 * 	@brief Example unit test
 * 	@note To compile: gcc example.c `pkg-config --cflags --libs glib-2.0` -o example
 */
#include <stdio.h>
#include <math.h>

#include <glib.h>

#include "functions.h"


#define EPSILON 0.000000001


void test_compute_potential_exfiltration() {

	int verbose = 0;
	double S = 0.75;
	double sat_deficit_z = 2.0;
	double Ksat_0 = 2.0;
	double m_z = 0.12;

	double psi_air_entry = 0.4;
	double pore_size_index = 0.15;
	double porosity_decay = 4000.0;
	double porosity_surf = 0.45;

	double expected_value = 0.033968;
	double poten_exfil = compute_potential_exfiltration(verbose,
			S, sat_deficit_z, Ksat_0, m_z,
			psi_air_entry, pore_size_index,
			porosity_decay,
			porosity_surf);

	g_assert(abs(poten_exfil - expected_value) < EPSILON);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/set1/test compute_potential_exfiltration", test_compute_potential_exfiltration);

	return g_test_run();
}


