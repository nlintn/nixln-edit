struct args {
    char *self;
    char *link_name;
    char *editor;
};

struct args parse_args(int argc, char **argv);

