#pragma once

#include <stdlib.h>

#include "array.h"

struct array {
    size_t stride;
    size_t len;
    char elements[];
};
//
//inline void *array_get(array *arr, size_t pos)
//{
//    return &arr->elements[pos * arr->stride];
//}
