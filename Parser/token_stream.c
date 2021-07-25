#include <stdlib.h>
#include <stdbool.h>

#include "char_stream.h"
#include "token_stream.h"

struct token_stream {
    struct char_stream *cs;
};

int ts_init(struct token_stream **ts, const char *buf, size_t buf_len)
{
    *ts = malloc(sizeof(struct token_stream));
    if (*ts == NULL) {
        return EXIT_FAILURE;
    }
    int err = cs_init(&(*ts)->cs, buf, buf_len);
    if (err != EXIT_SUCCESS) {
        free(ts);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

struct token ts_next(struct token_stream *ts)
{
    //return 0;
}

struct token ts_peek(struct token_stream *ts)
{
    const size_t before = cs_pos(ts->cs);
    const struct token result = ts_next(ts);
    const int offset = (int)(cs_pos(ts->cs) - before);
    cs_seek(ts->cs, -offset);
    return result;
}

inline bool ts_end(struct token_stream *ts)
{
    return ts_peek(ts).type == EOF;
}
