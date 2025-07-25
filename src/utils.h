#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LEN(x) (sizeof(x) / sizeof(*x))

__attribute__((__always_inline__))
static inline void print_run_error(const char *err_msg, const char *err_subject) {
    fprintf(stderr, "Error: %s - %s: %s\n", err_msg, strerror(errno), err_subject);
}

__attribute__((__always_inline__, __noreturn__))
static inline void print_run_error_exit(const char *error_msg, const char *err_subject) {
    print_run_error(error_msg, err_subject);
    exit(EXIT_FAILURE);
}

__attribute__((__always_inline__))
static inline void check_malloc(const void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Error: failed to allocate memory - %s:\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int link_restore(int path_count, const char *const dests[path_count], const char *const paths[path_count]);

