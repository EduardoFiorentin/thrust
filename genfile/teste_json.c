#include <json-c/json.h>
#include <stdlib.h>
#include <stdio.h>


#define JSON_FILE                               "teste.json"
#define JSON_KEY_GLOBAL_CONFIGS                 "config"
#define JSON_KEY_GLOBAL_CONFIGS_TARGET_NAME     "target"
#define JSON_KEY_TABLES                         "tables"
#define JSON_KEY_NUMBER_RECORDS                 "num_records"
#define JSON_KEY_TABLE_COLUMNS                  "columns"


int main() {

    struct json_object* root = json_object_from_file(JSON_FILE);

    // acesso às propriedades
    struct json_object *obj_config, *obj_tables;

    
    if (!json_object_object_get_ex(root, JSON_KEY_GLOBAL_CONFIGS, &obj_config)) {
        printf("Config não pode ser lido\n");
    }

    if (!json_object_object_get_ex(root, JSON_KEY_TABLES, &obj_tables)) {
        printf("Tables não pode ser lido\n");
    }

    const char* target = json_object_get_string(json_object_object_get(obj_config, JSON_KEY_GLOBAL_CONFIGS_TARGET_NAME));
    printf("%s\n", target);


    json_object_object_foreach(obj_tables, key, value_tables) {
        printf("Tabela: %s\n", key);
        
        struct json_object *config_root;
        if (!json_object_object_get_ex(obj_tables, key, &config_root)) {
            printf("%s não pode ser lido\n", key);
        }

        int num_regs;
        struct json_object *columns; 
        
        num_regs = json_object_get_int(json_object_object_get(config_root, JSON_KEY_NUMBER_RECORDS));
        
        if (!json_object_object_get_ex(config_root, JSON_KEY_TABLE_COLUMNS, &columns)) {
            printf("%s.%s não pode ser lido\n", key, JSON_KEY_TABLE_COLUMNS);
        }


        printf("\tNum registros: %d\n", num_regs);
        json_object_object_foreach(columns, key_column, value_column) {
            printf("\tcoluna: %s\n", key_column);

            
            struct json_object *column_details;
            if (!json_object_object_get_ex(columns, key_column, &column_details)) {
                printf("Detail não encontrado\n");
            }

            json_object_object_foreach(column_details, key_col_param, value_col_param) {
                printf("\t\t%s: ",key_col_param);
                if (json_object_get_type(value_col_param) == json_type_int) {
                    printf("%d\n", json_object_get_int(value_col_param));
                }
                else if (json_object_get_type(value_col_param) == json_type_string) {
                    printf("%s\n", json_object_get_string(value_col_param));
                }
                else printf("tipo não descrito\n");
            }

            
            json_object_put(column_details);
        }
        json_object_put(config_root);
        json_object_put(columns);
    }

    json_object_put(obj_config);
    json_object_put(obj_tables);
    json_object_put(root);

    return 0;
}