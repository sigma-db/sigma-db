#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "statement.h"

int main(int argc, char *argv[])
{
    const char input[] = "Employee(0, \"Jack Walls\", 8360, 1)";

    parser_context_t *ctx;
    parser_create(&ctx, input, sizeof input);

    while (!parser_end(ctx)) {
        statement_t stmt = parser_next(ctx);
        const char *stmt_str = statement_tostring(stmt);
        printf("%s\r\n", stmt_str);
    }

    return EXIT_SUCCESS;
}
