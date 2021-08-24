#pragma once

#include <string.h>

#define MAX_RELATION_NAME_LENGTH 64
#define MAX_RELATION_ARITY 64
#define MAX_VAR_NAME_LENGTH 32

enum statement_type { CREATE, INSERT, SELECT, INFO, DUMP };
enum tuple_type { NAMED, UNNAMED };
enum value_type { VARIABLE, LITERAL };
enum data_type { INT, CHAR, STRING, BOOL };

struct value {
    enum value_type type;
    union {
        void *value;
        const char variable[MAX_VAR_NAME_LENGTH];
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

struct attribute {
    const char *name;
    size_t width;
    enum data_type type;
};

struct create_statement {
    const char *rel_name;
    size_t arity;
    struct attribute attrs[];
};

struct insert_statement {
    const char *rel_name;
    struct tuple tuple;
};

struct select_statement {
    void *val;
};

struct info_statement {
    const char *rel_name;
};

struct dump_statement {
    const char *rel_name;
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

const char *statement_tostring(struct statement *stmt);
