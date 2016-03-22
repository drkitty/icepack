#include "common.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fail.h"


// void fork_prog(pid_t* pid, const char* prog, ...);

#define fork_prog(pid, prog, ...) do { \
    *pid = fork(); \
    if (*pid != 0) \
        break; \
    execlp(prog, __VA_ARGS__); \
    *pid = 0; \
    fatal_e(E_RARE, "Can't exec %s", prog); \
} while (false)


// void wait_prog(pid_t* pid, int* status);

#define wait_prog(pid, status) do { \
    if (waitpid(*pid, status, 0) == -1) { \
        *pid = 0; \
        fatal_e(E_RARE, "Can't wait"); \
    } \
    if (WIFEXITED(*status)) { \
        *pid = 0; \
        *status = WEXITSTATUS(*status); \
        break; \
    } else if (WIFSIGNALED(*status)) { \
        *pid = 0; \
        *status = -WTERMSIG(*status); \
        break; \
    } \
} while (true)


extern pid_t child_pid;
