#pragma once

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_LEN(x) (sizeof(x) / sizeof(*x))

__attribute__((__always_inline__))
static inline void print_error(const char *err_msg, const char *err_subject) {
    fprintf(stderr, "Error: %s - %s (%s)\n", err_msg, strerror(errno), err_subject);
}

int link_restore(int path_count, const char *const dests[path_count], const char *const paths[path_count]);

__attribute__((__always_inline__, __noreturn__))
static inline void print_error_restore_exit(const char *error_msg, const char *err_subject,
        int path_count, const char *const dests[path_count], const char *const paths[path_count]) {
    print_error(error_msg, err_subject);
    link_restore(path_count, dests, paths);
    exit(EXIT_FAILURE);
}

__attribute__((__always_inline__))
static inline void check_malloc(const void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Error: failed to allocate memory - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}


