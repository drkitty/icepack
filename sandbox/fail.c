#include "fail.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


void vx_(const char* srcname, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fflush(stdout);
#ifdef DEBUG
    fprintf(stdout, "%s:%d: ", srcname, line);
#else
    (void)srcname; (void)line;
#endif
    vfprintf(stdout, format, args);
    putchar('\n');
}


void warning_(const char* srcname, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fflush(stdout);
#ifdef DEBUG
    fprintf(stderr, "%s:%d: ", srcname, line);
#else
    (void)srcname; (void)line;
#endif
    vfprintf(stderr, format, args);
    putc('\n', stderr);
}


void warning_e_(const char* srcname, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fflush(stdout);
#ifdef DEBUG
    fprintf(stderr, "%s:%d: ", srcname, line);
#else
    (void)srcname; (void)line;
#endif
    vfprintf(stderr, format, args);
    if (errno != 0)
        fprintf(stderr, " (%s)", strerror(errno));
    fputc('\n', stderr);
}


void fatal_(int rtn, const char* srcname, int line,
        const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fflush(stdout);
#ifdef DEBUG
    fprintf(stderr, "%s:%d: ", srcname, line);
#else
    (void)srcname; (void)line;
#endif
    vfprintf(stderr, format, args);
    putc('\n', stderr);
    exit(rtn);
}


void fatal_e_(int rtn, const char* srcname, int line,
        const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fflush(stdout);
#ifdef DEBUG
    fprintf(stderr, "%s:%d: ", srcname, line);
#else
    (void)srcname; (void)line;
#endif
    vfprintf(stderr, format, args);
    if (errno != 0)
        fprintf(stderr, " (%s)", strerror(errno));
    fputc('\n', stderr);
    exit(rtn);
}
