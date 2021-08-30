#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "test.h"

static _Noreturn void failure_cb(struct context ctx, const char *msg, size_t lineno)
{
    fprintf(stderr, "Test \"%s\" on line %zu failed: %s.\n", ctx.name, lineno, msg);
    longjmp(*ctx.buf, 1);
}

int test_run_collection(const char *coll_name, char test_names[], test_f tests[])
{
    jmp_buf error;

    size_t test_cnt = 0;
    size_t fail_cnt = 0;

    /* Iterate the test names provided as a comma-separated list `test_names`
     * and the respective function pointers provided as varargs
     */
    char *ctx = NULL;
    char *name = strtok_s(test_names, ", ", &ctx);
    while (name != NULL) {
        if (!setjmp(error)) {
            struct context ctx = { 
                .name = name,
                .fail = failure_cb,
                .buf = &error,
            };
            tests[test_cnt++](ctx);
        } else {
            fail_cnt += 1;
        }
        name = strtok_s(NULL, ", ", &ctx);
    }

    printf("\n");
    if (fail_cnt == 0) {
        printf("All tests in collection \"%s\" succeeded.\n", coll_name);
    } else {
        const char *num = fail_cnt == 1 ? "test" : "tests";
        printf("%zu %s in collection \"%s\" failed.\n", fail_cnt, num, coll_name);
    }

    return fail_cnt;
}
