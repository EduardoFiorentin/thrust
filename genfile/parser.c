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
    GRN* structure = calloc(1, sizeof(GRN));
    
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
        structure->tables[idx_table].num_columns = num_columns; 
        structure->tables[idx_table].columns = malloc(sizeof(ColumnGRN) * num_columns);

        read_columns_from_columns_root(structure, columns_root, idx_table);

        idx_table++;
    }
}

void read_columns_from_columns_root(GRN* structure, json_object *columns_root, int idx_table) {
    int idx_column = 0;
    json_object_object_foreach(columns_root, key_column, value_column) {
        printf("\tColuna: %s\n", key_column);
        
        // TODO remember to free strdup
        structure->tables[idx_table].columns[idx_column].key = strdup(key_column);
        
        // params
        int num_params = json_object_object_length(value_column);
        structure->tables[idx_table].columns[idx_column].num_params = num_params;
        structure->tables[idx_table].columns[idx_column].params = malloc(sizeof(Param) * num_params);

        read_values_from_column_root(structure, value_column, idx_table, idx_column);

        idx_column++;
    }
}

void read_values_from_column_root(GRN* structure, json_object *column_details_root, int idx_table, int idx_column) {
    int idx_value = 0;
    json_object_object_foreach(column_details_root, key_col_param, value_col_param) {
        Param new_param;
        new_param.key = strdup(key_col_param);

        printf("\t\t%s: ",key_col_param);
        if (json_object_get_type(value_col_param) == json_type_int) {
            int value = json_object_get_int(value_col_param);
            new_param.type = PARAM_INT;
            new_param.value.i = value;

            printf("%d\n", value);
        }
        else if (json_object_get_type(value_col_param) == json_type_string) {
            printf("%s\n", json_object_get_string(value_col_param));
            const char* value = json_object_get_string(value_col_param);
            new_param.type = PARAM_STRING;
            new_param.value.s = strdup(value);
        }

        else if (json_object_get_type(value_col_param) == json_type_double) {
            double value = json_object_get_double(value_col_param);
            new_param.type = PARAM_DOUBLE;
            new_param.value.d = value;

            printf("%f\n", value);
        }
        else if (json_object_get_type(value_col_param) == json_type_boolean) {
            int value = json_object_get_boolean(value_col_param);
            new_param.type = PARAM_BOOL;
            new_param.value.b = value;

            printf("%d\n", value);
        }
        else printf("tipo não descrito\n");

        structure->tables[idx_table].columns[idx_column].params[idx_value] = new_param;

        idx_value++;
    }
}


struct json_object* genfile_read_json(const char* file_name) {
    return json_object_from_file(file_name);
}


int main() {

    GRN* genfile_grn = genfile_parser_execute("genfile.json");

    // read target info

    printf("\n\nTeste de valores gerados |-----------------------------------\n");
    for (int i = 0; i < genfile_grn->num_tables; i++) {
        printf("\t%ld\n", genfile_grn->tables[i].num_records);
    }

    printf("\n\nTeste de impressão das informações dos objetos\n");
    printf("\nTarget info |------------------------------------------------\n");
    printf("\tKey: %s\n",   genfile_grn->configs[0].key);
    printf("\tType: %d\n",  genfile_grn->configs[0].type);
    printf("\tValue: %s\n", genfile_grn->configs[0].value.s);
    printf("\nbuffer info |------------------------------------------------\n");
    printf("\tKey: %s\n",   genfile_grn->configs[1].key);
    printf("\tType: %d\n",  genfile_grn->configs[1].type);
    printf("\tValue: %d\n", genfile_grn->configs[1].value.i);

    printf("\nInformações das tabelas |-------------------------------------\n");

    for (int idx_table = 0; idx_table < genfile_grn->num_tables; idx_table++) {
        printf("Table %d - %s\n", idx_table, genfile_grn->tables[idx_table].key);
        for (
            int idx_col = 0; 
            idx_col < genfile_grn->tables[idx_table].num_columns; 
            idx_col++
        ) {
            printf("\tColumn: %d - %s\n", idx_col, genfile_grn->tables[idx_table].columns[idx_col].key);
            for (
                int idx_param = 0;
                idx_param < genfile_grn->tables[idx_table].columns[idx_col].num_params;
                idx_param++
            ) {
                printf("\t\tField: %d - %s: ", idx_param, genfile_grn->tables[idx_table].columns[idx_col].params[idx_param].key);
                if (genfile_grn->tables[idx_table].columns[idx_col].params[idx_param].type == PARAM_INT) {
                    printf("%d\n", genfile_grn->tables[idx_table].columns[idx_col].params[idx_param].value.i);
                }
                else if (genfile_grn->tables[idx_table].columns[idx_col].params[idx_param].type == PARAM_STRING) {
                    printf("%s\n", genfile_grn->tables[idx_table].columns[idx_col].params[idx_param].value.s);
                }
                else if (genfile_grn->tables[idx_table].columns[idx_col].params[idx_param].type == PARAM_BOOL) {
                    printf("%d\n", genfile_grn->tables[idx_table].columns[idx_col].params[idx_param].value.b);
                }

            }   
        }
    }

    return 0;
}

/*

Idea to free memory - test better 
void free_grn(GRN* g) {
    for (size_t i = 0; i < g->num_configs; i++) {
        free(g->configs[i].key);
        if (g->configs[i].type == PARAM_STRING)
            free(g->configs[i].value.s);
    }
    free(g->configs);

    for (size_t t = 0; t < g->num_tables; t++) {
        free(g->tables[t].key);
        for (size_t c = 0; c < g->tables[t].num_columns; c++) {
            free(g->tables[t].columns[c].key);
            // liberar params aqui depois
        }
        free(g->tables[t].columns);
    }
    free(g->tables);
    free(g);
}


*/