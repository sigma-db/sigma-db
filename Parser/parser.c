#include <stdlib.h>

#include "parser.h"

#include "token_stream.h"

struct parser_context {
    struct token_stream *tokens;
};

int parser_create(struct parser_context **ctx, const char *qstr, size_t qstr_len)
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

void parser_destroy(struct parser_context *ctx)
{
    free(ctx);
}

struct statement *parser_next(struct parser_context *ctx)
{
    struct token token = token_stream_next(ctx->tokens);

    struct statement result;

    //switch (token.type) {
    //case TK_LITERAL_INTEGER:
    //    int value = ctx->input[0] - '0';
    //    for (int i = 1; i < tk_len; i++) {
    //        value = value * 10 + ctx->input[i] - '0';
    //    }
    //    result = (struct statement){ .type = tk_type, .as_int = value };
    //    break;
    //case TK_END:
    //    result = (struct statement){ .type = tk_type, .as_int = 0 };
    //    break;
    //}

    //ctx->input += tk_len;
    return &result;
}

bool parser_has_more(struct parser_context *ctx)
{
    return false;
}

static size_t skip_whitespace(struct parser_context *ctx)
{
    if (token_stream_peek(ctx->tokens, 1).type == TK_SPACE) {
        token_stream_next(ctx->tokens);
        return 1;
    }
    return 0;
}

static size_t skip_comment(struct parser_context *ctx)
{
    //char *start = ctx->current;
    //char c = *ctx->current;
    //if (c == '%') {
    //    do {
    //        c = *++ctx->current;
    //    } while (c != '\n' && c != 0);
    //    if (c == '\n') ctx->current++;
    //}
    //return ctx->current - start;
    return 0;
}

static inline void skip_ignored(struct parser_context *ctx)
{
    size_t cnt;
    do {
        cnt = skip_whitespace(ctx) + skip_comment(ctx);
    } while (cnt > 0);
}
