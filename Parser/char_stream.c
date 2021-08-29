#include <stdlib.h>
#include <assert.h>

#include "char_stream.h"

#define iteration_invariant(it) \
    ((it)->begin <= (it)->current && ((it)->end == NULL || (it)->current < (it)->end))

// Checks whether `*current != 0` and `end != NULL -> current < end` holds.
// 
// That is, the current value must not be '\0' and if the string's size 
// is known in advance, the current position must be strictly before the end.
#define iterator_has_more(it) \
    ((*(it)->current != 0) && ((it)->end == NULL || (it)->current < (it)->end))

#define index(cs, k) \
    (((cs)->pos + k) % CHAR_STREAM_LOOKAHEAD)

#define next(cs) \
    ((cs)->cache[(cs)->pos])

#ifdef PARSER_UTF8_ENABLED
#   define code_point_size(c) (c < 0x80 ? 1 : c < 0x800 ? 2 : c < 0x10000 ? 3 : 4)
#else
#   define code_point_size(c) 1
#endif // PARSER_UTF8_ENABLED

struct char_stream {
    // iterator fields
    const char *begin;
    const char *end;
    const char *current; // invariant: begin <= buf < end (with end = 0 meaning infty)

    // char stream-specific fields
    size_t pos; // index into `cache`, i.e. 0 <= pos < CHAR_STREAM_LOOKAHEAD
    sigma_char_t cache[CHAR_STREAM_LOOKAHEAD];
};

static void advance(struct char_stream *cs);
static sigma_char_t next_code_point(struct char_stream *cs);

int char_stream_create(struct char_stream **cs, const char *buf, size_t bufsz)
{
    assert(cs != NULL);
    assert(buf != NULL);

    *cs = malloc(sizeof(struct char_stream));
    if (*cs == NULL) {
        return -1;
    }
    **cs = (struct char_stream){
        .begin = buf,
        .end = bufsz > 0 ? buf + bufsz : NULL,
        .current = buf,
        .pos = 0,
    };

    assert(iteration_invariant(*cs));

    char_stream_advance(*cs, CHAR_STREAM_LOOKAHEAD);

    return 0;
}

void char_stream_destroy(struct char_stream *cs)
{
    free(cs);
}

sigma_char_t char_stream_next(struct char_stream *cs)
{
    assert(next(cs) > 0);

    sigma_char_t value = next(cs);
    advance(cs);
    return value;
}

sigma_char_t char_stream_peek(struct char_stream *cs, size_t k)
{
    assert(0 < k && k <= CHAR_STREAM_LOOKAHEAD);

    return cs->cache[index(cs, k - 1)];
}

void char_stream_advance(struct char_stream *cs, size_t delta)
{
    while (delta-- > 0) {
        advance(cs);
    }
}

size_t char_stream_pos(struct char_stream *cs)
{
    // Sum size of all currently cached coint points, excluding terminating null character(s) if present
    size_t sum = 0;
    for (size_t i = 0; i < CHAR_STREAM_LOOKAHEAD; i++) {
        sigma_char_t cp = cs->cache[index(cs, i)];
        if (cp <= 0) {
            break; // stop after encountering the first null character (or error)
        }
        sum += code_point_size(cp);
    }
    // Subtract widths of all cached code points from (cs->current - cs->begin)
    return cs->current - cs->begin - sum;
}

bool char_stream_has_more(struct char_stream *cs)
{
    return next(cs) > 0;
}

static void advance(struct char_stream *cs)
{
    cs->cache[cs->pos] = iterator_has_more(cs) ? next_code_point(cs) : 0;
    cs->pos = index(cs, 1);

    // in case we're done, we must be exactly one past the last element
    assert(iteration_invariant(cs) || cs->current == cs->end);
}

// Returns the next byte from the input and forwards by one byte
static char next_byte(struct char_stream *cs)
{
    assert(iterator_has_more(cs));

    return *cs->current++;
}

static sigma_char_t next_code_point(struct char_stream *cs)
{
    // 1 byte (ASCII)
    sigma_char_t c = next_byte(cs);
    if (c < 0x80) { // BUG: always true since 0x80 == -128 (make next_byte return unsigned char?)
        return c;
    }
#ifdef PARSER_UTF8_ENABLED
    // 2 bytes
    c = (c << 6) | (next_byte(cs) & 0x3F);
    if (c < 0x00003800) {
        return c & 0x000007FF; // (1 << 11) - 1
    }
    // 3 bytes
    c = (c << 6) | (next_byte(cs) & 0x3F);
    if (c < 0x000F0000) {
        return c & 0x0000FFFF; // (1 << 16) - 1
    }
    // 4 bytes
    c = (c << 6) | (next_byte(cs) & 0x3F);
    if (c < 0x03E00000) {
        return c & 0x001FFFFF; // (1 << 21) - 1
    }
    cs->current -= 4; // undo reading the last 4 bytes
#else
    cs->current -= 1; // undo reading the last byte
#endif // PARSER_UTF8_ENABLED
    assert(false); // should not ever be reached on valid input
    return -1; // get stuck in an invalid state
}
