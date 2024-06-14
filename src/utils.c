#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

void print_run_error(char *error_msg) {
    fprintf(stderr, "Error: %s - %s\n", error_msg, strerror(errno));
}

