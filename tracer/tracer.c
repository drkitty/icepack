#include "common.h"

#include <asm/unistd_64.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include "arch_generic.h"
#include "fail.h"


static bool wait_for_trap(pid_t pid)
{
    int status;

    while (true) {
        pid_t p = waitpid(pid, &status, WUNTRACED);
        if (p == -1)
            fatal(E_RARE, "Can't wait");
        else if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
            return true;
        else if (WIFEXITED(status) || WIFSIGNALED(status))
            return false;
    }
}


static bool wait_for_syscall(pid_t pid)
{
    if (ptrace(PTRACE_SYSCALL, pid, NULL, 0) == -1)
        fatal_e(E_RARE, "Can't wait for syscall");

    return wait_for_trap(pid);
}


int main(int argc, char** argv)
{
    if (argc < 3)
        fatal(E_USAGE, "Usage:  tracer ROOTDIR PROG [ARGS]...");

    char* rootdir;

    {
        rootdir = realpath(argv[1], NULL);
        if (rootdir == NULL)
            fatal_e(E_COMMON, "Can't resolve fake root directory");
        printf("%s\n", rootdir);
    }

    pid_t pid = fork();
    if (pid == -1)
        fatal_e(E_RARE, "Can't fork");

    if (pid == 0) {
        argv[2] = realpath(argv[2], NULL);
        if (argv[2] == NULL)
            fatal_e(E_COMMON, "Can't resolve program path");

        chdir(rootdir);
        if (ptrace(PTRACE_TRACEME, -1, NULL, NULL) == -1)
            fatal_e(E_RARE, "Can't request trace");
        execvp(argv[2], argv + 2);

        fatal_e(E_COMMON, "Can't execute program");
    }

    {
        int status;
        pid_t p;
        do {
            p = waitpid(pid, &status, WUNTRACED);
            if (p == -1)
                fatal(E_RARE, "Can't wait");
        } while (!WIFSTOPPED(status));

        {
            int opts = PTRACE_O_EXITKILL;
            if (ptrace(PTRACE_SETOPTIONS, pid, NULL, opts) == -1)
                fatal_e(E_RARE, "Can't set ptrace options");
        }

        while (true) {
            if (!wait_for_syscall(pid))
                break;

            {
                struct syscall_info si = get_syscall_info(pid);
                printf("arg1 = 0x%llx\n", si.args[0]);
                if (si.num == __NR_open) {
                    char buf[256];
                    if (get_user_string(pid, si.args[0], buf, sizeof(buf)))
                        puts(buf);
                    else
                        warning("Can't get string");
                }
            }

            if (!wait_for_syscall(pid))
                break;

            putchar('\n');
        }
    }

    free(rootdir);

    return 0;
}
