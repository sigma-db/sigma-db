#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <stdio.h>
#include <stdarg.h>
#include <locale.h>

#include "test.h"

#include <stdnoreturn.h>

// Attributes
#define BRIGHT 1
#define DIM 2
#define NORMAL 22

// Colors
#define BLACK 31
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37
#define DEFAULT 39

// Unicode Characters
#define DOTTED_CIRCLE 0x25CC
#define BLACK_CIRCLE 0x25CF
#define CHECKMARK 0x2713
#define CROSSMARK 0x2717
#define PROMPT 0x276F

#define stringize(expr) #expr
#define escape(code) "\x1b[" stringize(code) "m"

#define bright(string) escape(BRIGHT) string escape(NORMAL)
#define dim(string) escape(DIM) string escape(NORMAL)
#define colour(code, string) escape(code) string escape(DEFAULT)

#define black(string) colour(BLACK, string)
#define red(string) colour(RED, string)
#define green(string) colour(GREEN, string)
#define yellow(string) colour(YELLOW, string)
#define blue(string) colour(BLUE, string)
#define magenta(string) colour(MAGENTA, string)
#define cyan(string) colour(CYAN, string)
#define white(string) colour(WHITE, string)

#define log(fmt, ...) fprintf(stdout, fmt, __VA_ARGS__)
#define bullet(ch, col) log()
#define item(level, bullet, fmt, ...) log("%*s%lc " fmt, level * 4, "", bullet, __VA_ARGS__)

#define error(format, ...) log(red(format), __VA_ARGS__)
#define success(format, ...) log(green(format), __VA_ARGS__)
#define warn(format, ...) log(yellow(format), __VA_ARGS__)
#define info(format, ...) log(white(format), __VA_ARGS__)

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
        return -1;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        return -1;
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
    item(2, BLACK_CIRCLE, yellow("Expectation on line %d failed: %s\n"), lineno, msg);
}

int test_run(const char *name, test_f test, struct context ctx)
{
    log("%*s" dim("%lc %s"), 4, "", DOTTED_CIRCLE, name);
    int fail = !setjmp(*ctx.buf) ? test(ctx), 0 : 1;
    if (fail) {
        error("\r    %lc %s\n", CROSSMARK, name);
    } else {
        info("\r    " green("%lc") " %s\n", CHECKMARK, name);
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
        suite_success("%d passing\n", test_cnt - fail_cnt);
    }
    if (fail_cnt > 0) {
        error("%lc %d failing\n", CROSSMARK, fail_cnt);
    }

    return fail_cnt;
}
