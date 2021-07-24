#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"
#include "char_stream.h"

struct token_stream {
    struct char_stream *cs;
};

int lex_init(struct token_stream **ts, const char *buf, size_t buf_len)
{
    *ts = malloc(sizeof(struct token_stream));
    if (*ts == NULL) {
        return EXIT_FAILURE;
    }
    int err = cs_init((*ts)->cs, buf, buf_len);
    if (err != EXIT_SUCCESS) {
        free(ts);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int32_t lex_next(struct token_stream *ts)
{
    return 0;
}

int32_t lex_peek(struct token_stream *ts, size_t k)
{
    return 0;
}

bool lex_end(struct token_stream *ts)
{
    return false;
}
