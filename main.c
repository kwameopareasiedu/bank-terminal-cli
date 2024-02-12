#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "database.h"
#include "utils.h"
#include "terminal.h"

#define CHECK_AUTHENTICATED_USER if (terminal_get_user() == NULL) { fprintf(stderr, "Not logged in!\n"); continue; }

void show_prompt(user_t *user);

int main() {
    if (!db_connect()) {
        fprintf(stderr, "Unable to connect to db");
        return -1;
    }

    printf("Bank Terminal CLI - 1.0.0\n");

    char cmd[CMD_BUF_SIZE];

    while (true) {
        show_prompt(terminal_get_user());
        get_input(cmd, CMD_BUF_SIZE);

        if (strcmp(cmd, CMD_REGISTER) == 0) {
            if (terminal_get_user() != NULL) {
                fprintf(stderr, "Already logged in. Logout first!\n");
                continue;
            }

            char first_name[CMD_BUF_SIZE], last_name[CMD_BUF_SIZE],
                    email[CMD_BUF_SIZE], pass[CMD_BUF_SIZE], confirm_pass[CMD_BUF_SIZE];

            printf("\tFirst name: ");
            get_input(first_name, CMD_BUF_SIZE);
            printf("\tLast name: ");
            get_input(last_name, CMD_BUF_SIZE);
            printf("\tEmail: ");
            get_input(email, CMD_BUF_SIZE);
            printf("\tPassword (Input will be hidden): ");
            get_hidden_input(pass, CMD_BUF_SIZE);
            printf("\tConfirm password (Input will be hidden): ");
            get_hidden_input(confirm_pass, CMD_BUF_SIZE);

            if (strlen(first_name) == 0) {
                fprintf(stderr, "First name is required!\n");
            } else if (strlen(last_name) == 0) {
                fprintf(stderr, "Last name is required!\n");
            } else if (!is_email_valid(email)) {
                fprintf(stderr, "Email is not valid!\n");
            } else if (strlen(pass) < 6) {
                fprintf(stderr, "Password must be at least six (6) characters!\n");
            } else if (strcmp(pass, confirm_pass) != 0) {
                fprintf(stderr, "Passwords must match!\n");
            } else {
                if (terminal_register(first_name, last_name, email, pass)) printf("Success!\n");
                else fprintf(stderr, "Register failed!\n");
            }
        } else if (strcmp(cmd, CMD_LOGIN) == 0) {
            if (terminal_get_user() != NULL) {
                fprintf(stderr, "Already logged in. Logout first!\n");
                continue;
            }

            char email[CMD_BUF_SIZE], pass[CMD_BUF_SIZE];

            printf("\tEmail: ");
            get_input(email, CMD_BUF_SIZE);
            printf("\tPassword (Input will be hidden): ");
            get_hidden_input(pass, CMD_BUF_SIZE);
            terminal_authenticate(email, pass);

            if (terminal_get_user() != NULL) printf("Success!\n");
            else fprintf(stderr, "Login failed!\n");
        } else if (strcmp(cmd, CMD_DETAILS) == 0) {
            CHECK_AUTHENTICATED_USER
            terminal_print_user_details();
        } else if (strcmp(cmd, CMD_ADD_FUNDS) == 0) {
            CHECK_AUTHENTICATED_USER
            char amount_buf[CMD_BUF_SIZE], *end_ptr;

            printf("\tAmount to add: ");
            get_input(amount_buf, CMD_BUF_SIZE);
            double amount = strtod(amount_buf, &end_ptr);

            if (amount > 0 && terminal_add_funds(amount)) {
                printf("%.2f added to account balance\n", amount);
                printf("Balance is now %.2f\n", terminal_get_balance());
            } else fprintf(stderr, "Invalid amount\n");
        } else if (strcmp(cmd, CMD_WITHDRAW) == 0) {
            CHECK_AUTHENTICATED_USER
            char amount_buf[CMD_BUF_SIZE], *end_ptr;

            printf("\tAmount to withdraw: ");
            get_input(amount_buf, CMD_BUF_SIZE);
            double amount = strtod(amount_buf, &end_ptr);

            if (amount > 0 && terminal_withdraw_funds(amount)) {
                printf("%.2f withdrawn from account balance\n", amount);
                printf("Balance is now %.2f\n", terminal_get_balance());
            } else fprintf(stderr, "Invalid amount\n");
        } else if (strcmp(cmd, CMD_RECORDS) == 0) {
            CHECK_AUTHENTICATED_USER
            terminal_print_user_records();
        } else if (strcmp(cmd, CMD_LOGOUT) == 0) {
            CHECK_AUTHENTICATED_USER
            terminal_quit();
        } else if (strcmp(cmd, CMD_EXIT) == 0) {
            break;
        } else if (strcmp(cmd, CMD_HELP) == 0) {
            printf(""
                   "Bank terminal CLI - Help\n"
                   "\n"
                   "register    - Create account\n"
                   "login       - Login with email and password\n"
                   "* details   - Display account details\n"
                   "* fund      - Add funds to account\n"
                   "* withdraw  - Withdraw funds from account\n"
                   "* records   - Display account records of funds and withdrawals\n"
                   "* logout    - Logout of account\n"
                   "help        - Show this help message\n"
                   "exit        - Exit the program\n"
                   "\n"
                   "* requires login\n"
            );
        } else {
            printf("%s: command not found\n", cmd);
        }
    }

    terminal_quit();
    db_disconnect();
    return 0;
}

void show_prompt(user_t *user) {
    if (user != NULL) {
        printf("[%s %s]@cli: $ ", user->first_name, user->last_name);
    } else printf("cli: $ ");
}

