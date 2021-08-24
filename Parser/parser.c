#include <stdlib.h>

#include "parser.h"
#include "statement.h"
#include "token_stream.h"

struct parser_context {
    struct token_stream *tokens;
};

int parser_create(parser_context_t *ctx, const char *qstr, size_t qstr_len)
{
    *ctx = malloc(sizeof(struct parser_context));
    if (*ctx == NULL) {
        return -1;
    }
    struct token_stream *tokens;
    int error = token_stream_create(&tokens, qstr, qstr_len);
    if (error) {
        free(*ctx);
        return -1;
    }
    **ctx = (struct parser_context){
        .tokens = tokens
    };
    return 0;
}

void parser_destroy(parser_context_t ctx)
{
    free(ctx);
}

statement_t parser_next(parser_context_t ctx)
{
    struct token token = token_stream_next(ctx->tokens);

    struct statement result;

    switch (token.type) {
    case TK_LITERAL_INTEGER:
        int value = ctx->input[0] - '0';
        for (int i = 1; i < tk_len; i++) {
            value = value * 10 + ctx->input[i] - '0';
        }
        result = (struct statement){ .type = tk_type, .as_int = value };
        break;
    case TK_END:
        result = (struct statement){ .type = tk_type, .as_int = 0 };
        break;
    }

    ctx->input += tk_len;
    return result;
}

static void skip_whitespace(struct context *ctx)
{
    char c = *ctx->current;
    while (isspace(c) && c != 0) {
        c = *++ctx->current;
    }
}

static size_t skip_comment(struct context *ctx)
{
    char *start = ctx->current;
    char c = *ctx->current;
    if (c == '%') {
        do {
            c = *++ctx->current;
        } while (c != '\n' && c != 0);
        if (c == '\n') ctx->current++;
    }
    return ctx->current - start;
}

static inline void skip_ignored(struct context *ctx)
{
    do {
        skip_whitespace(ctx);
    } while (skip_comment(ctx) > 0);
}
