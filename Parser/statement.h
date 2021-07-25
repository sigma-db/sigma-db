#pragma once

#include <string.h>

#include "../Common/array.h"
#include "../Common/string.h"

const enum statement_type { CREATE, INSERT, SELECT, INFO, DUMP };
const enum tuple_type { NAMED, UNNAMED };
const enum value_type { VARIABLE, LITERAL };

struct value {
    enum value_type type;
    union {
        void *value;
        string variable;
    };
};

struct named_tuple {
    void *val;
};

struct unnamed_tuple {
    void *val;
};

struct tuple {
    enum tuple_type type;
    union {
        struct named_tuple named;
        struct unnamed_tuple unnamed;
    };
};

struct create_statement {
    void *val;
};

struct insert_statement {
    const char *rel_name;
    const size_t rel_name_len;
    struct tuple tuple;
};

struct select_statement {
    void *val;
};

struct info_statement {
    void *val;
};

struct dump_statement {
    void *val;
};

struct statement {
    enum statement_type type;
    union {
        struct create_statement as_create;
        struct insert_statement as_insert;
        struct select_statement as_select;
        struct info_statement as_info;
        struct dump_statement as_dump;
    };
};
