#ifndef _ROOF_GEOMETRY_H_
#define _ROOF_GEOMETRY_H_

#include "util.h"

typedef struct roof_geometry_s roof_geometry_t;

extern bool construct_roof_geometry(roof_geometry_t** _rtn_geometry);
extern bool destroy_roof_geometry(roof_geometry_t** _rtn_geometry);
extern bool add_roof_square(roof_geometry_t* _roof_geometry, int _row, int _col);

#endif // _ROOF_GEOMETRY_H_
