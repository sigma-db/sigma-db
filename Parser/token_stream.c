#include <string.h>

#include "char_stream.h"
#include "token_stream.h"
#include "ctype.h"

struct token_stream {
    struct char_stream *cs;
};

static int parse_string(struct token_stream *ts, const char **str);
static void skip_whitespace(struct token_stream *ts);

int token_stream_create(struct token_stream **ts, const char *buf, size_t buf_len)
{
    *ts = malloc(sizeof(struct token_stream));
    if (*ts == NULL) {
        return -1;
    }
    int err = char_stream_create(&(*ts)->cs, buf, buf_len);
    if (err != 0) {
        free(ts);
        return -1;
    }
    return 0;
}

void token_stream_destroy(struct token_stream *ts)
{
    char_stream_destroy(ts->cs);
    free(ts);
}

struct token token_stream_next(struct token_stream *ts)
{
    size_t length = 1;
    char32_t c = char_stream_next(ts->cs);
    switch (character_class(c)) {
    case CC_DIGIT:
        while (isdigit(qstr[length])) length++;
        *token_type = TK_LITERAL_INTEGER;
        break;
    case CC_NULL:
        *token_type = TK_END;
        break;
    case CC_OTHER:
        length = 0;
        *token_type = TK_ERROR;
        break;
    }
    return length;
}

struct token token_stream_peek(struct token_stream *ts)
{
    const size_t before = char_stream_pos(ts->cs);
    const struct token result = token_stream_next(ts);
    const int offset = (int)(char_stream_pos(ts->cs) - before);
    char_stream_advance(ts->cs, -offset);
    return result;
}

inline bool token_stream_end(struct token_stream *ts)
{
    return token_stream_peek(ts).type == EOF;
}

static int parse_string(struct token_stream *ts, const char **str)
{
    struct char_stream *cs = ts->cs;
    skip_whitespace(ts);
    if (!char_stream_has_more(cs) && char_stream_peek_next(cs) == '"') {
        while 
    }
}

static void skip_whitespace(struct token_stream *ts)
{
    struct char_stream *cs = ts->cs;
    while (char_stream_has_more(cs)) {
        sigma_char_t c = char_stream_next(cs);
        if (!isspace(c)) {
            break;
        }
    }
}
