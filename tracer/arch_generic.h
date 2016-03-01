#include "common.h"

#include <sys/types.h>
#include <sys/user.h>


void print_user_regs(pid_t pid, struct user_regs_struct* regs);
