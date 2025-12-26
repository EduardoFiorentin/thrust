#ifndef RANDOM_INT_GEN
#define RANDOM_INT_GEN

#include "../source.h"

typedef struct {
    int min;
    int max;
} RandomIntParams;

FieldValue random_int_gen(Context* context, const FieldSpec* spec, const Record* record);


#endif