#ifdef _WIN32
#    include <windows.h>
#endif // _WIN32

#include <locale.h>
#include <stdarg.h>
#include <stdio.h>

#include "logging.h"
#include "test.h"

#include <stdnoreturn.h>

typedef int (*test_run_f)(struct context);

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

static noreturn void failure_handler(struct context ctx, int lineno, const char *msg)
{
    write(indent(2),
          bullet(CROSSMARK, RED),
          error("Assertion on line %d failed: %s\n", lineno, msg));
    longjmp(*ctx.buf, 1);
}

static void warning_handler(struct context ctx, int lineno, const char *msg)
{
    write(indent(2),
          bullet(BLACK_CIRCLE, YELLOW),
          warn("Expectation on line %d failed: %s\n", lineno, msg));
}

void test_init()
{
#ifdef _WIN32
    enable_vt_mode();
    SetConsoleOutputCP(CP_UTF8);
#endif
    setlocale(LC_ALL, "en_US.UTF-8");
}

int test_run(const char *name, test_f test, struct context ctx)
{
    write(indent(1), bullet(DOTTED_CIRCLE, WHITE), format("%s", name));
    int fail = !setjmp(*ctx.buf) ? test(ctx), 0 : 1;
    if (fail) {
        write(CR, indent(1), bullet(CROSSMARK, RED), error("%s\n", name));
    } else {
        write(CR, indent(1), bullet(CHECKMARK, GREEN), format("%s\n", name));
    }
    return fail;
}

int test_run_collection(const char *name, ...)
{
    va_list    ap;
    test_run_f test;
    jmp_buf    error;

    int test_cnt = 0;
    int fail_cnt = 0;

    const struct context ctx = {
        .fail = failure_handler,
        .warn = warning_handler,
        .buf  = &error,
    };

    test_init(); // TODO: find better place

    write(bullet(PROMPT, CYAN), format(cyan("%s ") dim("(" __FILE__ ")") "\n", name));

    va_start(ap, name);
    while ((test = va_arg(ap, test_run_f)) != NULL) {
        test_cnt += 1;
        fail_cnt += test(ctx);
    }
    va_end(ap);

    fprintf(stdout, "\n");
    if (fail_cnt < test_cnt) {
        write(bullet(CHECKMARK, GREEN), success("%d passing\n", test_cnt - fail_cnt));
    }
    if (fail_cnt > 0) {
        write(bullet(CROSSMARK, RED), error("%d failing\n", fail_cnt));
    }

    return fail_cnt;
}
