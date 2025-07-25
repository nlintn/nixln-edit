#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#include "VERSION.h"

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
}

__attribute__((__noreturn__))
static inline void print_arg_error(const char *file_name, const char *error_msg) {
    fprintf(stderr, "%s: %s\n", file_name == NULL ? NAME: file_name, error_msg);
    fprintf(stderr, "Try '%s -h' for more information.\n", file_name == NULL ? "nixln-edit" : file_name);
    exit(EXIT_FAILURE);
}
__attribute__((__noreturn__))
static inline void print_unknown_opt_error(const char *file_name, const char *option) {
#define UNKNOWN_OPT_MSG "unknown option: "
    char *msg_buf = malloc(strlen(UNKNOWN_OPT_MSG) + strlen(option) + 1);
    sprintf(msg_buf, UNKNOWN_OPT_MSG "%s", option);
    print_arg_error(file_name, msg_buf);
#undef UNKNOWN_OPT_MSG
}

static inline void set_editor(const char *val, char **buf) {
    *buf = realloc(*buf, (strlen(val) + 1) * sizeof(char));
    check_malloc(*buf);
    strcpy(*buf, val);
}

static inline void add_link(const char *val, char ***buf_list, int *buf_len) {
    *buf_list = realloc(*buf_list, (*buf_len + 1) * sizeof(char *));
    (*buf_list)[*buf_len] = malloc(strlen(val) + 1);
    check_malloc((*buf_list)[*buf_len]);
    strcpy((*buf_list)[*buf_len], val);
    *buf_len += 1;
}

struct args parse_args(int argc, const char *const *argv) {
    struct args ret = {
        .link_paths = NULL,
        .link_count = 0,
        .editor = NULL };

    if (argc <= 0) {
        fprintf(stderr, "failed to parse args\n");
        exit(EXIT_FAILURE);
    }
    const char *self = argv[0];

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '\0') {
            continue;
        }
        if (argv[i][0] != '-') {
            add_link(argv[i], &ret.link_paths, &ret.link_count);
            continue;
        }
        switch (argv[i][1]) {
            case 'e':
                if (i + 1 == argc) {
                    print_arg_error(self, "no value for option [-e] specified");
                }
                set_editor(argv[i + 1], &ret.editor);
                i += 1;
            break;

            case 'h':
                print_help(self);
                exit(EXIT_SUCCESS);

            case 'v':
                puts(NAME " " VERSION);
                exit(EXIT_SUCCESS);

            case '-':
                switch (argv[i][2]) {
                    // just read file names after '--'
                    case '\0':
                        while (++i < argc) {
                            add_link(argv[i], &ret.link_paths, &ret.link_count);
                        }
                        goto exitLoop;

                    case 'e':
                        if (!strcmp(argv[i] + 3, (char *) "--editor" + 3)) {
                            if (i + 1 == argc) {
                                print_arg_error(self, "no value for option [-e] specified");
                            }
                            set_editor(argv[i + 1], &ret.editor);
                            i += 1;
                            goto continueLoop;
                        }
                    break;

                    case 'h':
                        if (!strcmp(argv[i] + 3, (char *) "--help" + 3)) {
                            print_help(self);
                            exit(EXIT_SUCCESS);
                        }
                    break;

                    case 'v':
                        if (!strcmp(argv[i] + 3, (char *) "--version" + 3)) {
                            puts(NAME " " VERSION);
                            exit(EXIT_SUCCESS);
                        }
                    break;
                }

            __attribute__ ((fallthrough));
            default:
                print_unknown_opt_error(self, argv[i]);
            break;
        }
        continueLoop:
    }

    exitLoop:

    if (ret.link_paths == NULL) {
        print_arg_error(self, "missing link operand"); 
    }
    if (ret.editor == NULL) {
        char *ed_env = getenv("EDITOR");
        if (ed_env == NULL) {
            print_arg_error(self, "no editor specified (either by the [-e] option or the $EDITOR environment variable)");
        }
        set_editor(ed_env, &ret.editor);
    }

    return ret;
}

