#ifndef RANDOM_INT_GEN
#define RANDOM_INT_GEN

#include "../source.h"

int             random_int_gen_init(Context* context, uint32_t seed);
FieldValue      random_int_gen_next(Context* context, uint32_t table_pos, uint32_t column_pos);
int             random_int_gen_shutdown(Context* context);

#endif