#include "common.h"

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "fail.h"
#include "forkutil.h"
#include "igloo.h"


int verbosity = 0;

int child_pid;


static void kill_child(int signum)
{
    if (signum == SIGALRM)
        kill(child_pid, SIGKILL);

    alarm(2);
    kill(child_pid, signum);
    int status;
    wait_prog(&child_pid, &status);
}


static void handle_signal(int signum)
{
    if (child_pid != 0)
        kill_child(signum);

    kill(getpid(), signum);
}


static void register_signal_handler()
{
    sigset_t ss;
    if (sigemptyset(&ss) != 0)
        fatal_e(E_RARE, "Can't empty signal set");
    struct sigaction sa = {
        .sa_handler = handle_signal,
        .sa_mask = ss,
        .sa_flags = 0,
    };
    int signums[] = {
        SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGPIPE, SIGTERM
    };
    for (unsigned int i = 0; i < lengthof(signums); ++i)
        if (sigaction(signums[i], &sa, NULL) != 0)
            fatal_e(E_RARE, "Can't set signal handler");
}


static void exit_with_usage()
{
    print(
        "Usage:  icepack [OPTIONS] FILE\n"
        "\n"
        "Options:\n"
        "  -h   Show this usage message\n"
        "  -v   Increase verbosity\n"
    );
}


struct args {
    char* name;
};


static struct args get_args(int argc, char** argv)
{
    struct args args;

    opterr = 0;
    while (true) {
        int c = getopt(argc, argv, "hv");
        if (c == -1)
            break;
        else if (c == '?')
            fatal(E_USAGE, "Unrecognized option '%c'", optopt);
        else if (c == 'h')
            exit_with_usage();
        else if (c == 'v')
            ++verbosity;
    }

    if (argv[optind] == NULL)
        fatal(E_USAGE, "Missing filename");
    args.name = argv[optind];
    ++optind;
    if (argv[optind] != NULL)
        fatal(E_USAGE, "Trailing arguments");

    return args;
}


int main(int argc, char** argv)
{
    struct args args = get_args(argc, argv);

    register_signal_handler();

    struct ipak p;
    ipak_load(&p, args.name);
    return 0;
}
