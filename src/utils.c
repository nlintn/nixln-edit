#include "utils.h"

#include <unistd.h>

int link_restore(int path_count, const char *const dests[path_count], const char *const paths[path_count]) {
    int success = 0;
    for (int i = 0; i < path_count; i++) {
        if (remove(paths[i]) == -1) {
            print_error("failed to remove temporary copy of link", paths[i]);
            success = 1;
        }
        if (symlink(dests[i], paths[i]) == -1) {
            print_error("failed to restore link to destination", dests[i]);
            success = 1;
        }
    }
    return success;
}

