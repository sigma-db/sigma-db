#pragma once

#include <stdbool.h>

enum token_type { EOF };

struct token {
    enum token_type type;
    void *data;
};

struct token_stream;

int ts_init(struct token_stream **ts, const char *buf, size_t buf_len);

struct token ts_next(struct token_stream *ts);

struct token ts_peek(struct token_stream *ts);

bool ts_end(struct token_stream *ts);
