#include <stdlib.h>

#include "parser.h"
#include "statement.h"
#include "token_stream.h"

statement parse(const char *qstr, size_t qstr_len)
{
    struct token_stream *ts;
    ts_init(&ts, qstr, qstr_len);

    while (!ts_end(ts)) {
        struct token tok = ts_next(ts);
    }
    
    
    //return stmt;
}

void parser_dump(statement stmt)
{
    switch (stmt->type) {
    case CREATE:
        break;

    case INSERT:
        struct insert_statement insert_stmt = stmt->as_insert;
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
