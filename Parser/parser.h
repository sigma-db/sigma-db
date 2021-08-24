#pragma once

#include <stdlib.h>

typedef struct parser_context *parser_context_t;

typedef struct statement *statement_t;

int parser_create(parser_context_t *ctx, const char *qstr, size_t qstr_len);

void parser_destroy(parser_context_t ctx);

statement_t parser_next(parser_context_t ctx);
