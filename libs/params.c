#include <stdio.h>
#include <string.h>
#include "params.h"
#include "../main.h"

Param search_param_by_key(ParamList* param_list, const char* key) {
    for (size_t i = 0; i < param_list->param_count; i++) {
        if (strcmp(param_list->params[i].key, key) == 0) {
            return param_list->params[i];
        }
    }
    Param null_param = {NULL, PARAM_NULL, {0}};
    return null_param;
}


int main() {
    /*
    
    parametros podem ser passados sempre com uma forma genérica

    Param param [
        {
            "name": "min",
            "type": "TYPE_INT",
            "value": 100
        },
        {
            "name": "max",
            "type": "TYPE_INT",
            "value": 200
        }
    ]
    
    */

    Param param_min;
    param_min.key = "min";
    param_min.type = PARAM_INT;
    param_min.value.i = 100;

    Param param_max;
    param_max.key = "max";
    param_max.type = PARAM_INT;
    param_max.value.i = 200;
   
    ParamList param_list;
    Param params[] = {param_min, param_max};
    param_list.params = params;
    param_list.param_count = sizeof(params)/sizeof(param_min);  

    // Assim, qualquer função de generator pode receber seus parametros de forma genérica
        
    Param min_param = search_param_by_key(&param_list, "min");
    Param max_param = search_param_by_key(&param_list, "max");
    Param null_param = search_param_by_key(&param_list, "null_key");
    if (null_param.type == PARAM_NULL) {
        printf("param 'null_key' not found\n");
    }
    
    printf("min: %d\n", min_param.value.i);
    printf("max: %d\n", max_param.value.i);

    return 0;

}