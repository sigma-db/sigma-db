#include <stdlib.h>
#include <setjmp.h>

#define IMPORT(name) \
    void name()

#define EXPORT(name, ...) \
    void name() \
    { \
        test_run_collection(#name, #__VA_ARGS__, __VA_ARGS__); \
    }

#define TEST(name) \
    static void name(struct context ctx)

#define EXPECT(cond) \
    do { \
        if (!(cond)) ctx.fail(ctx, #cond, __LINE__); \
    } while (0)

struct context {
    void (*fail)(struct context, const char *, size_t);
    const char *name;
    jmp_buf *buf;
};

typedef void (*test_f)(struct context);

/**
 * Runs a collection of tests and returns the number of failed tests
 */
int test_run_collection(const char *coll_name, const char *test_names, ...);
