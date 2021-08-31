#include <string.h>

#include "char_stream.h"
#include "token_stream.h"
#include "ctype.h"

struct token_stream {
    struct char_stream *chars;
};

static int parse_string(struct token_stream *ts, const char **str);
static void skip_whitespace(struct token_stream *ts);

int token_stream_create(struct token_stream **ts, const char *buf, size_t bufsz)
{
    *ts = malloc(sizeof(struct token_stream));
    if (*ts == NULL) {
        return -1;
    }
    int error = char_stream_create(&(*ts)->chars, buf, bufsz);
    if (error) {
        free(ts);
        return -1;
    }
    return 0;
}

void token_stream_destroy(struct token_stream *ts)
{
    char_stream_destroy(ts->chars);
    free(ts);
}

struct token token_stream_next(struct token_stream *ts)
{
    size_t length = 1;
    char32_t c = char_stream_next(ts->chars);
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

struct token token_stream_peek(struct token_stream *ts, size_t k)
{
    const size_t before = char_stream_pos(ts->chars);
    const struct token result = token_stream_next(ts);
    const int offset = (int)(char_stream_pos(ts->chars) - before);
    char_stream_advance(ts->chars, -offset);
    return result;
}

inline bool token_stream_has_more(struct token_stream *ts)
{
    return token_stream_peek(ts, 1).type != TK_END;
}

static int parse_string(struct token_stream *ts, const char **str)
{
    struct char_stream *chars = ts->chars;
    skip_whitespace(ts);
    if (char_stream_has_more(chars) && char_stream_peek(chars, 1) == '"') {
        char_stream_next(ts->chars);
        while (char_stream_has_more(chars) && char_stream_peek(chars, 1) != '"') {

        }
    }
}

static void skip_whitespace(struct token_stream *ts)
{
    while (char_stream_has_more(ts->chars)) {
        if (!isspace(char_stream_peek(ts->chars, 1))) {
            break;
        }
        char_stream_next(ts->chars);
    }
}
