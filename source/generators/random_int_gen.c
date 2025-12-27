#include "../services/random_int_service.h"
#include "random_int_gen.h"

/*
Random int gerenator
@param 
    seed: A number that specifies which sequence of numbers the rand() function will follow.
@param
    limit: Upper random generation limit inclusive.

@return 
    Pseudo random number between 0 and limit
*/
// int generate_random_age(Context* ctx) {
//     return random_service_next(ctx, 100);
// }

FieldValue random_int_gen(
    Context* context,
    const FieldSpec* spec,
    const Record* record
) {
    // Converte os parâmetros genéricos para o tipo esperado
    RandomIntParams* p = (RandomIntParams*) spec->params;

    // Usa os parâmetros diretamente, sem parsing adicional
    int value = random_service_next(context, p->max);

    // Monta e retorna o valor gerado
    FieldValue fv;
    fv.type = FIELD_INT;
    fv.value.i = value;
    return fv;
}
