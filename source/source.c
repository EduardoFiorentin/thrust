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
    // error_print_exit("source", "Memory allocation failed.");
    
    size_t num_tables = context->tables_count;
    int idx_table = 0;
    for (; idx_table < num_tables; idx_table++) {

        // loop: numero de registros que devem ser gerados
        size_t idx_record = 0;
        for (; idx_record < context->tables[idx_table].num_records; idx_record++) {
            // gera registro vazio
            Record* new_record = (Record*) malloc(sizeof(Record));
            if (!new_record)
                error_print_exit("source", "Memory allocation failed - Record");

            // para cada coluna 
            size_t num_columns = context->tables[idx_table].columns_count;
            new_record->field_count = num_columns;
            
            new_record->fields = (Field*) malloc(sizeof(Field) * num_columns);
            if (!new_record->fields) {
                error_print_exit("source", "Memory allocation failed - fields");
            }

            size_t idx_column = 0;
            for (; idx_column < num_columns; idx_column++) {
                // aciona gerador correto 
                // generator correto já é colocado no contexto em sua montagem lá no inicio.
        
                // TODO trocar a copia dos valores para acesso por endereço
                // Field* fd = (Field*) malloc(sizeof(Field) * num_columns);
                Field fd;
                // if (!fd) {
                //     error_print_exit("source", "Memory allocation failed - Field");
                // }
                
                    
                    
                FieldValue fv;
                // if (!fv) {
                //     error_print_exit("source", "Memory allocation failed - FieldValue.");
                // }
                
                // gera valor
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

            // libera memória
            // for (int i = 0; i < new_record->field_count; i++) {
            //     free(&new_record->fields[i].value);
            //     free(&new_record->fields[i]);
            // }
            free(new_record->fields);
            free(new_record);

        }    
    }
    
    
    
    
    
    
    // while not finished
        // generate new register 
        // send to shared buffer 
    // FieldSpec fs;
    // RandomIntParams rip; 
    // Record rec;
    

    // rip.max = 1000;
    // rip.min = 30;

    // fs.generator = "";
    // fs.name = "Campo teste";        // talvez nem precise ser guardado
    // fs.params = &rip;  // verificar se passar assim como endereço mantém os valores válidos
    
    // for (int i = 0; i < 5; i++) {
    //     FieldValue v = random_int_gen_next(context, &fs, &rec);
    //     printf("Generated: %d\n", v.value.i);
    // }
    
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
