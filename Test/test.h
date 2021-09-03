#include <stddef.h>
#include <setjmp.h>

#define RUN(...) \
    test_run_collection(__func__, __VA_ARGS__, NULL)

#define SUITE(name) \
    void name(void)

#define TEST(name) \
    static void test_ ## name(struct context ctx); \
    static int name(struct context ctx) \
    { \
        return test_run(#name, test_ ## name, ctx); \
    } \
    static void test_ ## name(struct context ctx)

#define FAIL(msg) \
    ctx.fail(ctx, __LINE__, msg)

#define WARN(msg) \
    ctx.warn(ctx, __LINE__, msg)

#define ASSERT(cond) \
    if (!(cond)) FAIL(#cond)

#define EXPECT(cond) \
    if (!(cond)) WARN(#cond)

struct context {
    void (*fail)(struct context, int, const char *);
    void (*warn)(struct context, int, const char *);
    jmp_buf *buf;
};

typedef void (*test_f)(struct context);

/**
 * Runs a test and returns 0 on success
 */
int test_run(const char *name, test_f test, struct context ctx);

/**
 * Runs a collection of tests and returns the number of failed tests
 */
int test_run_collection(const char *name, ...);
