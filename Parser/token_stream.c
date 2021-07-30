#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "../Common/assert.h"
#include "char_stream.h"
#include "token_stream.h"

struct token_stream {
    struct char_stream *cs;
};

static int ts_parse_string(struct token_stream *ts, const char **str);
static void ts_skip_whitespace(struct token_stream *ts);

int ts_init(struct token_stream **ts, const char *buf, size_t buf_len)
{
    *ts = malloc(sizeof(struct token_stream));
    if (*ts == NULL) {
        return -1;
    }
    int err = cs_init(&(*ts)->cs, buf, buf_len);
    if (err != 0) {
        free(ts);
        return -1;
    }
    return 0;
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

static int ts_parse_string(struct token_stream *ts, const char **str)
{
    struct char_stream *cs = ts->cs;
    ts_skip_whitespace(ts);
    if (!cs_end(cs) && cs_peek_next(cs) == '"') {
    }
}

static void ts_skip_whitespace(struct token_stream *ts)
{
    struct char_stream *cs = ts->cs;
    while (!cs_end(cs)) {
        sigma_char c = cs_next(cs);
        if (!isspace(c)) {
            cs_seek(cs, -1);
            break;
        }
    }
}
