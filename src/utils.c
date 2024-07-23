#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void print_run_error(const char *error_msg) {
    fprintf(stderr, "Error: %s - %s\n", error_msg, strerror(errno));
}

int link_restore(const char *dest, const char *name) {
    if (remove(name) == -1) {
        print_run_error("failed to remove temporary copy of link destination");
        return 1;
    }
    if (symlink(dest, name) == -1) {
        print_run_error("failed to restore link");
        return 1;
    }
    return 0;
}

