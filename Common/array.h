#pragma once

#include <stdlib.h>
#include <stdarg.h>

#define array_define(type) \
    typedef struct { \
         size_t len; \
         type elements[]; \
    } type ## _array; \
    \
    inline type ## _array *type ## _array_init(size_t sz) \
    { \
        type ## _array *arr = malloc(sizeof(type) * sz + sizeof(size_t)); \
        arr->len = sz; \
        return arr; \
    }

#define array_init(type, sz) (type ## _array_init(sz))

#define array_at(arr, pos) (&arr->elements[pos])

#define array_get(arr, pos) (arr->elements[pos])

#define array_set(arr, pos, val) do { arr->elements[pos] = val; }
