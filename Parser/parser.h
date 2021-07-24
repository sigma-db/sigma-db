#pragma once

#include <stdlib.h>

typedef struct statement *statement;

statement parse(const char *str, size_t str_len);

void parser_dump(statement stmt);
