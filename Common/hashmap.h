#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

typedef struct hashmap hashmap;

typedef int (*comparator_f)(const void *, const void *);

int hashmap_create(hashmap **map, size_t capacity_exp, comparator_f cmp);

void hashmap_destroy(hashmap *map);

void hashmap_set(hashmap *map, const void *key, const void *val);

const void *hashmap_get(hashmap *map, const void *key);

const void *hashmap_get_or_default(hashmap *map, const void *key, const void *default_val);

int hashmap_has(hashmap *map, const void *key);

#endif // !HASHMAP_H
