/** @file compute_gamma.h
 *  @brief Computes gamma for neighbours of each patch
 */
#ifndef COMPUTE_GAMMA_H
#define COMPUTE_GAMMA_H

#include "blender.h"
#include "util.h"
#include "patch_hash_table.h"

int compute_gamma(struct flow_struct *flow_table, int num_patches, PatchTable_t *patchTable, FILE *f1,
		float scale_trans, double cell, int sc_flag, int slp_flag, int d_flag, bool surface);

#endif
