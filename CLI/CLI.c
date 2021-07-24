#include "instance.h";
#include "parser.h"

int main(int argc, char *argv[])
{
    const char input[] = "Employee(0, \"Jack Walls\", 8360, 1)";
    const statement stmt = parse(input, sizeof(input));

    parser_dump(stmt);

    return EXIT_SUCCESS;
}
