#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdbool.h>

#include "statement.h"

struct parser_context;

int parser_create(struct parser_context **ctx, const char *qstr, size_t qstr_len);

void parser_destroy(struct parser_context *ctx);

struct statement *parser_next(struct parser_context *ctx);

bool parser_has_more(struct parser_context *ctx);

// TODO: Create header public/parser.h and code file main.c to implement public (convenience) interface

#endif // !PARSER_H
