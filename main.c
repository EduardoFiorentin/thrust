// #include "postgres.h"
#include "source/source.h"
// host=localhost port=5432 dbname=mydb connect_timeout=10
// password=postgres user=postgres dbname=postgres

// void exit_gracefully(PGconn *conn) {
//     PQfinish(conn);
//     exit(1);
// }


int main() 
{   
    // PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres connect_timeout=10");
    // PGresult *res;

    // if (PQstatus(conn) == CONNECTION_BAD) {
    //     fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
    //     PQfinish(conn);
    //     exit(1);
    // }

    // printf("Database connected!\n");
    // printf("User: %s\n", PQuser(conn));
    // printf("Database name: %s\n", PQdb(conn));
    // printf("Password: %s\n", PQpass(conn));

    // PGconn* conn = create_connection();

    // res = PQexec(conn, "select * from Habilitacoes;");

    // // Verificar o status do resultado da query
    // if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    //     fprintf(stderr, "Falha na query SELECT: %s\n", PQerrorMessage(conn));
    //     PQclear(res);
    //     exit_gracefully(conn);
    // }

    // // Processar e imprimir os resultados
    // int linhas = PQntuples(res);
    // int colunas = PQnfields(res);

    // printf("Foram retornadas %d linhas.\n", linhas);

    // for (int i = 0; i < linhas; i++) {
    //     for (int j = 0; j < colunas; j++) {
    //         printf("%s: %s\t", PQfname(res, j), PQgetvalue(res, i, j));
    //     }
    //     printf("\n");
    // }

    // // Liberar a memória do resultado da query
    // PQclear(res);

    // create_connection();

    // Fechar a conexão
    // PQfinish(conn);

    // printf("Conexao fechada.\n");

    Genfile genfile;

    RandomServiceContext random;

    // 
    Context context; 

    context.random = random;

    run_thread_source(&genfile, &context);


    return 0;
}