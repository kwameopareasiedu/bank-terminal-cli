#ifndef TYPES
#define TYPES

#define QUERY_BUF_SIZE 512
#define CMD_BUF_SIZE 512

#define CMD_REGISTER "register"
#define CMD_LOGIN "login"
#define CMD_DETAILS "details"
#define CMD_ADD_FUNDS "fund"
#define CMD_WITHDRAW "withdraw"
#define CMD_RECORDS "records"
#define CMD_LOGOUT "logout"
#define CMD_HELP "help"
#define CMD_EXIT "exit"

#define DB_NAME_KEY "name"
#define DB_USER_KEY "user"
#define DB_PASS_KEY "pass"
#define DB_HOST_KEY "host"
#define DB_PORT_KEY "port"

typedef struct {
    char host[512];
    char port[512];
    char name[512];
    char user[512];
    char pass[512];
} db_conn_data_t;

typedef struct {
    char id[37];
    char first_name[65];
    char last_name[65];
    char password[97];
    char email[97];
    double balance;
} user_t;

typedef struct {
    char id[37];
    double amount;
    char datetime[27];
} record_t;

#endif