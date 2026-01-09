#include <stdio.h>
#include <stdlib.h>

void error_print(const char* module, const char* message) {
    printf("\x1b[31m[ERRO] [%s] %s\x1b[0m\n", module, message);
}

void error_print_exit(const char* module, const char* message) {
    printf("\x1b[31m[ERRO] [%s] %s\x1b[0m\n", module, message);
    exit(1);
}
