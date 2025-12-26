#include <stdio.h>
#include "source.h"
#include "generators/random_int_gen.h"
#include "../main.h"

// internal function - used to print signed messages
void source_print(const char* message) {
    printf("[%s] %s\n", "source", message);
}

// Configuração inicial 
void init_source(Genfile* genfile, Context* context) {
    source_print("Source module starting...");

    // validate config file (MANDAR PARA OUTRO ARQUIVO/MODULO - source assume que é válido)
    // Start context
    // init_context(...) // Function from context.c

    // Start submodules
    // begin all necessary submodules (generators)
    // Call each submodule "load_context(context)" function
    random_service_init(context, 159);    // TODO implementar seed vinda do genfile
    
    // acho que cada submódulo poderia fica responsável por carregar suas próprias 
    // dependências no contexto 

    

}

// Função de execução 
void run_source(Genfile* genfile, Context* context) {

    source_print("Source module started and running.");
    // while not finished
        // generate new register 
        // send to shared buffer 
    FieldSpec fs;
    RandomIntParams rip; 
    Record rec;
    

    rip.max = 1000;
    rip.min = 30;

    fs.generator = "";
    fs.name = "Campo teste";        // talvez nem precise ser guardado
    fs.params = &rip;  // verificar se passar assim como endereço mantém os valores válidos
    
    for (int i = 0; i < 5; i++) {
        FieldValue v = random_int_gen(context, &fs, &rec);
        printf("Generated: %d\n", v.value.i);
    }
    
    source_print("Source module finished.");
}

// função de finalização - 
// finaliza submódulos, fecha conexões, etc.
void shutdown_source(Context* context) {
    source_print("Cleaning context info.");
    random_service_shutdown(context);
    // Call each submodule "clean_context" function
}


// Orchestrador do módulo
int run_thread_source(Genfile* genfile, Context* context) {
    init_source(genfile, context);
    run_source(genfile, context);
    shutdown_source(context);
}
