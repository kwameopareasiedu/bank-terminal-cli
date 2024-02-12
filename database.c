#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "types.h"

static PGconn *conn;

void parse_conn_line(db_conn_data_t *db_data, const char *line);

bool db_connect() {
    FILE *file = fopen("./conf", "r");

    if (file == NULL) {
        fprintf(stderr, "Missing db conf file\n");
        exit(-1);
    }

    db_conn_data_t conn_data;
    char line[512];

    while (fgets(line, sizeof(line), file)) {
        parse_conn_line(&conn_data, line);
    }

    fclose(file);

    char conn_str[512];
    sprintf(conn_str, "dbname=%s user=%s password=%s host=%s port=%s",
            conn_data.name, conn_data.user, conn_data.pass, conn_data.host, conn_data.port);
    conn = PQconnectdb(conn_str);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Error connecting to db: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return false;
    }

    printf("Db connected!\n");
    return true;
}

PGconn *db_get_connection() {
    return conn;
}

void db_disconnect() {
    if (conn != NULL) {
        PQfinish(conn);
        conn = NULL;
        printf("Db disconnected!\n");
    }
}

void parse_conn_line(db_conn_data_t *db_data, const char *line) {
    int _size = 128;
    char key[_size];
    char val[_size];
    int before_equal = 1;
    int idx = 0;
    int k_idx = 0;
    int v_idx = 0;
    char c;

    while ((c = line[idx++]) != '\n') {
        if (c == '=') {
            before_equal = 0;
        } else if (before_equal) {
            key[k_idx++] = c;
        } else {
            val[v_idx++] = c;
        }
    }

    key[k_idx] = '\0';
    val[v_idx] = '\0';

    if (strcmp(key, DB_NAME_KEY) == 0) strcpy(db_data->name, val);
    else if (strcmp(key, DB_USER_KEY) == 0) strcpy(db_data->user, val);
    else if (strcmp(key, DB_PASS_KEY) == 0) strcpy(db_data->pass, val);
    else if (strcmp(key, DB_HOST_KEY) == 0) strcpy(db_data->host, val);
    else if (strcmp(key, DB_PORT_KEY) == 0) strcpy(db_data->port, val);
}