#pragma once

#include <stdlib.h>

typedef struct {
    void *elements;
    size_t element_sz;
    size_t len;
} array;

#define array_type(type) \
    typedef struct { \
         type *elements; \
         size_t len; \
    } array_ ## type; \
    \
    inline type array_ ## type ## _at(array_ ## type arr, size_t pos) { \
        return arr.elements[pos]; \
    }

#define array_at(arr, pos) (arr.elements[pos])

array_type(int);
int test()
{
    array_int arr;
    int a = array_at(arr, 2);
}
