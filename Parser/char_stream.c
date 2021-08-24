#include <stdlib.h>
#include <assert.h>

#include "char_stream.h"

// Checks that value != 0 and `end != NULL -> current < end`
// That is, if the string's size is known in advance, the current position must be before the end
#define stream_has_more(it) \
    (((it)->value != 0) && ((it)->end == NULL || (it)->current < (it)->end))

#define assert_iteration_invariant(it) \
    assert((it)->begin <= (it)->current && ((it)->end == NULL || (it)->current < (it)->end))

struct circular_queue {
    size_t pos;
    sigma_char_t values[CHAR_STREAM_LOOKAHEAD];
};

struct char_stream {
    // iterator fields
    const char *begin;
    const char *end;
    const char *current; // invariant: begin <= buf < end (with end = 0 meaning infty)

    // char stream-specific fields
    sigma_char_t value; // most recently read code point
    struct circular_queue lookahead;
};

static sigma_char_t next(struct char_stream *cs);

static sigma_char_t circular_queue_add(struct circular_queue *q, sigma_char_t c);

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
        .value = -1,
        .lookahead = {
            .pos = 0,
            .values = { 0 },
        },
    };

    assert_iteration_invariant(*cs);

    // After this, the lookahead array will be filled and the cursor will point to the first element
    char_stream_advance(cs, CHAR_STREAM_LOOKAHEAD);

    return 0;
}

void char_stream_destroy(struct char_stream *cs)
{
    free(cs);
}

sigma_char_t char_stream_next(struct char_stream *cs)
{
    cs->value = circular_queue_add(&cs->lookahead, next(cs));
    return cs->value;
}

sigma_char_t char_stream_current(struct char_stream *cs)
{
    return cs->value;
}

sigma_char_t char_stream_peek(struct char_stream *cs, size_t k)
{
    assert(0 < k && k <= CHAR_STREAM_LOOKAHEAD);

    size_t p = (cs->lookahead.pos + k - 1) % CHAR_STREAM_LOOKAHEAD;
    return cs->lookahead.values[p];
}

size_t char_stream_advance(struct char_stream *cs, size_t delta)
{
    uint8_t i = 0; // number of code points read
    while (i < delta && stream_has_more(cs)) {
        if (circular_queue_add(&cs->lookahead, next(cs)) < 0) {
            goto exit;
        }
        i += 1;
    }
    // fill remaining entries with -1
    while (i < delta) {
        circular_queue_add(&cs->lookahead, -1);
        i += 1;
    }

exit:
    assert_iteration_invariant(cs);
    return char_stream_pos(cs);
}

size_t char_stream_pos(struct char_stream *cs)
{
    return cs->current - cs->begin;
}

bool char_stream_has_more(struct char_stream *cs)
{
    return stream_has_more(cs);
}

// returns the next byte from the input and forwards by one byte
static char next_char(struct char_stream *cs)
{
    assert(stream_has_more(cs));
    return *cs->current++;
}

static sigma_char_t next(struct char_stream *cs)
{
    // 1 byte (ASCII)
    sigma_char_t c = next_char(cs);
    if (c < 0x80) {
        return c;
}
#ifdef UNICODE
    // 2 bytes
    c = (c << 6) | (next_char(cs) & 0x3F);
    if (c < 0x00003800) {
        return c & 0x000007FF; // (1 << 11) - 1
    }
    // 3 bytes
    c = (c << 6) | (next_char(cs) & 0x3F);
    if (c < 0x000F0000) {
        return c & 0x0000FFFF; // (1 << 16) - 1
    }
    // 4 bytes
    c = (c << 6) | (next_char(cs) & 0x3F);
    if (c < 0x03E00000) {
        return c & 0x001FFFFF; // (1 << 21) - 1
    }
    cs->current -= 4; // undo reading the last 4 bytes
#else
    cs->current -= 1; // undo reading the last byte
#endif // UNICODE

    assert(false); // we should not ever get here on valid input
    return -1; // get stuck in an invalid state
}

static sigma_char_t circular_queue_add(struct circular_queue *q, sigma_char_t c)
{
    const sigma_char_t old = q->values[q->pos];
    q->values[q->pos] = c;
    q->pos = (q->pos + 1) % CHAR_STREAM_LOOKAHEAD;
    return old;
}
