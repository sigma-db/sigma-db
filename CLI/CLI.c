#include "instance.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    const char input[] = "Employee(0, \"Jack Walls\", 8360, 1)";

    parser_context *ctx;
    parser_create(&ctx, input, sizeof(input));

    while (!parser_end(ctx)) {
        statement stmt = parser_next(ctx);
        parser_dump(stmt);
    }

    return EXIT_SUCCESS;
}
