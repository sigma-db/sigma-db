#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "test.h"

TEST(create)
{
    hashmap *map;
    int      error = hashmap_create(&map, 7, strcmp);
    EXPECT(!error && map != NULL);

    hashmap_destroy(map);
}

TEST(set)
{
    const char *key = "first_name";
    const char *value;

    hashmap *map;
    hashmap_create(&map, 7, strcmp);

    hashmap_set(map, key, "David");
    value = hashmap_get(map, key);
    EXPECT(strcmp(value, "David") == 0);

    hashmap_set(map, key, "Luis");
    value = hashmap_get(map, key);
    EXPECT(strcmp(value, "Luis") == 0);

    hashmap_destroy(map);
}

TEST(expand)
{
    hashmap *map;
    hashmap_create(&map, 1, strcmp); // Allocates space for 2^(1 + 1) = 4 slots

    /* Expansion is assumed to occur if prior to an insertion, >50% of available slots are occupied
     * In our case, this must be after the 3rd insertion, i.e. upon the 4th one.
     */

    char   keys[5][3];
    size_t cnt = sizeof(keys) / sizeof(keys[0]);

    // Generate keys
    for (int i = 0; i < cnt; i++) {
        snprintf(keys[i], 3, "k%d", i + 1);
    }

    // Use keys to set hashmap entries
    for (int i = 0; i < cnt; i++) {
        hashmap_set(map, keys[i], "value");
    }

    // If all previously inserted keys exist, the expansion was successful
    for (int i = 0; i < cnt; i++) {
        EXPECT(hashmap_has(map, keys[i]));
    }

    hashmap_destroy(map);
}

SUITE(test_hashmap)
{
    RUN(create, set, expand);
}
