#include "random_int_service.h"

// Linear Congruential Generator (LCG)
// Simples, rápido, previsível e independente de rand()
#define A 1664525
#define C 1013904223

void random_service_init(Context* ctx, uint32_t seed) {
    
    if (ctx->random.initialized == '1') {
        source_print("The random number generation service has already been initialized. Nothing further needs to be done.");
        return;
    }
    
    if (seed == 0)
        seed = 123456789; // seed default segura

    ctx->random.state = seed;
    ctx->random.initialized = '1';
}

uint32_t random_service_next(Context* ctx, uint32_t max) {
    ctx->random.state = (A * ctx->random.state + C);
    return ctx->random.state % (max + 1);
}

void random_service_shutdown(Context* ctx) {
    // Nothing to clean
}
