#include "statement.h"

const char *statement_tostring(struct statement *stmt)
{
    switch (stmt->type) {
    case CREATE:
        break;

    case INSERT:
        struct insert_statement insert_stmt = stmt->as_insert;
        //size_t str_len = insert_stmt.rel_name_len + 3;
        //const char *str = malloc();
        break;

    case SELECT:
        break;

    case INFO:
        break;

    case DUMP:
        break;
    }

    return "<statement>";
}
