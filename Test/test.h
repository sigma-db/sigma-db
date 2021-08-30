#include <stdlib.h>
#include <setjmp.h>

#define IMPORT(name) \
    void name()

#define EXPORT(name) \
    void name() \
    { \
        suite_ ## name(); \
    }

#define SUITE(name, ...) \
    static void collection_ ## name() \
    { \
        test_f tests[] = { __VA_ARGS__ }; \
        char names[] = #__VA_ARGS__; \
        test_run_collection(#name, names, tests); \
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
int test_run_collection(const char *coll_name, char test_names[], test_f tests[]);
