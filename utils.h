#include <stdbool.h>
#include "types.h"

#ifndef UTILS
#define UTILS

void get_input(char *input, int buf_size);

void get_hidden_input(char *input, int buf_size);

bool is_email_valid(const char* email);

#endif