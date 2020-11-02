#ifndef _INDEX_STRICT_FIELDS_H_
#define _INDEX_STRICT_FIELDS_H_

#include "struct_index.h"

StructIndex_t *new_struct_index();
void free_struct_index(StructIndex_t *i);
StructIndex_t *index_struct_fields();

#endif
