#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>

 
typedef enum {
    FIELD_INT,
    FIELD_FLOAT,
    FIELD_STRING,
    FIELD_BOOL,
    FIELD_NULL
} FieldType;

typedef struct {
    FieldType type;
    union {
        int         i;
        double      f;
        char*       c;
        int         b;    
    } value;

} FieldValue;

typedef struct {
    const char*     table_name;
    const char*     column_name;
    FieldValue      value;
} Field;

typedef struct {
    Field*          fields;
    size_t          field_count;
} Record;



/* Generic parameters system structure */
typedef enum {
    PARAM_INT,
    PARAM_STRING,
    PARAM_FLOAT,
    PARAM_BOOL,
    PARAM_NULL
} ParamType;

typedef struct param {
    char *key;
    ParamType type;
    union {
        int i;
        float f;
        char *s;
        int b;
    } value;
} Param;

typedef struct parmList {
    Param *params;
    size_t param_count;
} ParamList;


/* Structures of generator description system */
typedef struct generatorDesc {
    char *type;              // "random_int", "serial_gen", etc
    Param *params;
    size_t param_count;
} GeneratorDesc;

typedef struct columnDesc {
    char *name;
    GeneratorDesc generator;
} ColumnDesc;

typedef struct tableDesc {
    char *name;
    ColumnDesc *columns;
    size_t column_count;
} TableDesc;

// structure for global / general configurations
typedef struct globalConfigs {
    char *target_key;
    // add another global configurations here
} GlobalConfigs;

// General structure for generation description
// Used to describe all generation parameters from specific parser to internal system
typedef struct genfile {
    GlobalConfigs config;
    TableDesc *tables;
    size_t table_count;
} Genfile;

Param* search_param_by_key(ParamList* params, const char* key);

#endif