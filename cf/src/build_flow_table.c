/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/** @file build_flow_table.c
 *      @brief Build the overall structure of a flow table
 */
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "main.h"
#include "blender.h"
#include "fileio.h"
#include "check_neighbours.h"
#include "zero_flow_table.h"
#include "find_patch.h"
#include "build_flow_table.h"

int build_flow_table(struct flow_struct* flow_table, double* dem, float* slope,
                     int* hill, int* zone, int* patch, int* stream, int* roads, int* sewers,
                     double* flna, FILE* f1, int maxr, int maxc, int f_flag, int sc_flag,
                     int sewer_flag, int slp_flag, double cell, double scale_dem) {

    /* local variable declarations */
    int num_patches;
    int inx;
    int r, c, pch;

    num_patches = 0;

    zero_flow_table(flow_table, maxr, maxc);

    for (r = 0; r < maxr; r++) {

        for (c = 0; c < maxc; c++) {
            inx = r * maxc + c;
            if (patch[inx] == NO_DATA) {
                printf(
                    "error in patch file use of NO_DATA as a patch label not allowed \n");
                exit(EXIT_FAILURE);
            }

            /* ignore areas outside the basin */
            if ((patch[inx] > 0) && (zone[inx] > 0) && (hill[inx] > 0)) {
                pch = find_patch(num_patches, flow_table, patch[inx], zone[inx],
                                 hill[inx]);
                if (pch == 0) {
                    num_patches += 1;
                    pch = num_patches;
                }

                flow_table[pch].patchID = patch[inx];
                flow_table[pch].hillID = hill[inx];
                flow_table[pch].zoneID = zone[inx];
                flow_table[pch].area += 1;
                flow_table[pch].x += (float) (1.0 * r);
                flow_table[pch].y += (float) (1.0 * c);
                flow_table[pch].z += (float) dem[inx];
                if (sewer_flag)
                    flow_table[pch].sewer += (int) sewers[inx];
                if ((STREAM_CONNECTIVITY_RANDOM == sc_flag)
                    || (SLOPE_STANDARD != slp_flag)) {
                    flow_table[pch].internal_slope += (float) (1.0 * slope[inx]
                                                               * DtoR);
                    if (flow_table[pch].max_slope < slope[inx])
                        flow_table[pch].max_slope = (float) (1.0 * slope[inx]);
                }
                if (roads[inx] >= 1)
                    flow_table[pch].land = 2;
                if (stream[inx] >= 1)
                    flow_table[pch].land = 1;

                if (f_flag)
                    flow_table[pch].flna += (float) flna[inx];
                else
                    flow_table[pch].flna = 0.0;

                flow_table[pch].num_adjacent += check_neighbours(r, c, patch,
                                                                 zone, hill, stream, &flow_table[pch],
                                                                 flow_table[pch].num_adjacent, f1, maxr, maxc, sc_flag,
                                                                 cell);
            } /* end if */
        }

    }

    printf("\n Total number of patches is %d", num_patches);

    return (num_patches);

}

