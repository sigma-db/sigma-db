#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <stdio.h>
#include <stdarg.h>
#include <locale.h>

#include "test.h"
#include "logging.h"

#include <stdnoreturn.h>

typedef int (*test_run_f)(struct context);

#ifdef _WIN32
static int enable_vt_mode()
{
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

static noreturn void failure_handler(struct context ctx, int lineno, const char *msg)
{
    error("        %lc Assertion on line %d failed: %s\n", BLACK_CIRCLE, lineno, msg);
    longjmp(*ctx.buf, 1);
}

static void warning_handler(struct context ctx, int lineno, const char *msg)
{
    write(join(item(2, BLACK_CIRCLE, YELLOW), format("Expectation on line %d failed: %s\n", lineno, msg)));
}

int test_run(const char *name, test_f test, struct context ctx)
{
    log("%*s" dim("%lc %s"), 4, "", DOTTED_CIRCLE, name);
    int fail = !setjmp(*ctx.buf) ? test(ctx), 0 : 1;
    if (fail) {
        error("\r    %lc %s\n", CROSSMARK, name);
    } else {
        write_test_success("%s\n", name);
    }
    return fail;
}

int test_run_collection(const char *name, ...)
{
#ifdef _WIN32
    // TODO: Find better place
    enable_vt_mode();
    SetConsoleOutputCP(CP_UTF8);
#endif // _WIN32
    setlocale(LC_ALL, "en_US.UTF-8");

    log(cyan("%lc %s ") dim("(" __FILE__ ")") "\n", PROMPT, name);

    va_list ap;
    test_run_f test;
    jmp_buf error;

    int test_cnt = 0;
    int fail_cnt = 0;
    const struct context ctx = {
        .fail = failure_handler,
        .warn = warning_handler,
        .buf = &error,
    };

    va_start(ap, name);
    while ((test = va_arg(ap, test_run_f)) != NULL) {
        test_cnt += 1;
        fail_cnt += test(ctx);
    }
    va_end(ap);

    fprintf(stdout, "\n");
    if (fail_cnt < test_cnt) {
        success("%d passing\n", test_cnt - fail_cnt);
    }
    if (fail_cnt > 0) {
        error("%lc %d failing\n", CROSSMARK, fail_cnt);
    }

    return fail_cnt;
}
