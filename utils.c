#include <stdio.h>
#include "utils.h"
#include <termios.h>

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