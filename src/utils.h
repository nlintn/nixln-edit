#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__attribute__((__always_inline__))
static inline void print_run_error(const char *error_msg) {
    fprintf(stderr, "Error: %s - %s\n", error_msg, strerror(errno));
}

__attribute__((__always_inline__, __noreturn__))
static inline void print_run_error_exit(const char *error_msg) {
    print_run_error(error_msg);
    exit(EXIT_FAILURE);
} 

__attribute__((__always_inline__))
static inline void check_malloc(const void *ptr) {
    if (ptr == NULL) {
        print_run_error_exit("failed to allocate memory");
    }
}

char *get_directory_of(const char *file_path);
int link_restore(const char *dest, const char *name);

