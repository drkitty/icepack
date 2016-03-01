#include "common.h"

#include <stdbool.h>
#include <sys/types.h>
#include <sys/user.h>


void print_user_regs(struct user_regs_struct* regs);

struct syscall_info {
    unsigned int num;
    unsigned long long int args[6];
} get_syscall_info(pid_t pid);

bool get_user_string(pid_t pid, unsigned long long int addr, char* buf,
        ssize_t buflen);
