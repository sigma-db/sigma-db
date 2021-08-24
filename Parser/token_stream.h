#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H

#include <stdlib.h>
#include <stdbool.h>

enum token_type {
    TK_ERROR,
    TK_LITERAL_INTEGER,
    TK_END
};

struct token {
    enum token_type type;
    void *data;
};

struct token_stream;

int token_stream_create(struct token_stream **ts, const char *buf, size_t buf_len);

void token_stream_destroy(struct token_stream *ts);

struct token token_stream_next(struct token_stream *ts);

struct token token_stream_peek(struct token_stream *ts);

bool token_stream_end(struct token_stream *ts);

#endif // ! TOKEN_STREAM_H
