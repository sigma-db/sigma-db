#include <stddef.h>

#define RUN(...) sigma_test_run_test_collection(__FILE__, __func__, __VA_ARGS__, NULL)

#define SUITE(name) void name(void)

#define TEST(name)                                                                                 \
    static void test_##name(struct config cfg);                                                    \
    static int  name(struct config cfg)                                                            \
    {                                                                                              \
        return sigma_test_run_test(#name, test_##name, cfg);                                       \
    }                                                                                              \
    static void test_##name(struct config cfg)

#define FAIL(msg) cfg.fail(cfg, __LINE__, msg)

#define WARN(msg) cfg.warn(cfg, __LINE__, msg)

#define ASSERT(cond)                                                                               \
    if (!(cond)) FAIL(#cond)

#define EXPECT(cond)                                                                               \
    if (!(cond)) WARN(#cond)

typedef void (*event_handler_f)(struct location, const char *);

struct config {
    event_handler_f fail;
    event_handler_f warn;
};

typedef void (*test_f)(struct config);

/**
 * Process forwarded command line arguments to configure the test engine
 */
int sigma_test_main(int argc, char *argv);

/**
 * Runs a test and returns 0 on success
 */
int sigma_test_run_test(const char *name, test_f test, struct config ctx);

/**
 * Runs a collection of tests and returns the number of failed tests
 */
int sigma_test_run_test_collection(const char *file_name, const char *suite_name, ...);
