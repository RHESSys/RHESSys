/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include <stdio.h>
#include <stdlib.h>

#include "normalize_roof_patches.h"

// For debugging only! Not good coding practice! Not thread safe!
static FILE* patch_file;

// This isn't restricted to roofs and could be pulled out as a general utility function
static bool consolidate_duplicate_receivers(
    struct flow_struct* _flow_table,
    int _patch)
{
    bool result = true;
    if(_flow_table == 0) {
        fprintf(stderr, "ERROR: Flow table pointer is NULL.\n");
        result = false;
    }
    else if(_patch < 0) {
        fprintf(stderr, "ERROR: Patch index, %d, is less than zero.\n", _patch);
        result = false;
    } else {
        // Walk over each adjacency in the list and look for duplicates in all of the adjacencies that follow this one in the list
        struct adj_struct* adjacency = 0;
        for(adjacency = _flow_table[_patch].adj_list; result && adjacency != 0 && adjacency->next != 0; adjacency = adjacency->next) {
            
            struct adj_struct* other_adj = 0;
            struct adj_struct* prev_adj = adjacency;
            for(other_adj = adjacency->next; result && other_adj != 0;) {

                // check for duplicate patch here. This equivalence checking would be good to encapsulate into an opaque adjacency
                // type.
                if(adjacency->patchID == other_adj->patchID &&
                   adjacency->hillID == other_adj->hillID &&
                   adjacency->zoneID == other_adj->zoneID) {

                    // sum the gammas
                    adjacency->gamma += other_adj->gamma;
                    
                    // remove the other adj
                    prev_adj->next = other_adj->next;
                    struct adj_struct* tmp_adj = other_adj;
                    other_adj = other_adj->next;
                    free(tmp_adj);
                    --_flow_table[_patch].num_adjacent;
                } else {

                    // skip this one
                    prev_adj = other_adj;
                    other_adj = other_adj->next;
                }
            }
        }
    }
    return result;
}

// This isn't restricted to roofs and could be pulled out as a general utility function
static bool normalize_receivers(
    struct flow_struct* _flow_table,
    int _patch)
{
    bool result = true;
    if(_flow_table == 0) {
        fprintf(stderr, "ERROR: Flow table pointer is NULL.\n");
        result = false;
    }
    else if(_patch < 0) {
        fprintf(stderr, "ERROR: Patch index, %d, is less than zero.\n", _patch);
        result = false;
    } else {
        
        // sum the receiver gammas
        float total_gamma = 0.0;
        for(struct adj_struct* adjacency = _flow_table[_patch].adj_list; result && adjacency; adjacency = adjacency->next) {
            total_gamma += adjacency->gamma;
        }
        
        // normalize each receivers gamma over the sum
        for(struct adj_struct* adjacency = _flow_table[_patch].adj_list; result && adjacency; adjacency = adjacency->next) {
            float new_gamma = adjacency->gamma / total_gamma;
            printf("Total gamma: %f\tAdjacency gamma: %f\tNew gamma: %f\n", total_gamma, adjacency->gamma, new_gamma);
            adjacency->gamma = new_gamma;
            printf("New gamma: %f\n", adjacency->gamma);
            fprintf(patch_file, "\t%f\n", adjacency->gamma);
        }
    }
    return result;
}

bool normalize_roof_patches(
    struct flow_struct* _flow_table,
    int _num_patches)
{
    bool result = true;
    if(_flow_table == 0) {
        fprintf(stderr, "ERROR: Flow table pointer is NULL.\n");
        result = false;
    } else if(_num_patches < 0) {
        fprintf(stderr, "ERROR: Number of patches, %d, is less than zero.\n", _num_patches);
        result = false;
    } else {

        // for debugging
        patch_file = fopen("RoofPatches.txt", "w");
        
        for(int index = 0; result && index < _num_patches; ++index) {
            if(_flow_table[index].land == LANDTYPE_ROOF) {

                
                /* Consolidate the duplicate receivers into a single entry */
                if(!consolidate_duplicate_receivers(_flow_table, index)) {
                    fprintf(stderr, "ERROR: Failed to consolidate the duplicate receivers for patch, %d.\n", index);
                    result = false;
                }

                fprintf(patch_file, "Roof patch: %d\tNum Adjacent: %d\n", _flow_table[index].patchID, _flow_table[index].num_adjacent);

                // Normalize the receivers
                if(!normalize_receivers(_flow_table, index)) {
                    fprintf(stderr, "ERROR: Failed to normalize the receivers for patch, %d.\n", index);
                    result = false;
                }

            }
        }

        fclose(patch_file);
    }
    return result;
}
