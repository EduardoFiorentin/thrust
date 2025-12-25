#ifndef SOURCE_H
#define SOURCE_H

#include <stdint.h>

typedef struct {
    uint32_t state;         // random seed
} RandomServiceContext;

typedef struct {
    RandomServiceContext random;
} Context;

typedef struct {
    // config do genfile futuramente
} Genfile;

int run_thread_source(Genfile* genfile, Context* context);

#endif