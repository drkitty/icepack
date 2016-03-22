#include "common.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fail.h"


int main(int argc, char** argv)
{
    if (argc < 3)
        fatal(E_USAGE, "Usage:  sand DIR PROG [ARGS...]");

    struct passwd* pw = getpwnam("sand");
    if (pw == NULL)
        fatal_e(E_RARE, "Can't find user \"sand\"");

    if (unshare(CLONE_NEWNS) != 0)
        fatal_e(E_COMMON, "Can't unshare mount namespace");

    if (chdir(argv[1]) != 0)
        fatal_e(E_COMMON, "Can't chdir");

    if (chroot(".") != 0)
        fatal_e(E_COMMON, "Can't chroot");

    if (setgid(pw->pw_gid) != 0)
        fatal_e(E_RARE, "Can't set gid");

    if (setuid(pw->pw_uid) != 0)
        fatal_e(E_RARE, "Can't set uid");

    execvp(argv[2], argv + 2);

    fatal_e(E_COMMON, "Can't execvp (check dynamic linker)");
}
