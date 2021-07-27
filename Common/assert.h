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

#define ENABLE_ASSERTIONS 1
#define NOOP ((void)0)
#define ERROR_CODE -1

#ifndef ENABLE_ASSERTIONS_IN_RELEASE
#   define ENABLE_ASSERTIONS_IN_RELEASE 0
#endif // !ENABLE_ASSERTIONS_IN_RELEASE

#ifdef NDEBUG
#   define ENABLE_ASSERTIONS ENABLE_ASSERTIONS_IN_RELEASE
#endif


#if ENABLE_ASSERTIONS
#include <stdlib.h>

#ifndef ASSERTION_LEVEL
#ifdef NDEBUG
#   define ASSERTION_LEVEL 1
#else
#   define ASSERTION_LEVEL 2
#endif // NDEBUG
#endif // !ASSERTION_LEVEL


#define stringify_(str) #str
#define stringify(str) stringify_(str)

#define cmd_builder_(ns, cmd) ns ## _ ## cmd
#define cmd_builder(ns, cmd) cmd_builder_(ns, cmd)
#define call(ns, cmd, ...) cmd_builder(ns, cmd)(__VA_ARGS__)


#if ASSERTION_LEVEL > 0
#include <stdio.h> 
#define MESSAGE_PREFIX "Assertion at " __FILE__ ":" stringify(__LINE__) " failed" 
#define print_custom(details) fprintf(stderr, MESSAGE_PREFIX ": " details ".\n")
#define print_generic() fprintf(stderr, MESSAGE_PREFIX ".\n")
#define print_default() print_generic()
#define print_() print_default()
#define call_print(type, ...) call(print, type, __VA_ARGS__)
#else
#define call_print(...) NOOP
#endif // !ASSERTION_LEVEL


#define handler_custom(expr) expr
#define handler_return(code) return code
#define handler_abort() abort()
#define handler_ignore() NOOP
#if ASSERTION_LEVEL > 1
#   define handler_default(...) handler_abort()
#else
#   define handler_default(...) handler_ignore()
#endif
#define handler_(...) handler_default(__VA_ARGS__)
#define call_handler(cmd, ...) call(handler, cmd, __VA_ARGS__)


#define assertion(condition, message_type, message, handler_type, handler) \
    do { if (!(condition)) { call_print(message_type, message); call_handler(handler_type, handler); } } while (0)
#else
#   define assertion(...) NOOP
#endif // !ENABLE_ASSERTIONS


#ifndef DISABLE_DEFAULT_ASSERTIONS
#define assert_true(predicate, handler_type, handler) \
    assertion(predicate, default, NULL, handler_type, handler)

#define assert(predicate, handler_type, handler) \
    assert_true(predicate, handler_type, handler)

#define assert_eq(a, b, handler_type, handler) \
    assertion(a == b, custom, "Values are not equal (" stringify(a != b) ")", handler_type, handler)

#define assert_within_bounds(inf, x, sup, handler_type, handler) \
    assertion(inf <= x && x <= sup, custom, "Value " #x " is not in range [" #inf ", " #sup ")", handler_type, handler)

#define assert_not_null(expr, handler_type, handler) \
    assertion(expr != NULL, custom, #expr " is NULL", handler_type, handler)

#define assert_fail(message, handler) \
    assertion(0, custom, message, custom, handler)
#endif // !DISABLE_DEFAULT_ASSERTIONS

#endif // !ASSERT_H

/* 
 * The general syntax to invoke assertions is as follows:
 * 
 *   assert(<test>, <cmd>, <cmd>, ...)
 * 
 * <test> can be any boolean expression, i.e. it must evaluates to either true or false
 * <cmd> can be any of { print, abort, exit, return, goto } which are executed in-order in case the assertion fails
 *       Some commands take a parameter which is separated from the command name by a space, e.g. `return -1`
 * 
 * Only a few assertions deviate from this ruleset by not taking a <test> parameter, e.g. assert_fail
 * 
 * Default values: 
 * 
 * TODO: rename to assert_const_fail or assert_static_fail?
 */
