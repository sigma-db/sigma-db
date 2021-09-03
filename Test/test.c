#include <stdarg.h>

#include "test.h"

#ifndef SIGMA_TEST_SILENT
#include <stdio.h>
#include <string.h>
#else
#endif // !SIGMA_TEST_SILENT

#include <stdnoreturn.h>

#define DIM 2
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define WHITE 37

#define stringize(expr) #expr
#define color(code, string) "\x1b[" stringize(code) "m" string "\x1b[0m"

#define error(format, ...) fprintf(stdout, color(RED, format), __VA_ARGS__)
#define success(format, ...) fprintf(stdout, color(GREEN, format), __VA_ARGS__)
#define warn(format, ...) fprintf(stdout, color(YELLOW, format), __VA_ARGS__)
#define info(format, ...) fprintf(stdout, color(WHITE, format), __VA_ARGS__)

#define CHECKMARK color(GREEN, u8"\u2713")
#define CROSSMARK color(RED, u8"\u2717")

typedef int (*test_run_f)(struct context);

static noreturn void failure_handler(struct context ctx, int lineno, const char *msg)
{
    error("\n    Assertion on line %d failed: %s.\n", lineno, msg);
    longjmp(*ctx.buf, 1);
}

static void warning_handler(struct context ctx, int lineno, const char *msg)
{
    warn("\n      - Expectation on line %d failed: %s.\n", lineno, msg);
}

int test_run(const char *name, test_f test, struct context ctx)
{
    info("  > %s", name);
    int fail = !setjmp(*ctx.buf) ? test(ctx), 0 : 1;
    if (fail) {
        error(u8"\r  " CROSSMARK " %s\n", name);
    } else {
        info(u8"\r  " CHECKMARK " %s\n", name);
    }
    return fail;
}

int test_run_collection(const char *name, ...)
{
    info("\x1b[47m\x1b[30m SUITE \x1b[0m %s\n", name);

    va_list ap;
    test_run_f test;
    jmp_buf error;

    int test_cnt = 0;
    int fail_cnt = 0;
    struct context ctx = {
        .fail = failure_handler,
        .warn = warning_handler,
        .buf = &error,
    };

    va_start(ap, name);
    while ((test = va_arg(ap, test_run_f)) != NULL) {
        test_cnt += 1;
        fail_cnt += test(ctx);
    }
    va_end(ap);

    success("\n%d passing\n", test_cnt - fail_cnt);
    error("%d failing\n", fail_cnt);

    return fail_cnt;
}
