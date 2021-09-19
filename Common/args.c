#include <string.h>

#include "args.h"

hashmap *parse_args(int argc, char *argv[])
{
    hashmap *args;
    hashmap_create(&args, 4, strcmp);
    for (size_t i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (strncmp(arg, "--", sizeof("--") - 1) != 0) {
            continue; // ignore malformed args
        }
        char *kv_sep = strchr(arg, '=');
        if (kv_sep == NULL) {
            // boolean argument
        } else {
            *kv_sep     = 0;
            char *key   = arg + 2; // skip leading "--"
            char *value = kv_sep + 1;
            hashmap_set(args, key, value);
        }
    }
    return args;
}
