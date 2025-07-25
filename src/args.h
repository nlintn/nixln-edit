struct args {
    char *editor;
    char **link_paths;
    int link_count;
};

struct args parse_args(int argc, const char *const *argv);

