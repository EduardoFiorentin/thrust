#include "main.h"
#include "source/source.h"
#include "source/generators/random_int_gen.h"
#include <string.h>
#include <stdio.h>

// TODO Mover implementação para um lugar adequado
Param* search_param_by_key(ParamList* params, const char* key) {
    for (int i = 0; i < params->param_count; i++) {
        if (strcmp(params->params[i].key, key) == 0) return &params->params[i]; 
    } 
    return NULL;
}

int main() 
{   
    Genfile genfile;
    
    // load genfile

    // validate genfile

    // inicialização do service 
    RandomIntServiceContext random;
    random.initialized = CTX_NOT_INITIALIZED;


   
    // inicialização do modelo a ser gerado
    
    // parametros do generator
    // RandomIntParams randomIntParams1;
    // randomIntParams1.max = 200;
    // randomIntParams1.min = 100;


    // Parte dos parametros será substituida pelo sistema generico de parametros futuramente
    // RandomIntParams randomIntParams2;
    // randomIntParams2.max = 9999;
    // randomIntParams2.min = 100;

    // Nova forma de passagem de parâmetros
    // ------------------------------------------------------------------------------------

    // Parametro do ColumnDescriptor 1 - RamdomIntParams
    ParamList paramsRamdomInt1; 
    paramsRamdomInt1.param_count = 2;
    paramsRamdomInt1.params = (Param*) malloc(sizeof(Param) * paramsRamdomInt1.param_count);

    Param min1, max1; 
    min1.key = "min";
    min1.type = PARAM_INT;
    min1.value.i = 100;

    max1.key = "max";
    max1.type = PARAM_INT;
    max1.value.i = 10000;

    paramsRamdomInt1.params[0] = min1;
    paramsRamdomInt1.params[1] = max1;


    // Parametro do ColumnDescriptor 2 - RamdomIntParams
    ParamList paramsRamdomInt2; 
    paramsRamdomInt2.param_count = 2;
    paramsRamdomInt2.params = (Param*) malloc(sizeof(Param) * paramsRamdomInt2.param_count);

    Param min2, max2; 
    min2.key = "min";
    min2.type = PARAM_INT;
    min2.value.i = 1;

    max2.key = "max";
    max2.type = PARAM_INT;
    max2.value.i = 300;

    paramsRamdomInt2.params[0] = min2;
    paramsRamdomInt2.params[1] = max2;

    // -------------------------------------------------------------------------------------

    ColumnDescriptor column_desc1;
    column_desc1.generator = random_int_gen_next;
    column_desc1.generator_params = &paramsRamdomInt1;

    ColumnDescriptor column_desc2;
    column_desc2.generator = random_int_gen_next;
    column_desc2.generator_params = &paramsRamdomInt2;


    ColumnRuntimeState columnTb1;
    columnTb1.column_name = "col tb 1";
    columnTb1.descriptor = &column_desc1;

    ColumnRuntimeState columnTb2;
    columnTb2.column_name = "col tb 2";
    columnTb2.descriptor = &column_desc2;

    ColumnRuntimeState columns1[5] = {columnTb1, columnTb1, columnTb1, columnTb1, columnTb1};
    ColumnRuntimeState columns2[10] = {columnTb2, columnTb2, columnTb2, columnTb2, columnTb2, columnTb2, columnTb2, columnTb2, columnTb2, columnTb2};

    TableDescriptor table1;
    table1.table_name = "Example 1";
    table1.columns_count = sizeof(columns1)/sizeof(columnTb1);
    table1.columns = columns1;
    table1.num_records = 10; 
    
    TableDescriptor table2;
    table2.table_name = "Example 2";
    table2.columns_count = sizeof(columns2)/sizeof(columnTb2);
    table2.columns = columns2;
    table2.num_records = 10; 

    TableDescriptor tables[] = {table1, table2};

    // contexto
    Context context; 
    context.random = random;
    context.tables = tables;
    context.tables_count = 2;


    run_thread_source(&genfile, &context);


    return 0;
}