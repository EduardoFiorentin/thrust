#ifndef SOURCE_H
#define SOURCE_H

#include <stdint.h>
#include "../main.h"

// Context structures

typedef enum {
    CTX_INITIALIZED,
    CTX_NOT_INITIALIZED
} Ctx_init;


// Estruturas de geração de números aleatórios 

typedef struct {
    Ctx_init    initialized;   // says if the other context fields where initialized
    uint32_t    state;         // random seed
} RandomIntServiceContext;

typedef struct {
    int min;
    int max;
} RandomIntParams;




// descriptor used to columns rules description
typedef struct {
    const char*     generator_id;      // id do gerador utilizado para a coluna específica 
    void*           generator_params;     // parâmetros usados pelo gerador 
} ColumnDescriptor;

// struct used to save informatios to generators in runtime 
typedef struct {
    const char*                 column_name;           // "id"
    const ColumnDescriptor*     desc;
    void*                       generator_state;
} ColumnRuntimeState;

typedef struct {
    const char*                 table_name;
    ColumnRuntimeState*         columns;
    size_t                      columns_count;
} TableDescriptor;

// parte estática 
typedef struct {
    // Service contexts
    RandomIntServiceContext         random;

    // descriptors
    TableDescriptor*                tables;        // guarda a lista de colunas definidas
    size_t                          tables_count;  // tamanho da lista
} Context;

// define o tipo de retorno de uma função 
//  Qualquer função que 
// receba os 3 tipos e retorne um FieldValue
// é considerada um GeneratorFn

// GeneratorFn pode ser usado para criar variáveis 
// que guardam qualquer função que "implemente esta 
// interface"
// typedef struct {
//     const char* name;
//     const char* generator;
//     void*       params;     // parametros específicos de cada generator (passados como struct)
// } FieldSpec;

typedef FieldValue (*GeneratorFn)(
    Context* context,
    // const FieldSpec* field_spec,
    uint32_t table_pos,
    uint32_t column_pos,
    const Record* partial_record
);


int run_thread_source(Genfile* genfile, Context* context);
void source_print(const char* message);

#endif