#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
// #include "toml.h"
#include "tomlc17.h"

static void error(const char *msg, const char *msg1) {
  fprintf(stderr, "ERROR: %s%s\n", msg, msg1 ? msg1 : "");
  exit(1);
}

int main() {

    toml_result_t result = toml_parse_file_ex("teste.toml");

    // Check for parse error
    if (!result.ok) {
        error(result.errmsg, 0);
    }

     // Extract values
    toml_datum_t config_str = toml_seek(result.toptab, "config.config_str");

    printf("valor: %s\n", config_str.u.s);

    const char* tables_list = "tables.list";
    toml_datum_t tables = toml_seek(result.toptab, tables_list);
    if (tables.type != TOML_ARRAY) {
        error("tables is not an array", 0);
    }

    char columns_array_path[256];
    char value_buffer[256];

    for (int idx_table = 0; idx_table < tables.u.arr.size; idx_table++) {
        const char* table_name = tables.u.arr.elem[idx_table].u.s;

        // formar string com a rota de acesso à lista de colunas 
        // nome : <column name>.<column>
        snprintf(columns_array_path, sizeof(columns_array_path), "%s.%s", table_name, "columns");


        printf("table: %s\n", columns_array_path);

        toml_datum_t columns_list = toml_seek(result.toptab, columns_array_path);
        for (int idx_columns = 0; idx_columns < columns_list.u.arr.size; idx_columns++) {
            const char* column_name = columns_list.u.arr.elem[idx_columns].u.s;
            printf("\tColumn: %s\n", column_name);

            snprintf(value_buffer, sizeof(value_buffer), "%s.%s.generator_type", table_name, column_name);
            toml_datum_t column_value = toml_seek(result.toptab, value_buffer);
            printf("\t\tgenerator_type: %s\n", column_value.u.s);
        }
    }

    toml_free(result);

    return 0;
}


/*
  Find a key in a toml_table. Return the value of the key if found,
  or a TOML_UNKNOWN otherwise.
 
 TOML_EXTERN toml_datum_t toml_get(toml_datum_t table, const char *key);

 
 *
 * Locate a value starting from a toml_table. Return the value of the key if
 * found, or a TOML_UNKNOWN otherwise.
 *
 * Note: the multipart-key is separated by DOT, and must not have any escape
 * chars. The maximum length of the multipart_key must not exceed 127 bytes.
 *
TOML_EXTERN toml_datum_t toml_seek(toml_datum_t table,
                                   const char *multipart_key);



*/


// static void error(const char* msg, const char* msg1)
// {
//     fprintf(stderr, "ERROR: %s%s\n", msg, msg1?msg1:"");
//     exit(1);
// }


// int main() {

//     FILE* fp;
//     char errbuf[200];

//     // 1. Read and parse toml file
//     fp = fopen("teste.toml", "r");
//     if (!fp) {
//         error("cannot open sample.toml - ", strerror(errno));
//     }

//     toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
//     fclose(fp);

//     if (!conf) {
//         error("cannot parse - ", errbuf);
//     }

//     // // 2. Traverse to a table.
//     // toml_table_t* server = toml_table_in(conf, "server");
//     // if (!server) {
//     //     error("missing [server]", "");
//     // }

//     // // 3. Extract values
//     // toml_datum_t host = toml_string_in(server, "host");
//     // if (!host.ok) {
//     //     error("cannot read server.host", "");
//     // }

//     // toml_array_t* portarray = toml_array_in(server, "port");
//     // if (!portarray) {
//     //     error("cannot read server.port", "");
//     // }

//     // printf("host: %s\n", host.u.s);
//     // printf("port: ");
//     // for (int i = 0; ; i++) {
//     //     toml_datum_t port = toml_int_at(portarray, i);
//     //     if (!port.ok) break;
//     //     printf("%d ", (int)port.u.i);
//     // }
//     // printf("\n");

//     // leitura dos configs
//     toml_table_t* global_config = toml_table_in(conf, "config");
//     if (!global_config) {
//         error("[genfile] missing 'config' argument", "");
//     }

//     toml_datum_t config_string = toml_string_in(global_config, "config_str");
//     if (!config_string.ok) {
//         error("[genfile] missing 'config_str' argument", "");
//     }

//     printf("Valor encontrado: %s\n", config_string.u.s);


//     // leitura das tables 
//     toml_table_t* table_list = toml_table_in(conf, "tables");
//     for (int i = 0; ; i++) {
//         const char* key = toml_key_in(table_list, i);
//         if (!key) break;
//         printf("table %s\n", key);

//         // leitura das subtabelas - pegando nomes das tabelas
//         size_t prefix_len = strlen("tables.");
//         size_t key_len = strlen(key);
//         char *name = malloc(prefix_len + key_len + 1);
//         if (!name) {
//             error("[genfile] malloc failed", "");
//         }
//         snprintf(name, prefix_len + key_len + 1, "tables.%s", key);
//         printf("%s\n", name);

        
//         toml_table_t* subtable = toml_table_in(conf, name);
//         if (subtable) {
//             printf("subtable %s\n", name);
//             for (int j = 0; ; j++) {
//                 const char* colkey = toml_key_in(subtable, j);
//                 if (!colkey) break;
//                 printf("\tcolumn %s\n", colkey);
//             }
//         }
//         free(name);
//         // printf("foi\n");
//         // toml_table_t* subtable = toml_table_in(conf, strcat("tables.", key));
//         // toml_table_t* subtable = toml_table_in(conf, strcat("tables.", key));
//         // for (int j = 0; ; j++) {
//         //     const char* key = toml_key_in(subtable, j);
//         //     if (!key) break;
//         //     printf("\tcolumn %s\n", key);
        
//         // }
//     }

//     // toml_table_t * table = toml_table_in(table_list, "tables");

//     // printf("%d\n", toml_key_in(table_list));


//     // 4. Free memory
//     // free(host.u.s);
//     toml_free(conf);

    
//     return 0;
// }