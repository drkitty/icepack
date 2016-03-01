#include "common.h"

#include <asm/unistd_64.h>
#include <sys/ptrace.h>
#include <stdbool.h>
#include <stdio.h>

#include "arch_generic.h"
#include "fail.h"


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


void print_user_regs(pid_t pid, struct user_regs_struct* regs)
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

    if (regs->orig_rax == __NR_open)
        print_user_string(pid, regs->rdi);
}


struct syscall_info get_syscall_info(pid_t pid)
{
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1)
        fatal_e(E_RARE, "Can't read registers");

    struct syscall_info call = {
        .num = regs.orig_rax,
        .args = {
            regs.rdi,
            regs.rsi,
            regs.rdx,
            regs.r10,
            regs.r8,
            regs.r9,
        },
    };

    return call;
}
