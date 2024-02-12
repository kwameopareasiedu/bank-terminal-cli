#include <stdbool.h>
#include "types.h"

#ifndef TERMINAL
#define TERMINAL

bool terminal_authenticate(char *email, char *pass);

user_t *terminal_get_user();

void terminal_print_user_details();

void terminal_print_user_records();

double terminal_get_balance();

record_t *terminal_get_records(int *count);

bool terminal_add_funds(double amount);

bool terminal_withdraw_funds(double amount);

void terminal_quit();

#endif