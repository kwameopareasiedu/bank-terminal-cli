#include <libpq-fe.h>
#include <stdbool.h>

#ifndef DATABASE
#define DATABASE

bool db_connect();

PGconn* db_get_connection();

void db_disconnect();

#endif