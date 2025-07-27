#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <libgen.h>

#include "args.h"
#include "utils.h"
#include "sig_handler.h"

int main(int argc, char **argv) {
    struct args args = parse_args(argc, argv);

    char **ln_dest_bufs = calloc(args.link_count, sizeof(char *));
    int i = 0;

    sig_h_setup(&i, (const char *const *) ln_dest_bufs, (const char *const *) args.link_paths);

    for (; i < args.link_count; i++) {
        const char *link_name = args.link_paths[i];
        char **ln_dest_buf = &ln_dest_bufs[i];

        struct stat sb;
        if (lstat(link_name, &sb) == -1) {
            print_run_error_exit("failed to read file stats", link_name);
        }
        if (!S_ISLNK(sb.st_mode)) {
            fprintf(stderr, "Error: file is not a symbolic link (%s)\n", link_name);
            exit(EXIT_FAILURE);
        }

        int ret, n = 16;
        do {
            n += 16;
            *ln_dest_buf = realloc(*ln_dest_buf, n);
            check_malloc(*ln_dest_buf);
            ret = readlink(link_name, *ln_dest_buf, n);

        } while (ret == n);
        if (ret == -1) {
            print_run_error_exit("failed to read link", link_name);
        }
        (*ln_dest_buf)[ret] = '\0';

        char *link_name_copy = malloc(strlen(link_name) + 1);
        strcpy(link_name_copy, link_name);
        char *dir = dirname(link_name_copy);
        int dir_fd = open(dir, O_DIRECTORY);
        if (dir_fd == -1) {
            print_run_error_exit("failed to open directory of link", dir);
        }
        free(link_name_copy);

        int ln_fd = openat(dir_fd, *ln_dest_buf, O_RDONLY);
        if (ln_fd == -1) {
            print_run_error_exit("failed to open link destination for reading", *ln_dest_buf);
        }
        close(dir_fd);

        if (remove(link_name) == -1) {
            print_run_error_exit("failed to remove link", link_name);
        }

        // write permission for group and other not needed
        int new_fd = creat(link_name, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if (new_fd == -1) {
            print_run_error("failed to create temporary file", link_name);
            if (symlink(*ln_dest_buf, link_name) == -1) {
                print_run_error("failed to restore soft link", *ln_dest_buf);
            }
            exit(EXIT_FAILURE);
        }

        char ln_buf[256];
        int read_amount;
        while ((read_amount = read(ln_fd, ln_buf, sizeof(ln_buf))) > 0) {
            // TODO: account for partial writes
            if (write(new_fd, ln_buf, read_amount) == -1) {
                print_run_error("failed to copy data to new tmp file", link_name);
                (void) link_restore(i + 1, (const char *const *) ln_dest_bufs, (const char *const *) args.link_paths);
                exit(EXIT_FAILURE);
            }
        }
        if (read_amount == -1) {
            print_run_error("failed to read data from link destination", *ln_dest_buf);
            (void) link_restore(i + 1, (const char *const *) ln_dest_buf, (const char *const *) args.link_paths);
            exit(EXIT_FAILURE);
        }
        close(ln_fd);
        close(new_fd);
    }

    pid_t pid = fork();
    if (pid == -1) {
        print_run_error("failed to spawn editor process", "fork()");
        (void) link_restore(args.link_count, (const char *const *) ln_dest_bufs, (const char *const *) args.link_paths);
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        char *ed_args[1 + 1 + args.link_count + 1];
        ed_args[0] = args.editor;
        ed_args[1] = "--";
        memcpy(ed_args + 2, args.link_paths, args.link_count * sizeof(char *));
        ed_args[ARRAY_LEN(ed_args) - 1] = NULL;

        if (execvp(args.editor, ed_args) == -1) {
            print_run_error("failed to execute editor", args.editor);
            (void) link_restore(args.link_count, (const char *const *) ln_dest_bufs, (const char *const *) args.link_paths);
            exit(EXIT_FAILURE);
        }
    }
    if (waitpid(pid, NULL, 0) == -1) {
        print_run_error("failed to yield editor process", args.editor);
    }

    return link_restore(args.link_count, (const char *const *) ln_dest_bufs, (const char *const *) args.link_paths);
}

