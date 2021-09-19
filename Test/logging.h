#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <string.h>

#include "style.h"
#include "symbols.h"

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

#endif // !LOGGING_H
