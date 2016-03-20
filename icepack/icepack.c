#include "common.h"

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "fail.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"


int verbosity = 0;


void exit_with_usage()
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
} get_args(int argc, char** argv)
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
    return 0;
}
