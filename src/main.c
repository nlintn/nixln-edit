#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/wait.h>

#include "args.h"
#include "utils.h"
#include "sig_handler.h"

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
    
    int dir_fd = open(get_directory_of(args.link_name), O_DIRECTORY);
    if (dir_fd == -1) {
        print_run_error("failed to open directory of link");
        exit(EXIT_FAILURE);
    }

    int ln_fd = openat(dir_fd, ln_dest_buf, O_RDONLY);
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
    
    sig_h_setup(ln_dest_buf, args.link_name);

    char ln_buf[256];
    int read_amount;
    while ((read_amount = read(ln_fd, ln_buf, sizeof(ln_buf))) > 0) {
        // TODO: account for partial writes
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

