#include <stdlib.h>
#include <setjmp.h>

#define RUN(...) \
    test__print_result(__func__, test__run_collection(__func__, __VA_ARGS__, NULL))

#define SUITE(name) \
    void name(void)

#define TEST(name) \
    static void test_ ## name(struct context ctx);  \
    static inline int name(struct context ctx) \
    { \
        return test__run(#name, test_ ## name, ctx); \
    } \
    static void test_ ## name(struct context ctx)

#define FAIL(msg) \
    ctx.fail(ctx, __LINE__, msg)

#define EXPECT(cond) \
    if (!(cond)) FAIL(#cond)

struct context {
    void (*fail)(struct context, int, const char *);
    jmp_buf *buf;
};

typedef void (*test_f)(struct context);

/**
 * Runs a test and returns 0 on success
 */
int test__run(const char *name, test_f test, struct context ctx);

/**
 * Runs a collection of tests and returns the number of failed tests
 */
int test__run_collection(const char *name, ...);

/**
 * Prints the result to stdout
 */
void test__print_result(const char *name, int fail_cnt);
