#ifndef RANDOM_SERVICE_H
#define RANDOM_SERVICE_H

#include "../source.h"

void        random_service_init(Context* ctx, uint32_t seed);
uint32_t    random_service_next(Context* ctx, uint32_t max);
void        random_service_shutdown(Context* ctx);

#endif