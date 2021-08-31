#include <stdlib.h>
#include <setjmp.h>

#define RUN(...) \
    test_run_collection(__func__, __VA_ARGS__, NULL)

#define SUITE(name) \
    void name(void)

#define TEST(name) \
    static void name(struct context ctx)

#define EXPECT(cond) \
    if (!(cond)) ctx.fail(ctx, __func__, __LINE__, #cond)

struct context {
    void (*fail)(struct context, const char *, size_t, const char *);
    jmp_buf *buf;
};

typedef void (*test_f)(struct context);

/**
 * Runs a collection of tests and returns the number of failed tests
 */
int test_run_collection(const char *name, ...);
