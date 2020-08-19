#ifndef _INDEX_STRICT_FIELDS_H_
#define _INDEX_STRICT_FIELDS_H_

#include "struct_index.h"

StructIndex_t *newStructIndex();
void freeStructIndex(StructIndex_t *i);
StructIndex_t *index_struct_fields();

#endif
