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

#ifndef ASSERT_H
#define ASSERT_H

#ifdef NDEBUG
#define DISABLE_ASSERTIONS
#endif // !_NDEBUG

#ifndef DISABLE_ASSERTIONS
#include <stdlib.h>

#ifndef ASSERTION_VERBOSITY_LEVEL
#define ASSERTION_VERBOSITY_LEVEL 1
#endif // !ASSERTION_VERBOSITY_LEVEL


#define stringify_(str) #str
#define stringify(str) stringify_(str)

#define cmd_builder_(prefix, suffix) prefix ## _ ## suffix
#define cmd_builder(prefix, suffix) cmd_builder_(prefix, suffix)
#define call(ns, cmd, ...) cmd_builder(ns, cmd)(__VA_ARGS__)


#define handler_custom(expr) expr
#define handler_ignore() ((void)0)
#define handler_default() abort()
#define handler_() handler_default()
#define call_handler(cmd, ...) call(handler, cmd, __VA_ARGS__)


#if ASSERTION_VERBOSITY_LEVEL > 0
#include <stdio.h>
#define print_detail(details) fprintf(stderr, "Assertion at %s:%d failed: %s.\n", __FILE__, __LINE__, details)
#define print_default() fprintf(stderr, "Assertion at %s:%d failed.\n", __FILE__, __LINE__)
#define print_(condition) message_default(condition)
#define call_print(type, ...) call(print, type, __VA_ARGS__)
#else
#define call_print(...) ((void)0)
#endif // !VERBOSE_ASSERTIONS


#define assertion(condition, message_type, message, handler_type, handler, ...) \
    do { if (!(condition)) { call_print(message_type, message); call_handler(handler_type, handler, __VA_ARGS__); } } while (0)


#define assert(condition) assertion(condition)

#define assert_eq(a, b, handler_type, handler) \
    assertion(a == b, detail, "Values are not equal (" stringify(a != b) ")", handler_type, handler)
#else
#define assertion(...) ((void)0)
#endif // !DISABLE_ASSERTIONS

#endif // !ASSERT_H

int test()
{
    assertion(1 == 2, detail, "not equal", custom, return -2);
    assert_eq(1, 9);
}
