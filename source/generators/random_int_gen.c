#include "../services/random_int_service.h"
#include "random_int_gen.h"
#include "../../main.h"
#include "../../utils/printer.h"
#include <stdio.h>

int random_int_gen_init(Context* context, uint32_t seed) {
    return random_int_service_require(context, seed);
}

/*
Random int gerenator
@param 
    seed A number that specifies which sequence of numbers the rand() function will follow.
@param
    limit Upper random generation limit inclusive.

@return 
    FieldValue with field name and value
*/
FieldValue random_int_gen_next(Context* context, uint32_t table_pos, uint32_t column_pos) {
    // Converte os parâmetros genéricos para o tipo esperado

    ColumnRuntimeState column = (ColumnRuntimeState) context->tables[table_pos].columns[column_pos];
    ParamList* params = column.descriptor->generator_params;

    Param* param_max = search_param_by_key(params, "max");
    if (!param_max) {
        error_print_exit("SOURCE", "Param 'max' not found in random int generator.");
    }

    int value = random_service_next(context, param_max->value.i);
    // printf("%d\n", value);
    // Monta e retorna o valor gerado
    FieldValue fv;
    
    fv.type = FIELD_INT;
    fv.value.i = value;
    return fv;
}


int random_int_gen_shutdown(Context* context) {
    random_service_shutdown(context);
    return 1;   // TODO implementar verificação
}