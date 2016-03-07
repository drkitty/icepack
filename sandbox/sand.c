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


// These *need* to be moved to a config file as soon as this program is stable.
const char* essential_libs[] = {
    "lib64/ld-linux-x86-64.so.2",
    "lib/x86_64-linux-gnu/libc.so.6",
    "lib/x86_64-linux-gnu/libpcre.so.3",
};


int main(int argc, char** argv)
{
    if (argc < 3)
        fatal(E_USAGE, "Usage:  sand DIR PROG [ARGS...]");

    for (i = 0; i < lengthof(essential_libs); ++i) {
        size_t head_len = strlen(argv[1]);
        size_t tail_len = strlen(essential_libs[i]);

        char src_name[1 + tail_len + 1];
        src_name[0] = '/';
        memcpy(src_name + 1, essential_libs[i], tail_len);
        src_name[1 + tail_len] = '\0';
        int src = open(src_name, O_RDONLY);

        char dst_name[head_len + 1 + tail_len + 1];
        memcpy(dst_name, argv[1], head_len);
        // ...

        int dst = open(dst_name, O_WRONLY | O_CREAT, 0664);
        if (src == -1)
            fatal_e(E_RARE, "Can't open \"%s\"", essntial_libs[i]);
        while (true) {
        }

        if (close(src) == -1)
            fatal_e(E_RARE, "Can't close \"%s\"", essential_libs[i]);
    }

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
