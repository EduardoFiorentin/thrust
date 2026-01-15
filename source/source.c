#include <stdio.h>
#include "source.h"
#include "generators/random_int_gen.h"
#include "../main.h"
#include "../utils/printer.h"

// internal function - used to print signed messages
void source_print(const char* message) {
    printf("[%s] %s\n", "source", message);
}

// Configuração inicial 
void init_source(Genfile* genfile, Context* context) {
    source_print("Module starting...");
    
    source_print("Starting context...");
    // Start context
    // init_context(...) // Function from context.c

    // Start submodules
    // begin all necessary submodules (generators)
    // Call each service "load_context(context)" function
    random_int_gen_init(context, 159);    // TODO implementar seed vinda do genfile
    
}

// Função de execução 
void run_source(Genfile* genfile, Context* context) {

    source_print("Source module started and running.");
    
    // loop: for each table who needs registre generation
    size_t num_tables = context->tables_count;
    int idx_table = 0;
    for (; idx_table < num_tables; idx_table++) {

        // loop: numero de registros que devem ser gerados
        size_t idx_record = 0;
        for (; idx_record < context->tables[idx_table].num_records; idx_record++) {
            
            Record* new_record = (Record*) malloc(sizeof(Record));
            if (!new_record)
                error_print_exit("source", "Memory allocation failed - Record");

            // for each column in the table, call generator and populate fields 
            size_t num_columns = context->tables[idx_table].columns_count;
            new_record->field_count = num_columns;  // TODO trocar nome field_count para num_columns
            
            new_record->fields = (Field*) malloc(sizeof(Field) * num_columns);
            if (!new_record->fields) {
                error_print_exit("source", "Memory allocation failed - fields");
            }

            size_t idx_column = 0;
            for (; idx_column < num_columns; idx_column++) {
                // activate correct generator
                // correct generator is already in context, and just need to be called.
        
                Field fd;
                FieldValue fv;
                
                fv = context->tables[idx_table].columns[idx_column].descriptor->generator(context, idx_table, idx_column);
                
                // popula Field global 
                fd.value = fv;
                fd.column_name = context->tables[idx_table].columns[idx_column].column_name;
                fd.table_name = context->tables[idx_table].table_name;
                
                // adiciona o field ao array de Fields (Registro)
                new_record->fields[idx_column] = fd;

            }

            // imprime (futura adição ao buffer)            
            printf("%ld: ", idx_record);
            for (int i = 0; i < new_record->field_count; i++) {
                printf("%d\t |", new_record->fields[i].value.value.i);
            }
            printf("\n");

            free(new_record->fields);
            free(new_record);

        }    
    }
    
    source_print("Source module finished.");
}

// função de finalização - 
// finaliza submódulos, fecha conexões, etc.
void shutdown_source(Context* context) {
    source_print("Cleaning context info.");
    random_int_gen_shutdown(context);
    // Call each submodule "clean_context" function
}


// Orchestrador do módulo
int run_thread_source(Genfile* genfile, Context* context) {
    init_source(genfile, context);
    run_source(genfile, context);
    shutdown_source(context);
    return 1;
}
