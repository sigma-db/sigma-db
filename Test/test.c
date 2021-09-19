#ifdef _WIN32
#    include <windows.h>
#endif // _WIN32

#include <locale.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../Common/args.h"

#include "logging.h"
#include "test.h"

#include <stdnoreturn.h>

#define suite_event(suite)                                                                         \
    (union event_args)                                                                             \
    {                                                                                              \
        .suite = suite                                                                             \
    }

#define test_event(suite, name, fail_cnt)                                                          \
    (union event_args)                                                                             \
    {                                                                                              \
        .test = { suite, name, fail_cnt }                                                          \
    }

#define assertion_event(test, lineno, message)                                                     \
    (union event_args)                                                                             \
    {                                                                                              \
        .assertion = { test, lineno, message }                                                     \
    }

typedef int (*test_run_f)(union event_args *);
typedef void (*report_f)(enum event_type, union event_args *);

enum event_type {
    SUITE_BEGIN,
    SUITE_END,
    TEST_BEGIN,
    TEST_END,
    ASSERTION_FAILURE,
};

struct suite {
    const char *name;
    const char *file_name;
    int         test_cnt;
    int         fail_cnt;
    jmp_buf *   buf;
};

struct test {
    struct suite *suite;
    const char *  name;
    int           fail_cnt;
};

struct assertion {
    struct test *test;
    int          lineno;
    const char * message;
};

union event_args {
    struct suite     suite;
    struct test      test;
    struct assertion assertion;
};

#ifdef _WIN32
static int enable_vt_mode();
#endif // _WIN32

static void          console_reporter(enum event_type type, union event_args *e);
static noreturn void on_fail(union event_args *e, int lineno, const char *msg);
static void          on_warn(union event_args *e, int lineno, const char *msg);

static report_f report = console_reporter;

int sigma_test_main(int argc, char *argv[])
{
    hashmap *args = args_parse(argc, argv);

    const char *reporter = hashmap_get_or_default(args, "reporter", "spec");

    if (strcmp(reporter, "spec") == 0) {
        report = console_reporter;
#ifdef _WIN32
        enable_vt_mode();
        SetConsoleOutputCP(CP_UTF8);
#endif
        setlocale(LC_ALL, "en_US.UTF-8");
    }
    return EXIT_SUCCESS;
}

int sigma_test_run_test(union event_args *e, const char *name, test_f test)
{
    union event_args args = test_event(&e->suite, name, 0);
    report(TEST_BEGIN, &args);
    int fail_cnt = !setjmp(*e->suite.buf) ? test(&args, on_fail, on_warn), 0 : args.test.fail_cnt;
    report(TEST_END, &args);
    return fail_cnt;
}

int sigma_test_run_test_collection(const char *file_name, const char *suite_name, ...)
{
    va_list    ap;
    test_run_f test;
    jmp_buf    error;

    struct suite suite = {
        .name      = suite_name,
        .file_name = file_name,
        .test_cnt  = 0,
        .fail_cnt  = 0,
        .buf       = &error,
    };
    union event_args args = suite_event(suite);

    report(SUITE_BEGIN, &args);

    va_start(ap, suite_name);
    while ((test = va_arg(ap, test_run_f)) != NULL) {
        suite.test_cnt += 1;
        suite.fail_cnt += test(&args) > 0;
    }
    va_end(ap);

    report(SUITE_END, &args);

    return suite.fail_cnt;
}

static noreturn void on_fail(union event_args *e, int lineno, const char *msg)
{
    e->test.fail_cnt += 1;
    report(ASSERTION_FAILURE, &assertion_event(&e->test, lineno, msg));
    longjmp(*e->test.suite->buf, 1);
}

static void on_warn(union event_args *e, int lineno, const char *msg)
{
    e->test.fail_cnt += 1;
    report(ASSERTION_FAILURE, &assertion_event(&e->test, lineno, msg));
}

static void console_reporter(enum event_type type, union event_args *e)
{
    switch (type) {
    case SUITE_BEGIN:
        write(bullet(PROMPT, CYAN),
              format(cyan("%s ") dim("(%s)") "\n", e->suite.name, e->suite.file_name));
        break;

    case SUITE_END:
        int test_cnt = e->suite.test_cnt;
        int fail_cnt = e->suite.fail_cnt;
        fprintf(stdout, "\n");
        if (fail_cnt < test_cnt) {
            write(bullet(CHECKMARK, GREEN), success("%d passing\n", test_cnt - fail_cnt));
        }
        if (fail_cnt > 0) {
            write(bullet(CROSSMARK, RED), error("%d failing\n", fail_cnt));
        }
        break;

    case TEST_BEGIN:
        write(indent(1), bullet(DOTTED_CIRCLE, WHITE), format("%s", e->test.name));
        break;

    case TEST_END:
        if (e->test.fail_cnt == 0) {
            write(CR, indent(1), bullet(CHECKMARK, GREEN), format("%s\n", e->test.name));
        }
        break;

    case ASSERTION_FAILURE:
        // Before printing the very first error message, we visually mark the test as failing
        if (e->assertion.test->fail_cnt == 1) {
            write(CR, indent(1), bullet(CROSSMARK, RED), error("%s\n", e->assertion.test->name));
        }
        write(
            indent(2),
            bullet(BULLET, YELLOW),
            warn("Expectation on line %d failed: %s\n", e->assertion.lineno, e->assertion.message));
        break;
    }
}

#ifdef _WIN32
static int enable_vt_mode()
{
    /*
     * Code taken from function `EnableVTMode` on
     * https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#example-of-select-anniversary-update-features
     */

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return -1;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return -2;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        return -3;
    }

    return 0;
}
#endif // _WIN32
