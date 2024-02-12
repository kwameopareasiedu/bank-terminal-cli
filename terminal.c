#include "terminal.h"
#include "database.h"
#include "utils.h"
#include "libs/libbcrypt/include/bcrypt/bcrypt.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

static user_t *auth_user;

bool terminal_register(char *first_name, char *last_name, char *email, char *password) {
    char check_query[QUERY_BUF_SIZE];
    sprintf(check_query, "SELECT id FROM users WHERE email='%s' LIMIT 1;", email);

    PGconn *conn = db_get_connection();
    PGresult *check_res = PQexec(conn, check_query);
    ExecStatusType check_status = PQresultStatus(check_res);

    if (check_status != PGRES_TUPLES_OK) {
        fprintf(stderr, "Unable to check existing email: %s", PQerrorMessage(conn));
        return false;
    } else if (PQntuples(check_res) > 0) {
        fprintf(stderr, "Email already exists\n");
        return false;
    }

    char hash_salt[96];
    char hashed_pass[96];
    bcrypt_gensalt(10, hash_salt);
    bcrypt_hashpw(password, hash_salt, hashed_pass);

    char insert_query[QUERY_BUF_SIZE];
    sprintf(insert_query, "INSERT INTO users(first_name,last_name,email,password) VALUES('%s','%s','%s','%s');",
            first_name, last_name, email, hashed_pass);

    PGresult *insert_res = PQexec(conn, insert_query);
    ExecStatusType insert_status = PQresultStatus(insert_res);

    if (insert_status != PGRES_COMMAND_OK) {
        fprintf(stderr, "Unable to register user: %s", PQerrorMessage(conn));
        return false;
    }

    return terminal_authenticate(email, password);
}

bool terminal_authenticate(char *email, char *pass) {
    char query[QUERY_BUF_SIZE];
    sprintf(query, "SELECT * FROM users WHERE email='%s' LIMIT 1;", email);

    PGconn *conn = db_get_connection();
    PGresult *res = PQexec(conn, query);
    ExecStatusType status = PQresultStatus(res);

    if (status != PGRES_TUPLES_OK || PQntuples(res) == 0)return NULL;

    char *end_ptr;
    int rows = PQntuples(res);
    int cols = PQnfields(res);

    if (rows > 0) {
        auth_user = (user_t *) malloc(sizeof(user_t));

        for (int col = 0; col < cols; col++) {
            char *field = PQfname(res, col);
            char *value = PQgetvalue(res, 0, col);


            if (strcmp(field, "id") == 0) strcpy(auth_user->id, value);
            else if (strcmp(field, "first_name") == 0) strcpy(auth_user->first_name, value);
            else if (strcmp(field, "last_name") == 0) strcpy(auth_user->last_name, value);
            else if (strcmp(field, "email") == 0) strcpy(auth_user->email, value);
            else if (strcmp(field, "balance") == 0) auth_user->balance = strtod(value, &end_ptr);
            else if (strcmp(field, "password") == 0) strcpy(auth_user->password, value);
        }

        if (bcrypt_checkpw(pass, auth_user->password) != 0) {
            // Passwords don't match
            free(auth_user);
            auth_user = NULL;
            return false;
        }

        return true;
    }

    return false;
}

user_t *terminal_get_user() {
    return auth_user;
}

void terminal_print_user_details() {
    if (auth_user == NULL) {
        fprintf(stderr, "Not authenticated!");
        return;
    }

    printf("User details:\n"
           "Name: %s %s\n"
           "Email: %s\n"
           "Balance: %.2f\n",
           auth_user->first_name,
           auth_user->last_name,
           auth_user->email,
           auth_user->balance
    );
}

void terminal_print_user_records() {
    if (auth_user == NULL) {
        fprintf(stderr, "Not authenticated!");
        return;
    }

    int record_count = 0;
    record_t *records = terminal_get_records(&record_count);

    printf("%d record%s\n", record_count, record_count == 1 ? "" : "s");

    for (int i = 0; i < record_count; i++) {
        record_t *record = records + i;

        double abs_amount = record->amount < 0 ? -1 * record->amount : record->amount;
        char *action = record->amount < 0 ? "withdrawn from" : "added to";

        printf("Record (%s): %.2f %s account at %s\n",
               record->id,
               abs_amount,
               action,
               record->datetime
        );
    }
}

double terminal_get_balance() {
    if (auth_user == NULL) {
        fprintf(stderr, "Not authenticated!");
        return -1;
    }

    return auth_user->balance;
}

record_t *terminal_get_records(int *count) {
    if (auth_user == NULL) {
        fprintf(stderr, "Not authenticated!");
        return NULL;
    }

    char query[QUERY_BUF_SIZE];
    sprintf(query, "SELECT * FROM records WHERE user_id='%s' ORDER BY created_at DESC;", auth_user->id);

    PGconn *conn = db_get_connection();
    PGresult *res = PQexec(conn, query);
    ExecStatusType status = PQresultStatus(res);

    if (status != PGRES_TUPLES_OK || PQntuples(res) == 0) return NULL;

    int rows = PQntuples(res);
    int cols = PQnfields(res);
    record_t *records = (record_t *) malloc(rows * sizeof(record_t));
    char *end_ptr;

    for (int row = 0; row < rows; row++) {
        record_t *record = records + row;

        for (int col = 0; col < cols; col++) {
            char *field = PQfname(res, col);
            char *value = PQgetvalue(res, row, col);

            if (strcmp(field, "id") == 0) strcpy(record->id, value);
            else if (strcmp(field, "amount") == 0) record->amount = strtod(value, &end_ptr);
            else if (strcmp(field, "created_at") == 0) strcpy(record->datetime, value);
        }
    }

    *count = rows;
    return records;
}

bool terminal_add_funds(double amount) {
    if (auth_user == NULL) {
        fprintf(stderr, "Not authenticated!");
        return NULL;
    }

    char query[QUERY_BUF_SIZE];
    sprintf(query,
            "INSERT INTO records(user_id,amount) VALUES('%s',%.2f);"
            "UPDATE users SET balance=balance+%.2f WHERE id='%s';",
            auth_user->id, amount, amount, auth_user->id
    );

    PGconn *conn = db_get_connection();
    PGresult *res = PQexec(conn, query);
    ExecStatusType status = PQresultStatus(res);

    if (status != PGRES_COMMAND_OK) {
        fprintf(stderr, "Unable to add funds: %s", PQerrorMessage(conn));
        return false;
    }

    auth_user->balance += amount;
    return true;
}

bool terminal_withdraw_funds(double amount) {
    if (auth_user == NULL) {
        fprintf(stderr, "Not authenticated!");
        return NULL;
    } else if (auth_user->balance - amount < 0) {
        fprintf(stderr, "Insufficient balance\n");
        return false;
    }

    char query[QUERY_BUF_SIZE];
    sprintf(query,
            "INSERT INTO records(user_id,amount) VALUES('%s',%.2f);"
            "UPDATE users SET balance=balance-%.2f WHERE id='%s';",
            auth_user->id, -amount, amount, auth_user->id
    );

    PGconn *conn = db_get_connection();
    PGresult *res = PQexec(conn, query);
    ExecStatusType status = PQresultStatus(res);

    if (status != PGRES_COMMAND_OK) {
        fprintf(stderr, "Unable to add funds: %s", PQerrorMessage(conn));
        return false;
    }

    auth_user->balance -= amount;
    return true;
}

void terminal_quit() {
    if (auth_user != NULL) {
        free(auth_user);
        auth_user = NULL;
    }
}