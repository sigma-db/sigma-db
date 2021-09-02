#include <string.h>

#include "char_stream.h"
#include "token_stream.h"
#include "ctype.h"

#define simple_token(tok_type) (struct token){ .type = tok_type }
#define value_token(tok_type, tok_length, ...) (struct token){ .type = tok_type, .length = tok_length, .value = { __VA_ARGS__ } }
#define str2tok(tok_type, str) (struct token){ .type = tok_type, .length = str.length, .reference = str.chars }

struct token_stream {
    struct char_stream *chars;
};

typedef struct {
    size_t capacity;
    size_t length;
    sigma_char_t *chars;
} string;

#define str_create(sz) { .capacity = sz, .length = 0, .chars = malloc(sizeof(sigma_char_t) * sz) }

static void str_append(string *str, sigma_char_t c)
{
    if (str->length == str->capacity) {
        // We double the capacity when running out of space
        sigma_char_t chars_new = realloc(str->chars, str->capacity * 2);
        if (chars_new == NULL) {
            return;
        }
        str->capacity *= 2;
        str->chars = chars_new;
    }
    str->chars[str->length++] = c;
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
        string digit_str = str_create(MAX_TOKEN_LENGTH);
        str_append(&digit_str, c);
        while (char_stream_has_more(ts->chars) && isdigit(char_stream_peek(ts->chars, 1))) {
            str_append(&digit_str, char_stream_next(ts->chars));
        }
        return str2tok(TK_LITERAL_INT, digit_str);

    case CC_ALPHA:
        break;

    case CC_SPACE:
        while (char_stream_has_more(ts->chars) && isspace(char_stream_peek(ts->chars, 1))) {
            char_stream_next(ts->chars);
        }
        return simple_token(TK_SPACE);

    case CC_QUOTE:
        if (c == '"') {
            string str = str_create(MAX_TOKEN_LENGTH);
            while ((c = char_stream_next(ts->chars)) != '"') {
                str_append(&str, c);
            }
            return str2tok(TK_LITERAL_STRING, str);
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
    return error_token;
}

bool token_stream_has_more(struct token_stream *ts)
{
    return char_stream_has_more(ts->chars);
}
