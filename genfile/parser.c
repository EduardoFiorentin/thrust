/* 

como os formatos de descrição podem ser variados - 
deixar o módulo cuidar da forma como será implementado internamente 
(quais os métodos, forma de chamada, etc)

O QUE SERÁ PADRONIZADO: 
Assinatura do método de chamada



// leitura do arquivo 
// parsing para GRN
*/
#include <json-c/json.h>
#include "../main.h"
#include <stdio.h>
#include <stdlib.h>
// #include "../utils/printer.h"

#define MODULE_NAME "GENFILE PARSER"

typedef struct tableListParamGRN TableListParamGRN;
typedef struct columnListParamGRN ColumnListParamGRN;
typedef struct grn GRN;

struct TableListParamGRN {
    const char*             key;
    size_t                  num_records;
    ColumnListParamGRN*     columns;
    size_t                  num_columns;   
};

struct columnListParamGRN {
    const char*     key;
    Param*          params;
    size_t          num_params;
};

struct grn {
    ParamList*              configs; 
    TableListParamGRN*      params;
};

GRN genfile_parser_execute(const char* file_name);
struct json_object* genfile_read_json(const char* file_name);

// TODO Remove -------------------------------------------

void aux_print_obj(struct json_object* obj) {
    printf("%s\n", json_object_to_json_string(obj));
}

void error_print(const char* module, const char* message) {
    printf("\x1b[31m[ERRO] [%s] %s\x1b[0m\n", module, message);
}

void error_print_exit(const char* module, const char* message) {
    printf("\x1b[31m[ERRO] [%s] %s\x1b[0m\n", module, message);
    exit(1);
}
// --------------------------------------------------------------------



// C FILE 
// TODO pensar em um nome melhor
GRN genfile_parser_execute(const char* file_name) {
    
    GRN structure;
    
    struct json_object *genfile_root, *configs_root, *tables_root;


    genfile_root = genfile_read_json(file_name);
    if (genfile_root == NULL) {
        error_print_exit(MODULE_NAME, "Genfile read failed!");
    }

    aux_print_obj(genfile_root);
}

struct json_object* genfile_read_json(const char* file_name) {
    return json_object_from_file(file_name);
}


int main() {

    GRN genfile_grn = genfile_parser_execute("genile.json");

    return 0;
}