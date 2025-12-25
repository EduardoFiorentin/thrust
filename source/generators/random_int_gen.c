#include "../services/random_service.h"


/*
Random int gerenator
@param 
    seed: A number that specifies which sequence of numbers the rand() function will follow.
@param
    limit: Upper random generation limit inclusive.

@return 
    Pseudo random number between 0 and limit
*/
int generate_random_age(Context* ctx) {
    return random_service_next(ctx, 100);
}