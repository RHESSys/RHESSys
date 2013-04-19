/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#include <stdio.h>
#include <stdlib.h>

#include "blender.h"
#include "add_flow_to_table.h"
#include "find_patch.h"

bool create_flow_adjacency(
    struct flow_struct* _flow_table,
    int _index,
    double _proportion,
    struct adj_struct** _rtn_adjacency)
{
    bool result = true;
    if(_flow_table == 0) {
        fprintf(stderr, "ERROR: Flow table pointer is NULL.\n");
        result = false;
    } else if(_rtn_adjacency == 0) {
        fprintf(stderr, "ERROR: Return adjacency pointer is NULL.\n");
        result = false;
    } else if(_index < 0) {
        fprintf(stderr, "ERROR: Flow table index less than zero.\n");
        result = false;
    } else if(_proportion < 0 || _proportion > 1) {
        fprintf(stderr, "ERROR: Proportion is out of bounds.\n");
        result = false;
    } else {
        // do stuff
        struct adj_struct* adjacency = (struct adj_struct*)calloc(1, sizeof(struct adj_struct));
        if(adjacency == 0) {
            fprintf(stderr, "ERROR: Failed to allocate flow table adjacency struct.\n");
            result = false;
        } else {
            // from the flow table
            adjacency->patchID = _flow_table[_index].patchID;
            adjacency->hillID = _flow_table[_index].hillID;
            adjacency->zoneID = _flow_table[_index].zoneID;
            adjacency->landtype = _flow_table[_index].land;
            adjacency->sewertype = _flow_table[_index].sewer;
            adjacency->slope = _flow_table[_index].slope;
            adjacency->z = _flow_table[_index].z;

            // computed
            adjacency->inx = _index;
            adjacency->gamma = (float)_proportion;
            adjacency->next = 0;
            adjacency->perimeter = 0;

            *_rtn_adjacency = adjacency;
        }
    }
    
    return result;
}

bool add_adjacency_to_contributor(
    struct flow_struct* _flow_table,
    int _index,
    struct adj_struct* _adjacency)
{
    bool result = true;
    if(_flow_table == 0) {
        fprintf(stderr, "ERROR: Flow table pointer is NULL.\n");
        result = false;
    } else if(_adjacency == 0) {
        fprintf(stderr, "ERROR: Adjacency pointer is NULL.\n");
        result = false;
    } else if(_index < 0) {
        fprintf(stderr, "ERROR: Contributor index is less than 0.\n");
        result = false;
    } else {
        struct adj_struct* adjacency_tail = _flow_table[_index].adj_ptr;
        _flow_table[_index].adj_ptr = _adjacency;
        adjacency_tail->next = _adjacency;
    }
    return result;
}

bool add_flow_to_table(
    int _contributor_row,
    int _contributor_col,
    int _receiver_row,
    int _receiver_col,
    int _maxr,
    int _maxc,
    struct flow_struct* _flow_table,
    int _num_patches,
    const int* _patch,
    const int* _hill,
    const int* _zone,
    double _proportion)
{
    bool result = true;
    int contributor_index = 0;
    int receiver_index = 0;
    
    if(_flow_table == 0) {
        fprintf(stderr, "ERROR: Flow table pointer is NULL.\n");
        result = false;
    } else if(_patch == 0) {
        fprintf(stderr, "ERROR: Patch pointer is NULL.\n");
        result = false;
    } else if(_proportion < 0 || _proportion > 1) {
        fprintf(stderr, "ERROR: The roof proportion is out of range.\n");
        result = false;
    } else if(!row_col_to_index(_contributor_row, _contributor_col, _maxr, _maxc, &contributor_index)) {
        fprintf(stderr, "ERROR: Failed to map contributor row: %d and column %d to an index.\n", _contributor_row, _contributor_col);
        result = false;
    } else if(!row_col_to_index(_receiver_row, _receiver_col, _maxr, _maxc, &receiver_index)) {
        fprintf(stderr, "ERROR: Failed to map receiver row: %d and column %d to an index.\n", _receiver_row, _receiver_col);
        result = false;
    }
    else {
        // do the sciences
        int contributor_patch = find_patch(_num_patches, _flow_table, _patch[contributor_index], _zone[contributor_index], _hill[contributor_index]);
        int receiver_patch = find_patch(_num_patches, _flow_table, _patch[receiver_index], _zone[receiver_index], _hill[receiver_index]);

        // Create an adjacency for the receiver
        struct adj_struct* adjacency = 0;
        if(!create_flow_adjacency(_flow_table, receiver_patch, _proportion, &adjacency)) {
            fprintf(stderr, "ERROR: Failed to create receiver adjacency.\n");
            result = false;
        }

        // Add the adjacency to the contributor
        else if(!add_adjacency_to_contributor(_flow_table, contributor_patch, adjacency)) {
            fprintf(stderr, "ERROR: Failed to add receiver adjacency to the contributor.\n");
            result = false;
        }
    }
    return result;
}
