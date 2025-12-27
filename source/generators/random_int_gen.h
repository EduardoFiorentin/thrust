#ifndef RANDOM_INT_GEN
#define RANDOM_INT_GEN

#include "../source.h"

typedef struct {
    int min;
    int max;
} RandomIntParams;

int             random_int_gen_init(Context* context, uint32_t seed);
FieldValue      random_int_gen_next(Context* context, const FieldSpec* spec, const Record* record);
int             random_int_gen_shutdown(Context* context);

#endif