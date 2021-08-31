#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "test.h"

static _Noreturn void failure_handler(struct context ctx, const char *name, size_t lineno, const char *msg)
{
    fprintf(stderr, "Test \"%s\" on line %zu failed: %s.\n", name, lineno, msg);
    longjmp(*ctx.buf, 1);
}

int test_run_collection(const char *name, ...)
{
    va_list ap;
    test_f test;
    jmp_buf error;

    size_t fail_cnt = 0;
    struct context ctx = {
        .fail = failure_handler,
        .buf = &error,
    };

    va_start(ap, name);
    while ((test = va_arg(ap, test_f)) != NULL) {
        if (!setjmp(error)) {
            test(ctx);
        } else {
            fail_cnt += 1;
        }
    }
    va_end(ap);

    return fail_cnt;
}

void test_print_result(const char *name, size_t fail_cnt)
{
    printf("\n");
    if (fail_cnt == 0) {
        printf("All tests in collection \"%s\" succeeded.\n", name);
    } else {
        const char *test_num = fail_cnt == 1 ? "test" : "tests";
        printf("%zu %s in collection \"%s\" failed.\n", fail_cnt, test_num, name);
    }
}
