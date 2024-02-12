#include "database.h"

int main() {
    if (!db_connect()) {
        fprintf(stderr, "Unable to connect to db");
        return -1;
    }

    PGconn *conn = db_get_connection();

    if (conn == NULL) {
        fprintf(stderr, "Db connection is null");
        return -1;
    }

    char *create_tables_sql = "CREATE TABLE IF NOT EXISTS users ("
                              "     id UUID PRIMARY KEY DEFAULT gen_random_uuid(),"
                              "     first_name VARCHAR (24) NOT NULL,"
                              "     last_name VARCHAR (24) NOT NULL,"
                              "     email VARCHAR (24) NOT NULL UNIQUE,"
                              "     balance DECIMAL(10,2) DEFAULT 0,"
                              "     password TEXT NOT NULL,"
                              "     created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                              "     updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
                              ");"
                              "CREATE TABLE IF NOT EXISTS records ("
                              "     id UUID PRIMARY KEY DEFAULT gen_random_uuid(),"
                              "     user_id UUID REFERENCES Users(id),"
                              "     amount DECIMAL(10,2) NOT NULL,"
                              "     created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                              "     updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
                              ");"
                              "CREATE FUNCTION fn_update_updated_at()"
                              "     RETURNS TRIGGER AS $$"
                              "     BEGIN"
                              "     NEW.updated_on = now();"
                              "     RETURN NEW;"
                              "     END;"
                              "$$ language 'plpgsql';"
                              "CREATE TRIGGER trigger_before_update_users"
                              "     BEFORE UPDATE"
                              "     ON users"
                              "     FOR EACH ROW"
                              "     EXECUTE PROCEDURE fn_update_updated_at();"
                              "CREATE TRIGGER trigger_before_update_records"
                              "     BEFORE UPDATE"
                              "     ON records"
                              "     FOR EACH ROW"
                              "     EXECUTE PROCEDURE fn_update_updated_at();";

    PGresult *create_tables_res = PQexec(conn, create_tables_sql);
    ExecStatusType create_tables_status = PQresultStatus(create_tables_res);

    if (create_tables_status != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error executing the query: %s\n", PQerrorMessage(conn));
        return -1;
    }

    printf("Db setup complete!");
    return 0;
}