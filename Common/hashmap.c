#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "hashmap.h"

#define GROWTH_FACTOR 2

struct slot {
    bool        taken;
    size_t      hash;
    const void *key;
    const void *val;
};

struct hashmap {
    comparator_f cmp;
    size_t       capacity;
    size_t       items_count;
    struct slot *items;
    char *       strings;
};

static struct slot *find_slot(struct hashmap *map, size_t hash, const void *key);
static void         expand(struct hashmap *map);
static uint64_t     hash_fnv(const void *key, size_t key_sz);

int hashmap_create(struct hashmap **map, size_t capacity, comparator_f cmp)
{
    *map = malloc(sizeof(struct hashmap));
    if (*map == NULL) {
        return -1;
    }
    capacity = (capacity + 1) << 1; // prevent "calloc(0)"

    /* clang-format off */
    **map = (struct hashmap){
        .cmp         = cmp,
        .items_count = 0,
        .capacity    = capacity,
        .items       = calloc(capacity, sizeof(struct slot)),
        .strings     = malloc(capacity * 16),
    };
    /* clang-format on */

    return 0;
}

void hashmap_destroy(struct hashmap *map)
{
    free(map->items);
}

void hashmap_set(struct hashmap *map, const void *key, const void *val)
{
    if (map->items_count > (map->capacity >> 1)) {
        // expand, if more then half full
        expand(map);
    }

    size_t       hash = hash_fnv(key, 0);
    struct slot *slot = find_slot(map, hash, key);

    if (!slot->taken) {
        slot->taken = true;
        map->items_count++; // do only when slot was empty
    }

    slot->hash = hash;
    slot->key  = key;
    slot->val  = val;
}

const void *hashmap_get(struct hashmap *map, const void *key)
{
    size_t       hash = hash_fnv(key, 0);
    struct slot *slot = find_slot(map, hash, key);

    return slot != NULL && slot->taken ? slot->val : NULL;
}

const void* hashmap_get_or_default(hashmap* map, const void* key, const void* default_val) {
    const void *val = hashmap_get(map, key);
    return val != NULL ? val : default_val;
}

int hashmap_has(struct hashmap *map, const void *key)
{
    return hashmap_get(map, key) != NULL;
}

static int
equal(struct hashmap *map, uint64_t hash1, const void *key1, uint64_t hash2, const void *key2)
{
    return hash1 == hash2 && key1 != NULL && key2 != NULL && map->cmp(key1, key2) == 0;
}

static struct slot *find_slot(struct hashmap *map, size_t hash, const void *key)
{
    size_t       pos  = hash % map->capacity;
    size_t       j    = 0;
    struct slot *slot = map->items + pos;

    for (size_t i = 0; i < map->capacity; i++) {
        /*
         * Since entries can *not* be deleted, it is safe to assume that when while probing an empty
         * spot is found, the provided key is not currently in the hashmap.
         */

        if (equal(map, slot->hash, slot->key, hash, key) || !slot->taken) {
            return slot;
        }
        j += j + 1; // quadratic probing: same as probe(j) = j + j^2
        size_t p = (pos + j) % map->capacity;
        slot     = map->items + p;
    }

    return NULL;
}

static void expand(struct hashmap *map)
{
    struct slot *items_old    = map->items;
    size_t       capacity_old = map->capacity;

    map->capacity *= GROWTH_FACTOR;
    map->items = calloc(map->capacity, sizeof(struct slot));

    for (size_t i = 0; i < capacity_old; i++) {
        struct slot slot                     = items_old[i];
        *find_slot(map, slot.hash, slot.key) = slot;
    }

    free(items_old);
}

static uint64_t hash_fnv(const void *key, size_t key_sz)
{
    /**
     * Code adapted from http://www.isthe.com/chongo/tech/comp/fnv/
     */

    uint64_t hash = 0xCBF29CE484222325;
    for (const uint8_t *c = key; *c != 0 && key_sz > 0; c++) {
        hash = (hash ^ *c) * 0x100000001B3;
    }
    return hash;
}
