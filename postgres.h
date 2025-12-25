#ifndef POSTGRES_H
#define POSTGRES_H

#include <libpq-fe.h>

PGconn* create_connection(void);

#endif  