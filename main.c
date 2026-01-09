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
    RandomIntParams randomIntParams1;
    randomIntParams1.max = 200;
    randomIntParams1.min = 100;
    
    RandomIntParams randomIntParams2;
    randomIntParams2.max = 9999;
    randomIntParams2.min = 100;
    
    ColumnDescriptor column_desc1;
    column_desc1.generator = random_int_gen_next;
    column_desc1.generator_params = &randomIntParams1;

    ColumnDescriptor column_desc2;
    column_desc2.generator = random_int_gen_next;
    column_desc2.generator_params = &randomIntParams2;


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