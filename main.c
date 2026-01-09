#include "main.h"
#include "source/source.h"
#include "source/generators/random_int_gen.h"

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
    RandomIntParams randomIntParams;
    randomIntParams.max = 200;
    randomIntParams.min = 100;
    
    ColumnDescriptor column_desc;
    column_desc.generator = random_int_gen_next;
    column_desc.generator_params = &randomIntParams;


    ColumnRuntimeState column;
    column.column_name = "col";
    column.descriptor = &column_desc;

    
    TableDescriptor table;
    table.table_name = "Example";
    table.columns_count = 1;
    table.columns = &column;
    
    // contexto
    Context context; 
    context.random = random;
    context.tables = &table;
    context.tables_count = 1;


    run_thread_source(&genfile, &context);


    return 0;
}