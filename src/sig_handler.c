#include "sig_handler.h"

#include <signal.h>
#include <stddef.h>
#include <unistd.h>

#include "utils.h"

static const char *_link_dest;
static const char *_link_name;

static void sig_handler(int sig) {
    (void) sig;
    // TODO: remove (deleting the file) not async-signal-safe
    link_restore(_link_dest, _link_name);
    _exit(1);
}

void sig_h_setup(const char *link_dest, const char *link_name) {
    _link_dest = link_dest;
    _link_name = link_name;

    struct sigaction new_handler, old;
    new_handler.sa_handler = &sig_handler;
    new_handler.sa_flags = 0;
    sigemptyset(&new_handler.sa_mask);

#define SA(SIG) do { \
    sigaction(SIG, NULL, &old); \
    if (old.sa_handler != SIG_IGN) { \
        sigaction(SIG, &new_handler, NULL); \
    }} while (0)

    SA(SIGABRT);
    SA(SIGALRM);
    SA(SIGBUS);
    SA(SIGFPE);
    SA(SIGHUP);
    SA(SIGILL);
    SA(SIGINT);
    SA(SIGIO);
    SA(SIGPIPE);
    SA(SIGPOLL);
    SA(SIGPROF);
    SA(SIGPWR);
    SA(SIGQUIT);
    SA(SIGSEGV);
    SA(SIGSYS);
    SA(SIGTERM);
    SA(SIGTRAP);
    SA(SIGUSR1);
    SA(SIGUSR2);
    SA(SIGVTALRM);
    SA(SIGXCPU);
    SA(SIGXFSZ);
}

