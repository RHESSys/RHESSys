#ifndef _FILEIO_H_
#define _FILEIO_H_

#include "check_neighbours.h"

void header_help(int *maxr, int *maxc, char* fndem);
int find_stream(struct flow_struct *, int, int *);
void find_max_flna(struct flow_struct *flow_table, int curr, int *str_inx);
void pause();

#endif // _FILEIO_H_
