#include "postgres.h"

#include <stdio.h>
#include <stdlib.h>
// #include <string.h>

#define HOST    "localhost"
#define PORT    "5432"
#define DBNAME  "postgres"
#define USER    "postgres"
#define PASS    "postgres"
#define TIMEOUT "10"

PGconn* create_connection(void) {
    // PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres connect_timeout=10");
    
    // PGresult *res;
    
    
    PGconn *conn = PQconnectdb(sprintf("host=%s port=%s dbname=%s user=%s password=%s connect_timeout=%s", HOST, PORT, DBNAME, USER, PASS, TIMEOUT));

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    printf("Database connected!\n");
    printf("User: %s\n", PQuser(conn));
    printf("Database name: %s\n", PQdb(conn));
    printf("Password: %s\n", PQpass(conn));

    return conn;
}
