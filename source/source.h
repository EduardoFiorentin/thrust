#ifndef SOURCE_H
#define SOURCE_H

#include <stdint.h>
#include "../main.h"

// forward declaration
typedef struct Context Context;

// GeneratorFn prototype
typedef FieldValue (*GeneratorFn)(
    Context* context,
    uint32_t table_pos,
    uint32_t column_pos
);


// Context structures
typedef enum {
    CTX_INITIALIZED,
    CTX_NOT_INITIALIZED
} Ctx_init;


// Random generator context
typedef struct {
    Ctx_init initialized;
    uint32_t state;
} RandomIntServiceContext;


typedef struct {
    int min;
    int max;
} RandomIntParams;


// Column descriptor
typedef struct {
    GeneratorFn     generator;
    ParamList*      generator_params;
    // TODO Implementar um local_generator_context - responsável por guardar um contexto específico do gerador
} ColumnDescriptor;


// Runtime column state
typedef struct {
    const char*             column_name;
    const ColumnDescriptor* descriptor;
    void*                   generator_state;
} ColumnRuntimeState;



typedef struct {
    const char*         table_name;
    ColumnRuntimeState* columns;
    size_t              columns_count;
    size_t              num_records;
} TableDescriptor;


// Global context
struct Context {
    RandomIntServiceContext random;
    TableDescriptor*        tables;
    size_t                  tables_count;
};

int run_thread_source(Genfile* genfile, Context* context);
void source_print(const char* message);

#endif