/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/** @file check_neighbours.c
 *  @brief examines the neighbourhood of the patch figures out
 *  if it is at a border at any border, length of perimeter is added
 *
 *  Aug 2010 - AD changed perimeter calculation flag formula
 *  and set new adjacent counter initial value to 1.
 *
 */

#include <stdio.h>
#include <math.h> 
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "fileio.h"
#include "check_neighbours.h"

int check_neighbours(int inputRow, int inputCol, int *patch, int *zone, int *hill,
                     int *stream, double* roofs, struct flow_struct *flow_entry, int num_adj, FILE *f1,
                     int maxr, int maxc, int sc_flag, double cell, bool surface) {

    /* local function declarations */

    struct adj_struct *check_list();
    int check_list_min();
    int r, c;
    int p_neigh;
    int z_neigh;
    int h_neigh;
    int stream_neigh;
    int new_adj = 0;

    for (r = -1; r <= 1; r++) {
        for (c = -1; c <= 1; c++) {
            /* don't look at neighbours beyond the edge */
            if ((inputRow + r < maxr) && (inputRow + r >= 0) && (inputCol + c < maxc)
                && (inputCol + c >= 0)) {

                /* is the neighbour a different patch*/
                /* or is it outside the basin - in which case we ignore it */
                /* also, for stream pixels, ignore non-stream neighbours */
                int index;
                if(!row_col_to_index(inputRow + r, inputCol + c, maxr, maxc, &index)) {
                    fprintf(stderr, "ERROR: Failed to compute an index from row: %d, and column: %d\n",
                            inputRow + r, inputCol + c);
                    return -1;
                }
                p_neigh = patch[index];
                h_neigh = hill[index];
                z_neigh = zone[index];
                stream_neigh = stream[index];

                // For surface water routing roof neighbors are determined elsewhere so skip them here - Brian
                if(surface && is_roof(roofs[index])) {
                    continue;
                }
                
                if (((p_neigh != flow_entry->patchID)
                     || (z_neigh != flow_entry->zoneID)
                     || (h_neigh != flow_entry->hillID))
                    && ((p_neigh > 0) && (z_neigh > 0) && (h_neigh > 0))) {

                    /* if stream  add in stream network processing */
                    /* create a list of downstream neighbours if it does not exist already */

                    if ((flow_entry->land == 1) && (stream_neigh > 0)) {
                        if ((flow_entry->num_dsa == 0)) {
                            flow_entry->num_dsa = 1;
                            if ((flow_entry->adj_str_list =
                                 (struct adj_struct *) malloc(
                                     sizeof(struct adj_struct)))
                                == NULL ) {
                                printf("\nMemory Allocation Failed for %d",
                                       flow_entry->patchID);
                                exit(EXIT_FAILURE);
                            }
                            flow_entry->adj_str_list->patchID = p_neigh;
                            flow_entry->adj_str_list->zoneID = z_neigh;
                            flow_entry->adj_str_list->hillID = h_neigh;
                            flow_entry->adj_str_list->perimeter = 0.0;
                            flow_entry->adj_str_list->next = NULL;
                            flow_entry->adj_str_ptr = flow_entry->adj_str_list;
                        } else {
                            /* search list for other entries for this neighbour */
                            flow_entry->adj_str_ptr = flow_entry->adj_str_list;
                            flow_entry->adj_str_ptr =
                                (struct adj_struct *) check_list(p_neigh,
                                                                 z_neigh, h_neigh,
                                                                 flow_entry->num_dsa,
                                                                 flow_entry->adj_str_list);

                            /* is this the first instance of the neighbour */
                            if (((flow_entry->adj_str_ptr->patchID != p_neigh)
                                 || (flow_entry->adj_str_ptr->zoneID
                                     != z_neigh)
                                 || (flow_entry->adj_str_ptr->hillID
                                     != h_neigh))
                                && ((p_neigh > 0) && (z_neigh > 0)
                                    && (h_neigh > 0)))

                            {
                                if ((flow_entry->adj_str_ptr->next =
                                     (struct adj_struct *) malloc(
                                         sizeof(struct adj_struct)))
                                    == NULL )

                                {
                                    printf("\nMemory Allocation Failed for %d",
                                           flow_entry->patchID);
                                    exit(EXIT_FAILURE);
                                }
                                flow_entry->adj_str_ptr =
                                    flow_entry->adj_str_ptr->next;
                                flow_entry->num_dsa += 1;
                                flow_entry->adj_str_ptr->patchID = p_neigh;
                                flow_entry->adj_str_ptr->zoneID = z_neigh;
                                flow_entry->adj_str_ptr->hillID = h_neigh;
                                flow_entry->adj_str_ptr->next = NULL;
                                flow_entry->adj_str_ptr->perimeter = 0.0;

                            } /* end new entry */
                        }
                        /******* AD CHANGED FROM abs((c+r)/ 2)*1.0/1.0== 0.5 TO BELOW *******/
                        /******* WAS YIELDING ONLY 1s and 0s IN ORIGINAL FORM??? ************/
                        if (abs(c + r) * 1.0 / 1.0 == 1.0)
                            flow_entry->adj_str_ptr->perimeter += cell * 0.5;
                        else
                            flow_entry->adj_str_ptr->perimeter += cell * 1.0
                                / sqrt(2.0);
                    } /* end if stream */

                    /* now do regular flowtable processing */
                    /* create a list of neighbours if it does not exist already */
                    if ((num_adj == 0)) {
                        num_adj = 1;
                        /******** AD ADDED new_adj=1 HERE BECAUSE WE WERE ALWAYS *******/
                        /******** MISSING LAST CORNER ADJ PATCH???                               *******/
                        new_adj = 1;
                        if ((flow_entry->adj_list =
                             (struct adj_struct *) malloc(
                                 sizeof(struct adj_struct))) == NULL ) {
                            printf("\nMemory Allocation Failed for %d",
                                   flow_entry->patchID);
                            exit(EXIT_FAILURE);
                        }
                        flow_entry->adj_list->patchID = p_neigh;
                        flow_entry->adj_list->zoneID = z_neigh;
                        flow_entry->adj_list->hillID = h_neigh;
                        flow_entry->adj_list->perimeter = 0.0;
                        flow_entry->adj_list->next = NULL;
                        flow_entry->adj_ptr = flow_entry->adj_list;
                    } else {
                        /* search list for other entries for this neighbour */
                        flow_entry->adj_ptr = flow_entry->adj_list;
                        flow_entry->adj_ptr = (struct adj_struct *) check_list(
                            p_neigh, z_neigh, h_neigh, num_adj,
                            flow_entry->adj_list);

                        /* is this the first instance of the neighbour */
                        if (((flow_entry->adj_ptr->patchID != p_neigh)
                             || (flow_entry->adj_ptr->zoneID != z_neigh)
                             || (flow_entry->adj_ptr->hillID != h_neigh))
                            && ((p_neigh > 0) && (z_neigh > 0)
                                && (h_neigh > 0)))

                        {
                            /* land processing */
                            if (flow_entry->land != 1) {
                                if ((flow_entry->adj_ptr->next =
                                     (struct adj_struct *) malloc(
                                         sizeof(struct adj_struct)))
                                    == NULL )

                                {
                                    printf("\nMemory Allocation Failed for %d",
                                           flow_entry->patchID);
                                    exit(EXIT_FAILURE);
                                }
                                flow_entry->adj_ptr = flow_entry->adj_ptr->next;
                                new_adj += 1;
                                num_adj += 1;
                                flow_entry->adj_ptr->patchID = p_neigh;
                                flow_entry->adj_ptr->zoneID = z_neigh;
                                flow_entry->adj_ptr->hillID = h_neigh;
                                flow_entry->adj_ptr->perimeter = 0.0;
                                flow_entry->adj_ptr->next = NULL;
                            } /* end land processing */
                            else { /* stream processing */

                                if ((flow_entry->adj_ptr->next =
                                     (struct adj_struct *) malloc(
                                         sizeof(struct adj_struct)))
                                    == NULL )

                                {
                                    printf("\nMemory Allocation Failed for %d",
                                           flow_entry->patchID);
                                    exit(EXIT_FAILURE);
                                }
                                flow_entry->adj_ptr = flow_entry->adj_ptr->next;
                                new_adj += 1;
                                num_adj += 1;
                                flow_entry->adj_ptr->patchID = p_neigh;
                                flow_entry->adj_ptr->zoneID = z_neigh;
                                flow_entry->adj_ptr->hillID = h_neigh;
                                flow_entry->adj_ptr->perimeter = 0.0;
                                flow_entry->adj_ptr->next = NULL;
                            } /* end stream processing */

                        } /* end new entry */
                    }

                    /* add perimeter length */
                    /******* AD CHANGED FROM abs((c+r)/ 2)*1.0/1.0== 0.5 TO BELOW *******/
                    /******* WAS YIELDING ONLY 1s and 0s IN ABOVE FORM ******************/
                    if (abs(c + r) * 1.0 / 1.0 == 1.0) {
                        flow_entry->adj_ptr->perimeter += cell * 0.5;
                    } else {
                        flow_entry->adj_ptr->perimeter += cell * 1.0
                            / sqrt(2.0);
                    }

                } /* end is neigh */

            } /* end edges if */

        } /* end col */

    } /* end row */

    flow_entry->adj_ptr = flow_entry->adj_list;
    flow_entry->adj_str_ptr = flow_entry->adj_str_list;

    return (new_adj);

}

struct adj_struct *check_list(patchID, zoneID, hillID, max, list)
int patchID, zoneID, hillID;int max;struct adj_struct *list; {

    int i, fnd;

    fnd = 0;
    i = 0;

    while ((fnd == 0) && (i < max)) {
        if ((list->patchID == patchID) && (list->zoneID == zoneID)
            && (list->hillID == hillID))
            fnd = 1;
        else {
            if (list->next == NULL )
                i = max;
            else {
                i += 1;
                list = list->next;
            }
        }

    }

    return (list);

                                    }

