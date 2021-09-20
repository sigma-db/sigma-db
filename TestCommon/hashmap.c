#include <string.h>

#include "test.h"
#include "hashmap.h"

static hashmap *map;

TEST(init)
{
    int error = hashmap_create(&map, 7, strcmp);
    EXPECT(!error && map != NULL);
}

TEST(set)
{
    const char *key = "first_name";

    hashmap_set(map, key, "David");
    EXPECT(hashmap_has(map, key));

    const char *value = hashmap_get(map, key);
    EXPECT(strcmp(value, "David") == 0);
}

SUITE(test_hashmap)
{
    RUN(init, set);
}
