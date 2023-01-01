#include "handle_error.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
void handle_error(int en, const char * msg) {
    if (en) {
        printf("%s: %s\n", msg, strerror(en));
        exit(1);
    }
}