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
#include "util.h"
#include "patch_hash_table.h"
#include "build_flow_table.h"

int build_flow_table(struct flow_struct* flow_table, PatchTable_t *patchTable, double* dem, float* slope,
                     int* hill, int* zone, int* patch, int* stream, int* roads, int* sewers, double* roofs,
                     double* flna, FILE* f1, int maxr, int maxc, int f_flag, int sc_flag,
                     int sewer_flag, int slp_flag, double cell, double scale_dem, bool surface) {

    /* local variable declarations */
    int num_patches;
    int inx;
    int r, c, pch;

    num_patches = 0;

    zero_flow_table(flow_table, maxr, maxc);

    for (r = 0; r < maxr; r++) {

        for (c = 0; c < maxc; c++) {
            if(!row_col_to_index(r, c, maxr, maxc, &inx)) {
                fprintf(stderr, "ERROR: Failed to compute an index from row: %d and column: %d.\n", r, c);
                return -1;
            }
            
            if (patch[inx] == NO_DATA) {
                printf(
                    "error in patch file use of NO_DATA as a patch label not allowed \n");
                exit(EXIT_FAILURE);
            }

            /* ignore areas outside the basin */
            if ((patch[inx] > 0) && (zone[inx] > 0) && (hill[inx] > 0)) {
            	PatchKey_t k = { patch[inx], zone[inx], hill[inx] };
            	pch = patchHashTableGet(patchTable, k);
            	if ( PATCH_HASH_TABLE_EMPTY == pch ) {
            		num_patches++;
            		pch = num_patches;
            		patchHashTableInsert(patchTable, k, pch);
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
                // land of type LANDTYPE_LAND is assumed
                if (surface && is_roof(roofs[inx]))
                    flow_table[pch].land = LANDTYPE_ROOF;
                if (roads[inx] >= 1)
                    flow_table[pch].land = LANDTYPE_ROAD;
                if (stream[inx] >= 1)
                    flow_table[pch].land = LANDTYPE_STREAM;

                if (f_flag)
                    flow_table[pch].flna += (float) flna[inx];
                else
                    flow_table[pch].flna = 0.0;

                // Debug
				fprintf(f1, "patch[%d]: %d %d %d %d\n",
						pch, flow_table[pch].patchID, flow_table[pch].hillID, flow_table[pch].zoneID,
						flow_table[pch].land);

				// num_adjacent should only be set once in zero_flow_table
				// enabling the line below causes the adjacency list to
				// be trashed by check_neighbours each time through this
				// loop. (selimnairb)
                //flow_table[pch].num_adjacent = 0;
                if(!surface || flow_table[pch].land != LANDTYPE_ROOF) {
                    int num_adj =  check_neighbours(r, c, patch, zone, hill, stream, roofs, &flow_table[pch],
                                                    flow_table[pch].num_adjacent, f1, maxr, maxc, sc_flag,
                                                    cell, surface);
                    if(num_adj < 0) {
                        fprintf(stderr, "ERROR: An error occurred while determing patch neighbors.\n");
                        return -1;
                    }
                    flow_table[pch].num_adjacent += num_adj;

                    // Debug output
                    struct adj_struct *adj = flow_table[pch].adj_list;
                    while (adj != NULL) {
                    	fprintf(f1, "\tadj: %d %d %d %d\n", adj->patchID, adj->hillID, adj->zoneID, adj->landtype);
                    	adj = adj->next;
                    }
                }
                
            } /* end if */
        }

    }

    printf("\n Total number of patches is %d", num_patches);

    return (num_patches);

}

