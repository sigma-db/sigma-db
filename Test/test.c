#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "test.h"

static _Noreturn void failure_cb(struct context ctx, const char *name, size_t lineno, const char *msg)
{
    fprintf(stderr, "Test \"%s\" on line %zu failed: %s.\n", name, lineno, msg);
    longjmp(*ctx.buf, 1);
}

int test_run_collection(const char *name, ...)
{
    jmp_buf error;
    struct context ctx = {
        .fail = failure_cb,
        .buf = &error,
    };

    va_list ap;
    va_start(ap, name);

    size_t fail_cnt = 0;
    test_f test = va_arg(ap, test_f);
    while (test != NULL) {
        if (!setjmp(error)) {
            test(ctx);
        } else {
            fail_cnt += 1;
        }
        test = va_arg(ap, test_f);
    }

    printf("\n");
    if (fail_cnt == 0) {
        printf("All tests in collection \"%s\" succeeded.\n", name);
    } else {
        const char *num = fail_cnt == 1 ? "test" : "tests";
        printf("%zu %s in collection \"%s\" failed.\n", fail_cnt, num, name);
    }

    return fail_cnt;
}
