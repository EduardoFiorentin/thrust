#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>

// Genfile structure
typedef struct {
    // config do genfile futuramente
} Genfile;

 
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
    const char*     name;
    FieldValue      value;
} Field;

typedef struct {
    Field*          fields;
    size_t          field_count;
} Record;

#endif