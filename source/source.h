#ifndef SOURCE_H
#define SOURCE_H

#include <stdint.h>
#include "../main.h"

// Context structures
typedef struct {
    uint32_t state;         // random seed
} RandomServiceContext;

typedef struct {
    RandomServiceContext random;
} Context;


int run_thread_source(Genfile* genfile, Context* context);

#endif