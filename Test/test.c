#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "test.h"

static _Noreturn void failure_cb(struct context ctx, const char *msg, size_t lineno)
{
    fprintf(stderr, "Test \"%s\" on line %zu failed: %s.\n", ctx.name, lineno, msg);
    longjmp(*ctx.buf, 1);
}

int test_run_collection(const char *coll_name, const char *test_names, ...)
{
    jmp_buf error;
    va_list ap;

    size_t test_cnt = 0;
    size_t fail_cnt = 0;

    /* Copy test_names into a modifiable string (for strtok) */
    size_t len = strlen(test_names) + 1;
    char *names = malloc(len);
    if (names == NULL) {
        fprintf(stderr, "Could not allocate memory to initialise collection \"%s\".\n", coll_name);
        return -1;
    }
    strcpy_s(names, len, test_names);

    va_start(ap, test_names);

    /* Iterate the test names provided as a comma - separated list `test_names`
     * and the respective function pointers provided as varargs
     */
    char *ctx = NULL;
    char *name = strtok_s(names, ", ", &ctx);
    while (name != NULL) {
        test_cnt += 1;
        if (!setjmp(error)) {
            struct context ctx = { .fail = failure_cb, .name = name, .buf = &error };
            va_arg(ap, test_f)(ctx);
        } else {
            fail_cnt += 1;
        }
        name = strtok_s(NULL, ", ", &ctx);
    }

    va_end(ap);

    printf("\n");
    if (fail_cnt == 0) {
        printf("All tests in collection \"%s\" succeeded.\n", coll_name);
    } else {
        const char *num = fail_cnt == 1 ? "test" : "tests";
        printf("%zu %s in collection \"%s\" failed.\n", fail_cnt, num, coll_name);
    }

    return fail_cnt;
}
