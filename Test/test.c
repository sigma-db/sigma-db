#ifdef _WIN32
#    include <windows.h>
#endif // _WIN32

#include <locale.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

#include "logging.h"
#include "test.h"

#include <stdnoreturn.h>

struct location {
    const char *file_name;
    const char *suite_name;
    const char *test_name; // only required when inside a test function
    int         lineno;    // only required for assertions/expectations
};

typedef int (*test_run_f)(struct config);
typedef void (*reporter_f)(struct location *sender, struct event_args);

enum event_type {
    SUITE_BEGIN,
    SUITE_END,
    TEST_BEGIN,
    TEST_FAILURE,
    TEST_END,
};

struct event_args {
    enum event_type type;
    union {
        struct {
            int test_cnt;
            int fail_cnt;
        } as_suite_end;

        struct {
            int         fail_cnt;
            const char *message;
        } as_test_failure;

        struct {
            const char *name;
            int         fail_cnt;
        } as_test_end;
    };
};

struct context {
    event_handler_f fail;
    event_handler_f warn;
};

#ifdef _WIN32
static int enable_vt_mode();
#endif // _WIN32

static void console_reporter(struct event_args e, struct location *loc);

static jmp_buf    error;
static reporter_f report;

static noreturn void failure_handler(struct location src, const char *desc)
{
    longjmp(error, 1);
}

static void warning_handler(struct location src, const char *msg)
{
    report((struct event_args){.type = TEST_FAILURE, .as_test_failure = {.message = msg}}, &src);
}

int sigma_test_main(int argc, char *argv)
{
#ifdef _WIN32
    enable_vt_mode();
    SetConsoleOutputCP(CP_UTF8);
#endif
    setlocale(LC_ALL, "en_US.UTF-8");

    report = console_reporter;
}

int sigma_test_run_test(const char *name, test_f test, struct config ctx)
{
    struct location loc = {.test_name = name};
    report((struct event_args){.type = TEST_BEGIN}, &loc);
    int fail = !setjmp(error) ? test(ctx), 0 : 1;
    report((struct event_args){.type = TEST_END, .as_test_end = {.fail_cnt = fail}}, &loc);
    return fail;
}

int sigma_test_run_test_collection(const char *file_name, const char *suite_name, ...)
{
    va_list    ap;
    test_run_f test;
    // jmp_buf    error;

    int test_cnt = 0;
    int fail_cnt = 0;

    struct config ctx = {
        .fail = failure_handler,
        .warn = warning_handler,
        //.buf  = &error,
    };
    struct location loc = {
        .file_name  = file_name,
        .suite_name = suite_name,
    };

    report((struct event_args){.type = SUITE_BEGIN}, &loc);

    va_start(ap, suite_name);
    while ((test = va_arg(ap, test_run_f)) != NULL) {
        test_cnt += 1;
        fail_cnt += test(ctx);
    }
    va_end(ap);

    report((struct event_args){.type         = SUITE_END,
                               .as_suite_end = {.test_cnt = test_cnt, .fail_cnt = fail_cnt}},
           &loc);

    return fail_cnt;
}

struct test {
    const char *name;
    int         test_cnt;
    int         fail_cnt;
};
struct suite {
    const char *name;
};

static void console_reporter(struct location *ctx, struct event_args e)
{
    switch (e.type) {
    case SUITE_BEGIN:
        write(bullet(PROMPT, CYAN),
              format(cyan("%s ") dim("(%s)") "\n", ctx->suite_name, ctx->file_name));
        break;

    case SUITE_END:
        int test_cnt = e.as_suite_end.test_cnt;
        int fail_cnt = e.as_suite_end.fail_cnt;
        fprintf(stdout, "\n");
        if (fail_cnt < test_cnt) {
            write(bullet(CHECKMARK, GREEN), success("%d passing\n", test_cnt - fail_cnt));
        }
        if (fail_cnt > 0) {
            write(bullet(CROSSMARK, RED), error("%d failing\n", fail_cnt));
        }
        break;

    case TEST_BEGIN:
        write(indent(1), bullet(DOTTED_CIRCLE, WHITE), format("%s", ctx->test_name));
        break;

    case TEST_FAILURE:
        // Before printing the *first* error message, we visually mark the test case as failing
        if (e.as_test_failure.fail_cnt == 1) {
            write(CR, indent(1), bullet(CROSSMARK, RED), error("%s\n", ctx->test_name));
        }
        write(indent(2),
              bullet(BLACK_CIRCLE, YELLOW),
              warn("Expectation on line %d failed: %s\n", ctx->lineno, e.as_test_failure.message));
        break;

    case TEST_END:
        if (e.as_test_end.fail_cnt == 0) {
            write(CR, indent(1), bullet(CHECKMARK, GREEN), format("%s\n", ctx->test_name));
        }
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
