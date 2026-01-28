#include <json-c/json.h>
#include "../main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODULE_NAME "GENFILE PARSER"
#define JSON_KEY_GLOBAL_CONFIGS                 "config"
#define JSON_KEY_TABLES                         "tables"
#define JSON_KEY_NUMBER_RECORDS                 "num_records"
#define JSON_KEY_TABLE_COLUMNS                  "columns"

// global properties
#define JSON_KEY_GLOBAL_CONFIGS_TARGET_NAME     "target"
#define JSON_KEY_GLOBAL_CONFIGS_BUFFER_SIZE     "buffer_size"


typedef struct tableGRN TableGRN;
typedef struct columnGRN ColumnGRN;
typedef struct grn GRN;

struct tableGRN {
    char*             key;
    size_t                  num_records;
    ColumnGRN*              columns;
    size_t                  num_columns;   
};

struct columnGRN {
    char*     key;
    Param*          params;
    size_t          num_params;
};

struct grn {
    Param*                  configs;
    size_t                  num_configs; 
    TableGRN*               tables;
    size_t                  num_tables;
};

/*
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
*/


GRN* genfile_parser_execute(const char* file_name);
struct json_object* genfile_read_json(const char* file_name);
void read_columns_from_columns_root(GRN* structure, json_object *columns_root, int idx_table);
void read_tables_from_table_root(GRN* structure, json_object *obj_tables, char* buffer_string);
void read_values_from_column_root(GRN* structure, json_object *column_details_root, int idx_table, int idx_column);
void read_global_properties_from_json(GRN* structure, struct json_object* obj_config, char* buffer_string);

// TODO Remove aux methods -------------------------------------------

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
GRN* genfile_parser_execute(const char* file_name) {
    
    printf("________________| Genfile Description - JSON parser |________________\n");
    printf("_____________________________________________________________________\n");
    printf("\n Global configs: \n");
    GRN* structure = malloc(sizeof(GRN));
    
    char buffer_string[100];
    
    struct json_object* root = json_object_from_file(file_name);

    // acess main doc properties
    struct json_object *obj_config, *obj_tables;

    // load root pointers
    if (!json_object_object_get_ex(root, JSON_KEY_GLOBAL_CONFIGS, &obj_config)) {
        sprintf(buffer_string, "'%s' property is required in the genfile.", JSON_KEY_GLOBAL_CONFIGS);
        error_print_exit(MODULE_NAME, buffer_string);
    }

    if (!json_object_object_get_ex(root, JSON_KEY_TABLES, &obj_tables)) {
        sprintf(buffer_string, "'%s' property is required in the genfile.", JSON_KEY_TABLES);
        error_print_exit(MODULE_NAME, buffer_string);
    }

    read_global_properties_from_json(structure, obj_config, buffer_string);
    
    printf("\nTables description: \n");
    printf("Numero de tabelas: %ld\nNumero de configurações globais: %ld\n", structure->num_tables, structure->num_configs);
    
    read_tables_from_table_root(structure, obj_tables, buffer_string);
    
    json_object_put(obj_config);
    json_object_put(obj_tables);
    json_object_put(root); 

    return structure;

}


void read_global_properties_from_json(GRN* structure, struct json_object* obj_config, char* buffer_string) {
    
    int num_global_configs = json_object_object_length(obj_config);

    structure->configs = (Param*) malloc(sizeof(Param) * num_global_configs);
    structure->num_configs = num_global_configs;
    
    // reading config.target
    const char* target = json_object_get_string(json_object_object_get(obj_config, JSON_KEY_GLOBAL_CONFIGS_TARGET_NAME));
    if (target == NULL) {
        sprintf(buffer_string, "'%s.%s' property is required in the genfile.", JSON_KEY_GLOBAL_CONFIGS, JSON_KEY_GLOBAL_CONFIGS_TARGET_NAME);
        error_print_exit(MODULE_NAME, buffer_string);
    }

    // add target config to structure
    structure->configs[0].key = strdup(JSON_KEY_GLOBAL_CONFIGS_TARGET_NAME);
    structure->configs[0].value.s = strdup(target);
    structure->configs[0].type = PARAM_STRING;

    // reading config.buffer_size
    struct json_object *buf_obj;
    if (!json_object_object_get_ex(obj_config, JSON_KEY_GLOBAL_CONFIGS_BUFFER_SIZE, &buf_obj) ||
        json_object_get_type(buf_obj) != json_type_int) {
        sprintf(buffer_string, "'%s.%s' must be an integer.",
                JSON_KEY_GLOBAL_CONFIGS,
                JSON_KEY_GLOBAL_CONFIGS_BUFFER_SIZE);
        error_print_exit(MODULE_NAME, buffer_string);
    }

    int buffer_size = json_object_get_int(buf_obj);

    if (!buffer_size) {
        sprintf(buffer_string, "'%s.%s' must be different from zero.", JSON_KEY_GLOBAL_CONFIGS, JSON_KEY_GLOBAL_CONFIGS_BUFFER_SIZE);
        error_print_exit(MODULE_NAME, buffer_string);
    }

    // add buffer_size config to structure
    structure->configs[1].key = strdup(JSON_KEY_GLOBAL_CONFIGS_BUFFER_SIZE);
    structure->configs[1].value.i = buffer_size;
    structure->configs[1].type = PARAM_INT;

    printf("Target name: %s\n", target);
    printf("Buffer size: %d\n", buffer_size);
}


