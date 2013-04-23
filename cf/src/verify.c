/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include <stdio.h>

#include "verify.h"

bool verify_num_adjacent_for_patch(
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

        // loop over all the adjacencies, counting them
        int num_adjacent = 0;
        for(struct adj_struct* adjacency = _flow_table[_patch].adj_list; result && adjacency; adjacency = adjacency->next) {
            ++num_adjacent;
        }
        if(num_adjacent != _flow_table[_patch].num_adjacent) {
            fprintf(stderr, "ERROR: The number of adjacencies in the list, %d, does not match the patches num_adjacent field, %d.\n",
                    num_adjacent, _flow_table[_patch].num_adjacent);
            result = false;
        }
    }
    return result;
}

bool verify_num_adjacent(
    struct flow_struct* _flow_table,
    int _num_patches)
{
    bool result = true;
    if(_flow_table == 0) {
        fprintf(stderr, "ERROR: Flow table pointer is NULL.\n");
        result = false;
    }
    else if(_num_patches < 0) {
        fprintf(stderr, "ERROR: Number of patches, %d, is less than zero.\n", _num_patches);
        result = false;
    } else {
        for(int index = 0; result && index < _num_patches; ++index) {
            if(!verify_num_adjacent_for_patch(_flow_table, index)) {
                fprintf(stderr, "ERROR: Patch, %d, has adjacency and num_adjacent mismatch.\n", index);
                result = false;
            }
        }
    }
    return result;
}
