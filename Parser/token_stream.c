#include <string.h>

#include "char_stream.h"
#include "token_stream.h"
#include "ctype.h"

#define simple_token(tok_type) (struct token){ .type = tok_type }
#define value_token(tok_type, tok_length, ...) (struct token){ .type = tok_type, .length = tok_length, .value = { __VA_ARGS__ } }
#define reference_token(tok_type, tok_length, tok_reference) (struct token){ .type = tok_type, .length = tok_length, .reference = tok_reference }

struct token_stream {
    struct char_stream *chars;
};

typedef struct {
    size_t capacity;
    size_t length;
    sigma_char_t *chars;
} string;

static inline const string string_create(size_t capacity)
{
    sigma_char_t *chars = malloc(sizeof(sigma_char_t) * capacity);
    string str = {
        .capacity = capacity,
        .length = 0,
        .chars = chars,
    };
    return str;
}

static int string_append_char(string *str, sigma_char_t c)
{
    if (str->length == str->capacity) {
        // We double the capacity when running out of space
        sigma_char_t chars_new = realloc(str->chars, str->capacity * 2);
        if (chars_new == NULL) {
            return -1;
        }
        str->capacity *= 2;
        str->chars = chars_new;
    }
    str->chars[str->length++] = c;
    return 0;
}

const struct token error_token = { .type = TK_ERROR };

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
    struct token tok = { .type = TK_END, .value = { 0 } };

    char32_t c = char_stream_next(ts->chars);
    switch (character_class(c)) {
    case CC_DIGIT:
        const string num_str = string_create(MAX_TOKEN_LENGTH);
        string_append_char(&num_str, c);
        while (char_stream_has_more(ts->chars) && isdigit(char_stream_peek(ts->chars, 1))) {
            string_append_char(&num_str, char_stream_next(ts->chars));
        }
        return reference_token(TK_LITERAL_INT, num_str.length, num_str.chars);

    case CC_ALPHA:
        break;

    case CC_SPACE:
        while (char_stream_has_more(ts->chars) && isspace(char_stream_peek(ts->chars, 1))) {
            char_stream_next(ts->chars);
        }
        return simple_token(TK_SPACE);

    case CC_QUOTE:
        if (c == '"') {
            const string str = string_create(MAX_TOKEN_LENGTH);
            while ((c = char_stream_next(ts->chars)) != '"') {
                string_append_char(&str, c);
            }
            return reference_token(TK_LITERAL_STRING, str.length, str.chars);
        }
        if (c == '\'') {
            if (char_stream_peek(ts->chars, 2) == '\'') {
                c = char_stream_next(ts->chars);
                char_stream_next(ts->chars);
                return value_token(TK_LITERAL_CHAR, 1, c);
            }
        }
        break;

    case CC_OTHER:
        break;
    }

exit_error:
    return error_token;
}

struct token token_stream_peek(struct token_stream *ts, size_t k)
{
    const size_t before = char_stream_pos(ts->chars);
    const struct token result = token_stream_next(ts);
    const int offset = (int)(char_stream_pos(ts->chars) - before);
    char_stream_advance(ts->chars, -offset);
    return result;
}

bool token_stream_has_more(struct token_stream *ts)
{
    return char_stream_has_more(ts->chars);
}
