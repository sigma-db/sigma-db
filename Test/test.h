#include <stddef.h>

#define RUN(...) sigma_test_run_test_collection(__FILE__, __func__, __VA_ARGS__, NULL)

#define SUITE(name) void name(void)

#define TEST(name)                                                                                 \
    static void test_##name(struct config cfg);                                                    \
    static int  name(union event_args *e)                                                          \
    {                                                                                              \
        return sigma_test_run_test(e, #name, test_##name);                                         \
    }                                                                                              \
    static void test_##name(union event_args *e, event_handler_f fail, event_handler_f warn)

#define FAIL(msg) fail(e, __LINE__, msg)

#define WARN(msg) warn(e, __LINE__, msg)

#define ASSERT(cond)                                                                               \
    if (!(cond)) FAIL(#cond)

#define EXPECT(cond)                                                                               \
    if (!(cond)) WARN(#cond)

enum event_type;
union event_args;

typedef void (*event_handler_f)(union event_args *, int, const char *);
typedef void (*test_f)(union event_args *, event_handler_f, event_handler_f);

/**
 * Process forwarded command line arguments to configure the test engine
 */
int sigma_test_main(int argc, char *argv);

/**
 * Runs a test and returns 0 on success
 */
int sigma_test_run_test(union event_args *e, const char *name, test_f test);

/**
 * Runs a collection of tests and returns the number of failed tests
 */
int sigma_test_run_test_collection(const char *file_name, const char *suite_name, ...);
