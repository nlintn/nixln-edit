#include "args.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "VERSION.h"

__attribute__((__noreturn__))
static inline void print_help(const char *file_name) {
    printf(
        "Usage: %s [OPTION]... LINK_NAME...\n"
        "Make quick temporary changes to files which link into e.g. /nix/store/...\n"
        "\n"
        "Available options:\n"
        " -e, --editor <EDITOR>\tspecify editor to use, default is $EDITOR\n"
        " -h, --help           \tprint this help and exit\n"
        " -v, --version        \tdisplay the version number and exit\n",
    file_name == NULL ? "./nixln-edit" : file_name);
    exit(EXIT_SUCCESS);
}

__attribute__((__always_inline__, __noreturn__))
static inline void print_arg_error(const char *file_name, const char *error_msg) {
    fprintf(stderr, "%s: %s\n", file_name == NULL ? NAME: file_name, error_msg);
    fprintf(stderr, "Try '%s --help' for more information.\n", file_name == NULL ? "nixln-edit" : file_name);
    exit(EXIT_FAILURE);
}

struct args parse_args(int argc, char *const *argv) {
    static struct args ret = {
        .link_paths = NULL,
        .link_count = 0,
        .editor = NULL };

    if (argc <= 0) {
        fprintf(stderr, "Error: failed to parse args\n");
        exit(EXIT_FAILURE);
    }
    const char *self = argv[0];

    static struct option long_options[] = {
        { "editor", required_argument, NULL, 'e' },
        { "help", no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'v' },
        { NULL, 0, NULL, 0 },
    };

    int c, opt_index = 0;
    while ((c = getopt_long(argc, argv, "e:hv", long_options, &opt_index)) != -1) {
        switch (c) {
            case 'e':
                ret.editor = optarg;
                break;
            case 'h':
                print_help(self);
            case 'v':
                puts(NAME " " VERSION);
                exit(EXIT_SUCCESS);
        }
    }

    if (optind == argc) {
        print_arg_error(self, "missing link operand(s)"); 
    }
    ret.link_count = argc - optind;
    ret.link_paths = argv + optind;

    if (ret.editor == NULL) {
        char *ed_env = getenv("EDITOR");
        if (ed_env == NULL) {
            print_arg_error(self, "no editor specified (either by the [-e] option or the $EDITOR environment variable)");
        }
        ret.editor = ed_env;
    }

    return ret;
}

