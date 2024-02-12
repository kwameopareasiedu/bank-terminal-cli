#include <stdio.h>
#include "utils.h"
#include <termios.h>
#include <regex.h>

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

void get_input(char *input, int buf_size) {
    int c, idx = 0;

    while ((c = getchar()) != '\n' && c != EOF && idx < buf_size - 1) {
        input[idx++] = (char) c;
    }

    input[idx] = '\0';
}

/** For POSIX-compliant terminals, the input will not be seen */
void get_hidden_input(char *input, int buf_size) {
    struct termios current_config, new_config;
    tcgetattr(fileno(stdin), &current_config); // Store the current terminal flags
    new_config = current_config;
    new_config.c_lflag &= ~ECHO; // Disable echo
    tcsetattr(fileno(stdin), TCSANOW, &new_config); // Set the new terminal flags
    get_input(input, buf_size); // Get the password
    tcsetattr(fileno(stdin), TCSANOW, &current_config); // Reset terminal flags
}

bool is_email_valid(const char *email) {
    char *pattern = "[-_a-zA-Z0-9.+!%]*@[-_a-zA-Z0-9.]*";
    regex_t regex;
    regmatch_t match[1];

    if (regcomp(&regex, pattern, REG_ICASE)) {
        fprintf(stderr, "Failed to compile email regex\n");
        return false;
    }

    int err_code;
    char err_msg[512];

    if ((err_code = regexec(&regex, email, ARRAY_SIZE(match), match, 0)) != 0) {
        regerror(err_code, &regex, err_msg, 512);
        fprintf(stderr, "Failed to match email: %s\n", err_msg);
        return false;
    }

    return true;
}