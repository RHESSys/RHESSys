/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#ifndef _SUB_H_
#define _SUB_H_

#include "build_flow_table.h"
#include "compute_gamma.h"
#include "find_patch.h"
#include "patch_hash_table.h"

void input_prompt(int *, int *, char *, char *, char *, char *, char *, char *,
                  char *, char *, char *, char *, char *, char *, char*, char *, char *,
                  int, int, int);

struct ID_struct sort_flow_table(struct flow_struct *flow_table, int num_patches, PatchTable_t *patchTable);
void add_roads(struct flow_struct *, int, FILE *, double);
void route_roads_to_patches(struct flow_struct *, int, int);

int compute_upslope_area(struct flow_struct *, int, FILE *, int, double);
int compute_dist_from_road(struct flow_struct *, int, FILE *, double);
int compute_drainage_density(struct flow_struct *, int, double);
void remove_pits(struct flow_struct *flow_table, int num_patches,
			int sc_flag, int slp_flag, double cell, FILE *f1);

void input_ascii_int(int *, char *, int, int, int);
void input_ascii_float(float *, char *, int, int, int, float);
void output_ascii_float(float *, char *, int, int);
void input_ascii_sint(short int *, char *, int, int, int);
void print_flow_table(int, struct flow_struct *, int, int, double, double,
                      char *, char *, double);
void print_stream_table(int, int, struct flow_struct *, int, int, double,
                        double, char *, char *, double, int);
void print_drain_stats(int, struct flow_struct *);


#endif // _SUB_H_
