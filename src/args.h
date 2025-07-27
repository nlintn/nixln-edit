struct args {
    char *editor;
    char *const *link_paths;
    int link_count;
};

struct args parse_args(int argc, char *const *argv);

