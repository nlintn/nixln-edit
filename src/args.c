#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#include "VERSION.h"

void print_help(char *file_name) {
    printf("Usage: %s [OPTION]... LINK_NAME\n", file_name == NULL ? "./nixln-edit" : file_name);
    printf("Make quick temporary changes to files which link into e.g. /nix/store");
    printf("\n");
    printf("Available options:\n");
    printf(" -e <EDITOR>\t\tspecify editor to use, default is $EDITOR\n");
    printf(" -h         \t\tprint this help and exit\n");
    printf(" -v         \t\tdisplay the version number and exit\n");
}

void print_arg_error(char *file_name, char *error_msg) {
    fprintf(stderr, "%s: %s\n", file_name == NULL ? "nixln-edit" : file_name, error_msg);
    fprintf(stderr, "Try '%s -h' for more information.\n", file_name == NULL ? "nixln-edit" : file_name);
}

struct args parse_args(int argc, char **argv) {
    struct args ret = {
        .self = NULL,
        .link_name = NULL,
        .editor = NULL };

    if (argc <= 0) {
        fprintf(stderr, "failed to parse args\n");
        exit(EXIT_FAILURE);
    }
    ret.self = malloc(strlen(argv[0]) + 1);
    if (ret.self == NULL) {
        print_run_error("failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    strcpy(ret.self, argv[0]);

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '\0') {
            continue;
        }
        if (argv[i][0] != '-') {
            ret.link_name = malloc(strlen(argv[i]) + 1);
            if (ret.link_name == NULL) {
                print_run_error("failed to allocate memory");
                exit(EXIT_FAILURE);
            }
            strcpy(ret.link_name, argv[i]);
            continue;
        }
        switch (argv[i][1]) {
            case 'e':
                if (i + 1 == argc) {
                    print_arg_error(ret.self, "no value for option [-e] specified");
                    exit(EXIT_FAILURE);
                }
                ret.editor = malloc(strlen(argv[i + 1]) + 1);
                if (ret.editor == NULL) {
                    print_run_error("failed to allocate memory");
                    exit(EXIT_FAILURE);
                }
                strcpy(ret.editor, argv[i + 1]);
                i += 1;
            break;

            case 'h':
                print_help(ret.self);
                exit(EXIT_SUCCESS);
            
            case 'v':
                printf("%s %s \n", NAME, VERSION);
                exit(EXIT_SUCCESS);

            default:
                print_arg_error(ret.self, "unknown option");
                exit(EXIT_FAILURE);
            break;
        }
    }
    
    if (ret.link_name == NULL) {
        print_arg_error(ret.self, "missing link operand"); 
        exit(EXIT_FAILURE);
    }
    if (ret.editor == NULL) {
        char *ed_env = getenv("EDITOR");
        if (ed_env == NULL) {
            print_arg_error(ret.self, "no editor specified (either by the [-e] option or the $EDITOR environment variable)");
            exit(EXIT_FAILURE);
        }
        ret.editor = malloc(strlen(ed_env) + 1);
        if (ret.editor == NULL) {
            print_run_error("failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        strcpy(ret.editor, ed_env);
    }

    return ret;
}

