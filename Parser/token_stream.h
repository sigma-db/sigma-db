#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H

#include <stdlib.h>
#include <stdbool.h>

#include "char_stream.h"

#define LEXER_LOOKAHEAD 2
#define MAX_TOKEN_LENGTH 16

enum token_type {
    TK_ERROR,

    // brackets
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACKET,
    TK_RBRACKET,
    TK_RBRACE,
    TK_LBRACE,

    // literal
    TK_LITERAL_INT,
    TK_LITERAL_STRING,
    TK_LITERAL_CHAR,

    // ignored
    TK_SPACE,
    TK_COMMENT,
    TK_END,
};

struct token {
    enum token_type type;
    size_t length;
    union {
        sigma_char_t value[MAX_TOKEN_LENGTH];
        sigma_char_t *reference;
    };
};

struct token_stream;

extern const struct token error_token;

int token_stream_create(struct token_stream **ts, const char *buf, size_t bufsz);

void token_stream_destroy(struct token_stream *ts);

struct token token_stream_next(struct token_stream *ts);

struct token token_stream_peek(struct token_stream *ts, size_t k);

bool token_stream_has_more(struct token_stream *ts);

#endif // ! TOKEN_STREAM_H
