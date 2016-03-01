#include "common.h"

#include <stdbool.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <asm/unistd_64.h>

#include "fail.h"


#if TRACER_AMD64

static void print_user_string(pid_t pid, unsigned long long int addr)
{
    putchar('"');
    while (true) {
        long r = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
        if (r == -1) {
            warning_e("Can't read memory");
            return;
        }
        r = ((unsigned char)r) & 0xFF;
        if ((r & 0xFF) == '\0')
            break;
        putchar(r);
        ++addr;
    }

    print("\"\n");
}


static void print_user_regs(pid_t pid, struct user_regs_struct* regs)
{
    printf("     r15 = 0x%llx\n", regs->r15);
    printf("     r14 = 0x%llx\n", regs->r14);
    printf("     r13 = 0x%llx\n", regs->r13);
    printf("     r12 = 0x%llx\n", regs->r12);
    printf("     rbp = 0x%llx\n", regs->rbp);
    printf("     rbx = 0x%llx\n", regs->rbx);
    printf("     r11 = 0x%llx\n", regs->r11);
    printf("     r10 = 0x%llx\n", regs->r10);
    printf("     r9 = 0x%llx\n", regs->r9);
    printf("     r8 = 0x%llx\n", regs->r8);
    printf("     rax = 0x%llx\n", regs->rax);
    printf("     rcx = 0x%llx\n", regs->rcx);
    printf("     rdx = 0x%llx\n", regs->rdx);
    printf("     rsi = 0x%llx\n", regs->rsi);
    printf("     rdi = 0x%llx\n", regs->rdi);
    printf("orig_rax = 0x%llx\n", regs->orig_rax);
    printf("     rip = 0x%llx\n", regs->rip);
    printf("      cs = 0x%llx\n", regs->cs);
    printf("  eflags = 0x%llx\n", regs->eflags);
    printf("     rsp = 0x%llx\n", regs->rsp);
    printf("      ss = 0x%llx\n", regs->ss);
    printf(" fs_base = 0x%llx\n", regs->fs_base);
    printf(" gs_base = 0x%llx\n", regs->gs_base);
    printf("      ds = 0x%llx\n", regs->ds);
    printf("      es = 0x%llx\n", regs->es);
    printf("      fs = 0x%llx\n", regs->fs);
    printf("      gs = 0x%llx\n", regs->gs);

    if (regs->orig_rax == __NR_open) {
        print_user_string(pid, regs->rdi);
    }
}

#else
# error No architecture specified.
#endif


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