void read_tables_from_table_root(GRN* structure, json_object *obj_tables, char* buffer_string) {
    // read essencials from tables
    int num_tables = json_object_object_length(obj_tables);
    structure->num_tables = num_tables;
    
    // aloc memory to tables
    structure->tables = (TableGRN*) malloc(sizeof(TableGRN) * num_tables);

    int idx_table = 0;
    json_object_object_foreach(obj_tables, key_tables, value_tables) {
        printf("Tabela: %s\n", key_tables);
        
        // getting tables list root
        struct json_object *tables_root;
        if (!json_object_object_get_ex(obj_tables, key_tables, &tables_root)) {
            sprintf(buffer_string, "'%s' table could not be found.", key_tables);
            error_print_exit(MODULE_NAME, buffer_string);
        }

        // aloc table key (name)
        structure->tables[idx_table].key = strdup(key_tables);

        int num_regs_tobe_gen;
        struct json_object *columns_root;

        // getting number of registers to be generated
        num_regs_tobe_gen = json_object_get_int(json_object_object_get(tables_root, JSON_KEY_NUMBER_RECORDS));
        structure->tables[idx_table].num_records = num_regs_tobe_gen;
        printf("\tNum registros: %d\n", num_regs_tobe_gen);

        // getting collumns list of the table 
        if (!json_object_object_get_ex(tables_root, JSON_KEY_TABLE_COLUMNS, &columns_root)) {
            sprintf(buffer_string, "'%s' property could not be found in '%s' table description.", JSON_KEY_TABLE_COLUMNS, key_tables);
            error_print_exit(MODULE_NAME, buffer_string);
        }

        // read number of columns 
        int num_columns = json_object_object_length(columns_root);
        printf("\tNum columns: %d\n", num_columns);

        // aloc space to columns for each table
        structure->tables[idx_table].columns = malloc(sizeof(ColumnGRN) * num_columns);
        // printf("Teste: %d %d", sizeof(ColumnGRN), sizeof(structure->tables[idx_table].columns));

        read_columns_from_columns_root(structure, columns_root, idx_table);

        json_object_put(tables_root);
        json_object_put(columns_root);
        idx_table++;
    }
}

void read_columns_from_columns_root(GRN* structure, json_object *columns_root, int idx_table) {
    int idx_column = 0;
    json_object_object_foreach(columns_root, key_column, value_column) {
        printf("\tColuna: %s\n", key_column);

        struct json_object *column_details;
        if (!json_object_object_get_ex(columns_root, key_column, &column_details)) {
            printf("Detail não encontrado\n");
        }

        // TODO remember to free strdup
        structure->tables[idx_table].columns[idx_column].key = strdup(key_column);
        structure->tables[idx_table].columns[idx_column].num_params = json_object_object_length(value_column);
        printf("Numero de parametros - testar: %s %ld\n", key_column, structure->tables[idx_table].columns[idx_column].num_params);

        read_values_from_column_root(structure, column_details, idx_table, idx_column);        

        
        json_object_put(column_details);
        idx_column++;
    }
}

void read_values_from_column_root(GRN* structure, json_object *column_details_root, int idx_table, int idx_column) {
    int idx_value = 0;
    json_object_object_foreach(column_details_root, key_col_param, value_col_param) {
        printf("\t\t%s: ",key_col_param);
        if (json_object_get_type(value_col_param) == json_type_int) {
            printf("%d\n", json_object_get_int(value_col_param));
        }
        else if (json_object_get_type(value_col_param) == json_type_string) {
            printf("%s\n", json_object_get_string(value_col_param));
            // pegar o tamanho da string 
            // alocar memoria para este tamanho
            // copiar o valor para a variável


        }
        else printf("tipo não descrito\n");
        idx_value++;
    }
}

struct json_object* genfile_read_json(const char* file_name) {
    return json_object_from_file(file_name);
}


int main() {

    GRN* genfile_grn = genfile_parser_execute("genfile.json");

    // read target info
    printf("\n\nTarget info |------------------------------------------------\n");
    printf("\tKey: %s\n",   genfile_grn->configs[0].key);
    printf("\tType: %d\n",  genfile_grn->configs[0].type);
    printf("\tValue: %s\n", genfile_grn->configs[0].value.s);

    printf("\n\nTeste de valores gerados |-----------------------------------\n");
    for (int i = 0; i < genfile_grn->num_tables; i++) {
        printf("\t%ld\n", genfile_grn->tables[i].num_records);
    }

    return 0;
}