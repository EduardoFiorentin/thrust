#ifndef SOURCE_H
#define SOURCE_H

#include <stdint.h>
#include "../main.h"

// Context structures

typedef enum {
    CTX_INITIALIZED,
    CTX_NOT_INITIALIZED
} Ctx_init;

typedef struct {
    Ctx_init    initialized;   // says if the other context fields where initialized
    uint32_t    state;         // random seed
} RandomServiceContext;

typedef struct {
    RandomServiceContext random;
} Context;

// define o tipo de retorno de uma função 
//  Qualquer função que 
// receba os 3 tipos e retorne um FieldValue
// é considerada um GeneratorFn

// GeneratorFn pode ser usado para criar variáveis 
// que guardam qualquer função que "implemente esta 
// interface"
typedef struct {
    const char* name;
    const char* generator;
    void*       params;     // parametros específicos de cada generator (passados como struct)
} FieldSpec;

typedef FieldValue (*GeneratorFn)(
    Context* context,
    const FieldSpec* field_spec,
    const Record* partial_record
);


int run_thread_source(Genfile* genfile, Context* context);
void source_print(const char* message);

#endif