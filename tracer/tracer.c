#include "common.h"

#include <stdbool.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fail.h"


int main(int argc, char** argv)
{
    if (argc < 2)
        fatal(E_USAGE, "Usage:  tracer PROG [ARGS]...");

    pid_t pid = fork();
    if (pid == -1)
        fatal_e(E_RARE, "Can't fork");

    if (pid == 0) {
        if (ptrace(PTRACE_TRACEME, -1, NULL, NULL) == -1)
            fatal_e(E_RARE, "Can't request trace");
        execvp(argv[1], argv + 1);

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

        int data = PTRACE_O_EXITKILL;
        if (ptrace(PTRACE_SETOPTIONS, pid, &data, NULL) == -1)
            fatal_e(E_RARE, "Can't set ptrace options");

        if (ptrace(PTRACE_SYSCALL, pid, &data, NULL) == -1)
            fatal_e(E_RARE, "Can't wait for syscall");

        while (true) {
            do {
                p = waitpid(pid, &status, WUNTRACED);
                if (p == -1)
                    fatal(E_RARE, "Can't wait");
            } while (!(WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP));

            v0("System call!");

            if (ptrace(PTRACE_SYSCALL, pid, &data, NULL) == -1)
                fatal_e(E_RARE, "Can't wait for syscall");
        }
    }

    return 0;
}
