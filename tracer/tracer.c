#include "common.h"

#include <stdbool.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include "arch_generic.h"
#include "fail.h"


static void wait_for_trap(pid_t pid)
{
    int status;
    do {
        pid_t p = waitpid(pid, &status, WUNTRACED);
        if (p == -1)
            fatal(E_RARE, "Can't wait");
    } while (!(WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP));
}


static void wait_for_syscall(pid_t pid)
{
    if (ptrace(PTRACE_SYSCALL, pid, NULL, 0) == -1)
        fatal_e(E_RARE, "Can't wait for syscall");

    wait_for_trap(pid);
}


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

        /*
         *{
         *    int opts = PTRACE_O_EXITKILL;
         *    if (ptrace(PTRACE_SETOPTIONS, pid, NULL, &opts) == -1)
         *        fatal_e(E_RARE, "Can't set ptrace options");
         *}
         */

        while (true) {
            wait_for_syscall(pid);

            {
                struct user_regs_struct regs;
                if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1)
                    fatal_e(E_RARE, "Can't read registers");

                print_user_regs(pid, &regs);

            }

            putchar('\n');

            wait_for_syscall(pid);

/*
 *            {
 *                struct user_regs_struct regs;
 *                if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1)
 *                    fatal_e(E_RARE, "Can't read registers");
 *
 *                print_user_regs(pid, &regs);
 *            }
 */

            print("\n--------------------------------\n\n");
        }
    }

    return 0;
}
