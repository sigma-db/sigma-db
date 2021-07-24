#pragma once

#include <stdint.h>
#include <stdbool.h>

struct token {
    void *data;
};

struct token_stream;

int lex_init(struct token_stream **ts, const char *buf, size_t buf_len);

int32_t lex_next(struct token_stream *ts);

int32_t lex_peek(struct token_stream *ts, size_t k);

bool lex_end(struct token_stream *ts);
