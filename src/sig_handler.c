#include "sig_handler.h"

#include <signal.h>
#include <stddef.h>
#include <unistd.h>

#include "utils.h"

static const int *_i;
static const char *const *_link_dests;
static const char *const *_link_paths;

static void sig_handler(int sig) {
    (void) sig;
    // TODO: remove (deleting the file) not async-signal-safe
    link_restore(*_i, _link_dests, _link_paths);
    _exit(1);
}

// Catch every Signal with default action 'Term' or 'Core' (except 'SIGKILL', because not catchable)
static const int sigs[] = {
    SIGABRT, SIGALRM, SIGBUS, SIGFPE, SIGHUP, SIGILL, SIGINT, SIGIO,
    SIGPIPE, SIGPOLL, SIGPROF, SIGPWR, SIGQUIT, SIGSEGV, SIGSYS,
    SIGTERM, SIGTRAP, SIGUSR1, SIGUSR2, SIGVTALRM, SIGXCPU, SIGXFSZ,
};
void sig_h_setup(const int* i, const char *const *link_dest, const char *const *link_name) {
    _i = i;
    _link_dests = link_dest;
    _link_paths = link_name;

    struct sigaction new_handler, old;
    new_handler.sa_handler = &sig_handler;
    new_handler.sa_flags = 0;
    sigemptyset(&new_handler.sa_mask);

    for (unsigned i = 0; i < sizeof(sigs) / sizeof(*sigs); i++) {
        sigaction(sigs[i], NULL, &old);
        if (old.sa_handler == SIG_DFL) {
            sigaction(sigs[i], &new_handler, NULL);
        }
    }
}

