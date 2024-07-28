#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_run_error(const char *error_msg) {
    fprintf(stderr, "Error: %s - %s\n", error_msg, strerror(errno));
}

char *get_directory_of(const char *file_path) {
    int found = 0;
    unsigned long i = strlen(file_path) - 1;
    for (; i > 0; i--) {
        if (file_path[i] == '/') {
            found = 1;
            break;
        }
    }
    if (!found) {
        i = 1;
    }

    char *buf = malloc(i + 1);
    if (buf == NULL) {
        print_run_error("failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    if (found) {
        memcpy(buf, file_path, i);
    } else {
        buf[0] = '.';
    }
    buf[i] = '\0';

    return buf;
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

