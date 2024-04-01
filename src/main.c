#include "VERSION.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/wait.h>

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

void print_run_error(char *error_msg) {
    fprintf(stderr, "Error: %s - %s\n", error_msg, strerror(errno));
}

struct args {
    char *self;
    char *link_name;
    char *editor;
};

// TODO: fix really primitive arg parsing
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

int link_restore(char *dest, char *name) {
    if (remove(name) == -1) {
        print_run_error("failed to remove temporary copy of link destination");
        return 1;
    }
    if (symlink(dest, name) == -1) {
        print_run_error("failed to restore soft link");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    struct args args = parse_args(argc, argv);
   
    int n = 20;
    char *ln_dest_buf = malloc(n);
    if (ln_dest_buf == NULL) {
        print_run_error("failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    int ret = readlink(args.link_name, ln_dest_buf, n);
    while (ret == n) {
        free(ln_dest_buf);
        n += 10;
        ln_dest_buf = malloc(n);
        if (ln_dest_buf == NULL) {
            print_run_error("failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        ret = readlink(args.link_name, ln_dest_buf, n);
    }
    if (ret == -1) {
        print_run_error("failed to read link target");
        exit(EXIT_FAILURE);
    }
    ln_dest_buf[ret] = '\0';

    int ln_fd = open(ln_dest_buf, O_RDONLY);
    if (ln_fd == -1) {
        print_run_error("failed to open link destination for reading");
        exit(EXIT_FAILURE);
    }

    if (remove(args.link_name) == -1) {
        print_run_error("failed to remove link");
        exit(EXIT_FAILURE);
    }

    int new_fd = creat(args.link_name, 0777);
    if (new_fd == -1) {
        print_run_error("failed to create temporary file");
        if (symlink(ln_dest_buf, args.link_name) == -1) {
            print_run_error("failed to restore soft link");
        }
        exit(EXIT_FAILURE);
    }

    char ln_buf[256];
    int read_amount;
    while ((read_amount = read(ln_fd, ln_buf, 256)) > 0) {
        // TODO: account for partiol writes
        if (write(new_fd, ln_buf, read_amount) == -1) {
            print_run_error("failed to set up temporary copy of link destination to edit");
            (void) link_restore(ln_dest_buf, args.link_name);
            exit(EXIT_FAILURE);
        }
    }
    if (read_amount == -1) {
        print_run_error("failed to read link destination");
        (void) link_restore(ln_dest_buf, args.link_name);
        exit(EXIT_FAILURE);
    }
    close(ln_fd);
    close(new_fd);

    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error: failed to spawn editor process\n");
        (void) link_restore(ln_dest_buf, args.link_name);
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        char *ed_args[] = { "/bin/sh", args.editor, args.link_name, NULL };
        if (execv("/bin/sh", ed_args) == -1) {
            print_run_error("failed to execute editor");
            (void) link_restore(ln_dest_buf, args.link_name);
            exit(EXIT_FAILURE);
        }
    }
    if (waitpid(pid, NULL, 0) == -1) {
        print_run_error("failed to yield editor process");
    }

    return link_restore(ln_dest_buf, args.link_name);
}

