#include <stdlib.h>

#include "parser.h"
#include "statement.h"

statement parse(const char *str, size_t str_len)
{
    statement stmt;

    //return stmt;
}

void parser_dump(statement stmt)
{
    switch (stmt->type) {
    case CREATE:
        break;

    case INSERT:
        struct insert_statement insert_stmt = stmt->insert;
        size_t str_len = insert_stmt.rel_name_len + 3;
        //const char *str = malloc();
        break;

    case SELECT:
        break;

    case INFO:
        break;

    case DUMP:
        break;
    }
}
