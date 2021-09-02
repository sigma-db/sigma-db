#include <stdarg.h>

#include "test.h"

#ifndef SIGMA_TEST_SILENT
#include <stdio.h>
#else
#endif // !SIGMA_TEST_SILENT

#define error(format, ...) fprintf(stdout, "\x1b[31m" format "\x1b[0m", __VA_ARGS__)
#define success(format, ...) fprintf(stdout, "\x1b[32m" format "\x1b[0m", __VA_ARGS__)
#define warn(format, ...) fprintf(stdout, "\x1b[33m" format "\x1b[0m", __VA_ARGS__)
#define info(format, ...) fprintf(stdout, format, __VA_ARGS__)

typedef int (*test_run_f)(struct context);

static _Noreturn void failure_handler(struct context ctx, int lineno, const char *msg)
{
    error("\n    Expectation on line %d failed: %s.\n", lineno, msg);
    longjmp(*ctx.buf, 1);
}

int test__run(const char *name, test_f test, struct context ctx)
{
    info(" > %s", name);
    int fail = !setjmp(*ctx.buf) ? test(ctx), 0 : 1;
    if (fail) {
        error("\r * %s", name);
    } else {
        info("\r * %s", name);
    }
    printf("\n");
    return fail;
}

int test__run_collection(const char *name, ...)
{
    info("%s\n", name);

    va_list ap;
    test_run_f test;
    jmp_buf error;

    int fail_cnt = 0;
    struct context ctx = {
        .fail = failure_handler,
        .buf = &error,
    };

    va_start(ap, name);
    while ((test = va_arg(ap, void *)) != NULL) {
        fail_cnt += test(ctx);
    }
    va_end(ap);

    return fail_cnt;
}

void test__print_result(const char *name, int fail_cnt)
{
    printf("\n");
    if (fail_cnt == 0) {
        success("All tests in collection \"%s\" succeeded.\n", name);
    } else {
        const char *test_num = fail_cnt == 1 ? "test" : "tests";
        error("%d %s in collection \"%s\" failed.\n", fail_cnt, test_num, name);
    }
}
