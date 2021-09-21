/*
 * MIT License
 *
 * Copyright (c) 2021 David Luis Wiegandt
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SIGMA_TEST_H
#define SIGMA_TEST_H

#ifdef _WIN32
#    include <windows.h>
#endif // _WIN32

#include <locale.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Intensities
#define BRIGHT 1
#define DIM    2
#define NORMAL 22

// Colours
#define BLACK   30
#define RED     31
#define GREEN   32
#define YELLOW  33
#define BLUE    34
#define MAGENTA 35
#define CYAN    36
#define WHITE   37
#define DEFAULT 39

// Helpers for constructing ANSI escape sequences
#define stringize(expr) #expr
#define escape(code)    "\x1b[" stringize(code) "m"

// Intensity Setters
#define bright(string)       escape(BRIGHT) string escape(NORMAL)
#define dim(string)          escape(DIM) string escape(NORMAL)
#define colour(code, string) escape(code) string escape(DEFAULT)

// Colour Setters
#define black(string)   colour(BLACK, string)
#define red(string)     colour(RED, string)
#define green(string)   colour(GREEN, string)
#define yellow(string)  colour(YELLOW, string)
#define blue(string)    colour(BLUE, string)
#define magenta(string) colour(MAGENTA, string)
#define cyan(string)    colour(CYAN, string)
#define white(string)   colour(WHITE, string)

// Unicode Characters
#define DOTTED_CIRCLE 0x25CC
#define BULLET        0x2022
#define CHECKMARK     0x2713
#define CROSSMARK     0x2717
#define PROMPT        0x276F

// Formatting Constants
#define INDENT 4

/* clang-format off */
#define va_cnt_(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define va_cnt(...) va_cnt_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)
/* clang-format on */

#define mangle__(name, arity) name##arity
#define mangle_(name, arity)  mangle__(name, arity)
#define mangle(name, ...)     mangle_(name, va_cnt(__VA_ARGS__))

#define head(hd, ...) hd
#define tail(hd, ...) __VA_ARGS__

#define unpack_(...) __VA_ARGS__
#define unpack(args) unpack_ args

#define join2(a, b)   (head a head b, tail a, tail b)
#define join3(a, ...) join2(a, join2(__VA_ARGS__))
#define join4(a, ...) join2(a, join3(__VA_ARGS__))
#define join5(a, ...) join2(a, join4(__VA_ARGS__))
#define join6(a, ...) join2(a, join5(__VA_ARGS__))
#define join7(a, ...) join2(a, join6(__VA_ARGS__))
#define join8(a, ...) join2(a, join7(__VA_ARGS__))

#define join(...)        mangle(join, __VA_ARGS__)(__VA_ARGS__)
#define write(...)       fprintf(stdout, unpack(join(__VA_ARGS__)))
#define string(dst, ...) sprintf(dst, unpack(join(__VA_ARGS__)))

// Predefined Templates
#define format(fmt, ...)         (fmt, __VA_ARGS__)
#define bullet(chcode, chcolour) (colour(chcolour, "%lc "), chcode)
#define indent(depth)            ("%*s", depth * INDENT, "")

// Predefined Constant Templates
#define CR format("%c", '\r')

#define error(fmt, ...)   (red(fmt), __VA_ARGS__)
#define success(fmt, ...) (green(fmt), __VA_ARGS__)
#define warn(fmt, ...)    (yellow(fmt), __VA_ARGS__)
#define info(fmt, ...)    (white(fmt), __VA_ARGS__)

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

typedef void (*test_f)(union event_args *);
typedef int (*test_call_f)(union event_args *);
typedef int (*suite_f)(void);

#define FAIL(e, msg)                                                                               \
    do {                                                                                           \
        e->test.fail_cnt += 1;                                                                     \
        report(ASSERTION_FAILURE, &assertion_event(&e->test, __LINE__, msg));                      \
        longjmp(*e->test.suite->buf, 1);                                                           \
    } while (0)

#define WARN(e, msg)                                                                               \
    do {                                                                                           \
        e->test.fail_cnt += 1;                                                                     \
        report(ASSERTION_FAILURE, &assertion_event(&e->test, __LINE__, msg));                      \
    } while (0)

