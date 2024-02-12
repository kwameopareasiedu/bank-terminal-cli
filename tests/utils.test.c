#include <stdio.h>
#include "../utils.h"

int main() {
    char * valid_email="test@gmail.com", *invalid_email="hello";

    printf(""
           "%s valid: %b\n"
           "%s valid: %b\n",
           valid_email, is_email_valid(valid_email),
           invalid_email, is_email_valid(invalid_email)
    );

    if (is_email_valid("test@gmail.com")) {
        return 0;
    } else return 1;
}