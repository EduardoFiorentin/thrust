#include "random_int_service.h"
#include <stdio.h>

// Linear Congruential Generator (LCG)
// Simples, rápido, previsível e independente de rand()
#define A 1664525
#define C 1013904223

int random_int_service_require(Context* context, uint32_t seed) {
    if (context->random.initialized == CTX_INITIALIZED) {
        source_print("The random number generation service has already been initialized. Nothing further needs to be done.");
        return 0;
    }
    random_service_init(context, seed);
    return 1;
}

void random_service_init(Context* context, uint32_t seed) {
    
    if (seed == 0)
        seed = 123456789; // seed default segura

    context->random.state = seed;
    context->random.initialized = CTX_INITIALIZED;
}

uint32_t random_service_next(Context* context, uint32_t max) {
    context->random.state = (A * context->random.state + C);
    return context->random.state % (max + 1);
}

void random_service_shutdown(Context* context) {
    // Nothing to clean
}
