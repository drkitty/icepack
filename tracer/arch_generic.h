#include "common.h"

#include <sys/types.h>
#include <sys/user.h>


void print_user_regs(pid_t pid, struct user_regs_struct* regs);


struct syscall_info {
    unsigned int num;
    unsigned long long int args[6];
} get_syscall_info(pid_t pid);