#define ASSERT(cond)                                                                               \
    if (!(cond)) FAIL(e, #cond)

#define EXPECT(cond)                                                                               \
    if (!(cond)) WARN(e, #cond)

#ifdef _WIN32
#    define sigma_test_setup_console()                                                             \
        do {                                                                                       \
            /*                                                                                     \
             * Code taken from function `EnableVTMode` on                                          \
             * https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences \
             */                                                                                    \
                                                                                                   \
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);                                         \
            if (hOut == INVALID_HANDLE_VALUE) {                                                    \
                return -1;                                                                         \
            }                                                                                      \
                                                                                                   \
            DWORD dwMode = 0;                                                                      \
            if (!GetConsoleMode(hOut, &dwMode)) {                                                  \
                return -2;                                                                         \
            }                                                                                      \
                                                                                                   \
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;                                          \
            if (!SetConsoleMode(hOut, dwMode)) {                                                   \
                return -3;                                                                         \
            }                                                                                      \
                                                                                                   \
            SetConsoleOutputCP(CP_UTF8);                                                           \
        } while (0)
#else
#    define sigma_test_setup_console()
#endif

static void report(enum event_type type, union event_args *e)
{
    switch (type) {
    case SUITE_BEGIN:
        write(bullet(PROMPT, CYAN),
              format(cyan("%s ") dim("(%s)") "\n", e->suite.name, e->suite.file_name));
        break;

    case SUITE_END:
        fprintf(stdout, "\n");
        int test_cnt = e->suite.test_cnt;
        int fail_cnt = e->suite.fail_cnt;
        if (fail_cnt < test_cnt) {
            write(bullet(CHECKMARK, GREEN), success("%d passing\n", test_cnt - fail_cnt));
        }
        if (fail_cnt > 0) {
            write(bullet(CROSSMARK, RED), error("%d failing\n", fail_cnt));
        }
        break;

    case TEST_BEGIN:
        write(indent(1), bullet(DOTTED_CIRCLE, WHITE), format("%s\r", e->test.name));
        break;

    case TEST_END:
        if (e->test.fail_cnt == 0) {
            write(indent(1), bullet(CHECKMARK, GREEN), format("%s\n", e->test.name));
        }
        break;

    case ASSERTION_FAILURE:
        // Before printing the very first error message, we visually mark the test as failing
        if (e->assertion.test->fail_cnt == 1) {
            write(indent(1), bullet(CROSSMARK, RED), error("%s\n", e->assertion.test->name));
        }
        write(
            indent(2),
            bullet(BULLET, YELLOW),
            warn("Expectation on line %d failed: %s\n", e->assertion.lineno, e->assertion.message));
        break;
    }
}

/**
 * Runs a test and returns 0 on success
 */
#define TEST(name)                                                                                 \
    static void test_##name(union event_args *e);                                                  \
    static int  name(union event_args *e)                                                          \
    {                                                                                              \
        union event_args args = test_event(&e->suite, #name, 0);                                   \
        report(TEST_BEGIN, &args);                                                                 \
        if (!setjmp(*e->suite.buf)) {                                                              \
            test_##name(&args);                                                                    \
        }                                                                                          \
        report(TEST_END, &args);                                                                   \
        return args.test.fail_cnt;                                                                 \
    }                                                                                              \
    static void test_##name(union event_args *e)

/**
 * Runs a collection of tests and returns the number of failed tests
 */
#define SUITE(suite_name, ...)                                                                     \
    int suite_name()                                                                               \
    {                                                                                              \
        test_call_f tests[] = {__VA_ARGS__};                                                       \
        jmp_buf     error;                                                                         \
                                                                                                   \
        struct suite suite = {                                                                     \
            .name      = #suite_name,                                                              \
            .file_name = __FILE__,                                                                 \
            .test_cnt  = 0,                                                                        \
            .fail_cnt  = 0,                                                                        \
            .buf       = &error,                                                                   \
        };                                                                                         \
        union event_args args = suite_event(suite);                                                \
                                                                                                   \
        report(SUITE_BEGIN, &args);                                                                \
        for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {                               \
            args.suite.test_cnt += 1;                                                              \
            args.suite.fail_cnt += tests[i](&args) > 0;                                            \
        }                                                                                          \
        report(SUITE_END, &args);                                                                  \
                                                                                                   \
        return suite.fail_cnt;                                                                     \
    }

/**
 * Executes all test suites and outputs their results
 */
#define SIGMA_TEST_MAIN(...)                                                                       \
    int main(int argc, char *argv[])                                                               \
    {                                                                                              \
        sigma_test_setup_console();                                                                \
        setlocale(LC_ALL, "en_US.UTF-8");                                                          \
        suite_f suites[] = {__VA_ARGS__};                                                          \
        for (int i = 0; i < sizeof(suites) / sizeof(suites[0]); i++) {                             \
            suites[i]();                                                                           \
        }                                                                                          \
        return EXIT_SUCCESS;                                                                       \
    }

#endif // !SIGMA_TEST_H
